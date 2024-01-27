#include "movePlayer.h"

MovePlayer::MovePlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp, CSV_Parser &cpStart)
    : Debug(Debug), file(file), cp(cp), cpStart(cpStart)
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

    for (int i = 0; i < 4; i++)
    {
      if (curves[i] != nullptr && limbActive(i))
      {
        float target = curves[i]->getValue(moveMillis());
        limbs[i]->setTarget(target, kp, ki);
      }
    }


    // Implement the logic to move the curves
  }
}

void MovePlayer::stop()
{
  state = IDLE;
  
  curves[0] = nullptr;
  curves[1] = nullptr;
  curves[2] = nullptr;
  curves[3] = nullptr;
}

void MovePlayer::startMove(const char *csvDir, bool beginPosOnly, float moveKp, float moveKi)
{
  curves[0] = nullptr;
  curves[1] = nullptr;
  curves[2] = nullptr;
  curves[3] = nullptr;

  if (!loadFile(csvDir))
  {
    return;
  }
  moveToBeginOnly = beginPosOnly;
  kp = moveKp;
  ki = moveKi;
  state = MOVE_TO_BEGIN;

  parseStartCSV();
  lastStartMovementTime = millis();
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

  // to bypass header
  String line = file.readStringUntil('\n');

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

  Serial.printf("target: %f, current: %f, error: %f\n", target, current, error);

  // move towards target at speed startMoveSpeed
  int32_t deltaTime = millis() - lastStartMovementTime;
  float moveAngle = startMoveSpeed * (deltaTime / 1000.0);
  float moveAngleSign = error > 0 ? 1.0 : -1.0;
  float moveAngleClamped = min(moveAngle, abs(error)) * moveAngleSign;

  limbs[limbIndex]->setTarget(current + moveAngleClamped, kp, ki);
}

void MovePlayer::startCurves()
{
  state = MOVE_CURVES;
  moveStartTime = millis();
  nextCurve[0] = 0;
  debugI("Starting curves");
}

void MovePlayer::readCurve()
{
  // create new curve
  if (nextCurveIndex != NO_INDEX){
    curves[nextCurveIndex] = new FloatBezierCurve(nextCurve[0], nextCurve[1], nextCurve[2], nextCurve[3], nextCurve[4], nextCurve[5], nextCurve[6], nextCurve[7]);
  }
  


  // copy next curve to current curve
  // std::copy(std::begin(nextCurve), std::end(nextCurve), std::begin(currentCurves[nextCurveIndex]));

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
      debugE("Not an sC command");
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