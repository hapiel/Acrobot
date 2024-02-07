#include "MovePlayer.h"

MovePlayer::MovePlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp)
    : Debug(Debug), file(file), cp(cp)
{
  limbs[0] = &armL;
  limbs[1] = &armR;
  limbs[2] = &legL;
  limbs[3] = &legR;
}

void MovePlayer::update()
{
  updateMillis = millis();

  if (state == MOVE_TO_BEGIN)
  {
    bool readyToMoveCurves = true;

    for (int i = 0; i < 4; i++)
    {
      if (limbActive(i))
      {

        if (abs(limbs[i]->getTarget() - startPositions[i]) > START_DISTANCE_TOLERANCE)
        {
          readyToMoveCurves = false;
          moveTowardsStart(i);
        }
      }
    }

    // if all limbs are at start position
    if (readyToMoveCurves)
    {
      startCurves();
    }
  }
  else if (state == MOVE_CURVES)
  {
    if (needToReadCurve())
    {
      readCurve();
    }
    uint8_t finishedCurves = 0;

    for (int i = 0; i < 4; i++)
    {
      if (curves[i] != nullptr && limbActive(i))
      {
        if (curves[i]->getStartTimeMs() < moveMillis())
        {
          float target = curves[i]->getValue(moveMillis());
          limbs[i]->setTarget(target, kp, ki);
          // if (i == 1){
          //   Serial.printf("ArmR: %f, millis: %d, startTime: %d, sy %d, sCx %d, sCy %d, ey %d, eCx %d, eCy %d\n", target, moveMillis(), curves[i]->getStartTimeMs(), curves[i]->startY, curves[i]->startControlX, curves[i]->startControlY, curves[i]->endY, curves[i]->endControlX, curves[i]->endControlY);
          // }
        }
        if (fileEnded && curves[i]->getEndTimeMs() < moveMillis())
        {

          finishedCurves++;
        }
      }
      else
      {
        // limb not active
        finishedCurves++;
      }
    }

    if (finishedCurves == 4)
    {
      moveFinished();
    }
  }
  
}

void MovePlayer::moveFinished()
{
  if (repeatMove)
  {
    debugD("Move finished, repeating");
    startMove(currentFileDir, moveToBeginOnly, repeatMove, kp, ki, startMoveSpeed);
  }
  else
  {
    debugD("Move finished, stopping");
    stop();
  }
}

void MovePlayer::stop()
{
  state = IDLE;

  resetAllCurves();

  fileEnded = true;
  nextCurveIndex = NO_INDEX;
}

void MovePlayer::startMove(const char *csvDir, bool beginPosOnly, bool repeat, float moveKp, float moveKi, float _startMoveSpeed)
{
  // disable previous curves
  resetAllCurves();

  nextCurveIndex = NO_INDEX;
  repeatMove = repeat;

  startMoveSpeed = _startMoveSpeed;

  if (!loadFile(csvDir))
  {
    return;
  }
  moveToBeginOnly = beginPosOnly;
  kp = moveKp;
  ki = moveKi;
  state = MOVE_TO_BEGIN;

  parseStartCSV();
  for (int i = 0; i < 4; i++)
  {
    lastStartMovementTime[i] = millis();
  }
}

bool MovePlayer::loadFile(const char *csvDir)
{
  fileEnded = false;
  strcpy(currentFileDir, csvDir);
  debugD("Selected file: %s", currentFileDir);

  file = SD.open(currentFileDir, FILE_READ);
  if (!file)
  {
    debugE("File (%s) open failed", currentFileDir);
    stop();
    return false;
  }
  else
  {
    debugI("File (%s) opened", currentFileDir);
    return true;
  }
}

void MovePlayer::parseStartCSV()
{

  // to bypass header & name row
  file.readStringUntil('\n');
  file.readStringUntil('\n');

  // debugD("line: %s", line.c_str());

  if (cp.parseRow())
  {

    for (int i = 0; i < 4; i++)
    {
      float *startPosition = (float *)cp[i + 2];
      startPositions[i] = startPosition[0];
    }

    debugI("Start positions: ArmL %f, ArmR %f, legL %f, legR %f", startPositions[0], startPositions[1], startPositions[2], startPositions[3]);
  }
  else
  {
    debugE("Error parsing start CSV");
  }
}

bool MovePlayer::limbActive(int limbIndex)
{
  // inactive limbs have a start position of 999.0
  return abs(startPositions[limbIndex] - 999.0) > 0.01;
}

void MovePlayer::moveTowardsStart(int limbIndex)
{
  float target = startPositions[limbIndex];
  float current = limbs[limbIndex]->getTarget();
  float error = target - current;

  // move towards target at speed startMoveSpeed
  int32_t deltaTime = millis() - lastStartMovementTime[limbIndex];
  lastStartMovementTime[limbIndex] = millis();

  // Serial.printf("I: %d, target: %f, current: %f, error: %f, dt: %d\n", limbIndex, target, current, error, deltaTime);

  float moveAngle = startMoveSpeed * (deltaTime / 1000.0);
  float moveAngleSign = error > 0 ? 1.0 : -1.0;
  float moveAngleClamped = min(moveAngle, abs(error)) * moveAngleSign;

  limbs[limbIndex]->setTarget(current + moveAngleClamped, kp, ki);
}

void MovePlayer::startCurves()
{
  if (moveToBeginOnly)
  {
    return;
  }
  state = MOVE_CURVES;
  moveStartTime = millis();
  nextCurve[0] = 0;

  // debugging
  // static int moveCount = 0;
  // moveCount++;
  // Serial.printf("Move count: %d\n", moveCount);
  // Serial.printf("Free heap: %d, minimum heap: %d \n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
  // end debugging
  debugI("Starting curves");
}

void MovePlayer::readCurve()
{
  // create new curve
  if (nextCurveIndex != NO_INDEX)
  {
    resetCurve(nextCurveIndex);
    curves[nextCurveIndex] = new FloatBezierCurve(nextCurve[0], nextCurve[1], nextCurve[2], nextCurve[3], nextCurve[4], nextCurve[5], nextCurve[6], nextCurve[7]);
  }


  if (cp.parseRow())
  {

    char **commandType = (char **)cp[0];
    char **motorID = (char **)cp[1];

    // only read the next curve if it is a curve
    if (strcmp(commandType[0], "sC") == 0)
    {
      if (strcmp(motorID[0], "m_arm_l") == 0)
      {
        nextCurveIndex = 0;
      }
      else if (strcmp(motorID[0], "m_arm_r") == 0)
      {
        nextCurveIndex = 1;
      }
      else if (strcmp(motorID[0], "m_leg_l") == 0)
      {
        nextCurveIndex = 2;
      }
      else if (strcmp(motorID[0], "m_leg_r") == 0)
      {
        nextCurveIndex = 3;
      }
      else
      {
        debugE("Invalid motorID");
      }

      for (int i = 0; i < 8; i++)
      {
        float *curveValue = (float *)cp[i + 2];
        nextCurve[i] = curveValue[0];
      }

      // Serial.printf("Next curve: %f, %f, %f, %f, %f, %f, %f, %f\n", nextCurve[0], nextCurve[1], nextCurve[2], nextCurve[3], nextCurve[4], nextCurve[5], nextCurve[6], nextCurve[7]);

      // if this new curve is also ready to start
      if (needToReadCurve())
      {
        readCurve();
      }
    }
    else
    {
      debugE("Not an sC cfommand");
    }
  }

  else
  {
    fileEnded = true;
    file.close();
    debugI("End of file reached");
  }
}

uint32_t MovePlayer::moveMillis()
{
  return updateMillis - moveStartTime;
}

bool MovePlayer::needToReadCurve()
{
  return nextCurve[0] < moveMillis() && !fileEnded;
}

void MovePlayer::resetCurve(uint8_t index)
{
  if (index < NUM_CURVES){
    if (curves[index] != nullptr)
  {
    delete curves[index];
    curves[index] = nullptr;
  }
  } else {
    debugE("Invalid index called on resetCurve");
  }
  
}

void MovePlayer::resetAllCurves(){
  for (int i = 0; i < NUM_CURVES; i++)
  {
    resetCurve(i);
  }
}