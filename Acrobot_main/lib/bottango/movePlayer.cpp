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
        }
        else
        {
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
    // Implement the logic to move the curves
  }
}

void MovePlayer::stop()
{
  state = IDLE;
}

void MovePlayer::startMove(const char *csvDir, bool beginPosOnly, float moveKp, float moveKi)
{

  if (!loadFile(csvDir))
  {
    return;
  }
  moveToBeginOnly = beginPosOnly;
  kp = moveKp;
  ki = moveKi;
  state = MOVE_TO_BEGIN;

  parseStartCSV();
  lastMoveTime = millis();
}

bool MovePlayer::loadFile(const char *csvDir)
{
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

  if (cpStart.parseRow())
  {
    debugD("Parsing start CSV");
    for (int i = 0; i < 4; i++)
    {
      float *startPosition = (float *)cpStart[i];
      startPositions[i] = *startPosition;
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
  int32_t deltaTime = millis() - lastMoveTime;
  float moveAngle = startMoveSpeed * (deltaTime / 1000.0);
  float moveAngleSign = error > 0 ? 1.0 : -1.0;
  float moveAngleClamped = min(moveAngle, abs(error)) * moveAngleSign;

  limbs[limbIndex]->setTarget(current + moveAngleClamped, kp, ki);
}

void MovePlayer::startCurves(){
  state = MOVE_CURVES;
  debugI("Starting curves");
}