#include "ChoreoPlayer.h"

ChoreoPlayer::ChoreoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR) : Debug(Debug), legL(legL), legR(legR)
{
}

void ChoreoPlayer::update()
{
  if (currentChoreoName == NONE)
  {
    return;
  }

  if (choreoTimePassed(currentChoreo[choreoStepCounter].time))
  {
    if (currentChoreo[choreoStepCounter].time != UINT32_MAX)
    { // stops at last step
      choreoStepCounter++;
      posLStepStart = legL.getPosition();
      posRStepStart = legR.getPosition();
      debugD("posL: %f, posR: %f", posLStepStart, posRStepStart);
    }
  }
  currentChoreo[choreoStepCounter - 1].move();
}

void ChoreoPlayer::start(ChoreoName choreoName)
{
  currentChoreoName = choreoName;

  switch (choreoName)
  {
  case NONE:
    break;
  case STANDING:
    launchChoreo(standingChoreo);
    break;
  case THREE_STEP:
    launchChoreo(threeStepChoreo);
    break;
  case WALK_CONT:
    launchChoreo(walkContChoreo);
    break;
  case WALK_CONT_FORCE:
    launchChoreo(walkContForceChoreo);
    break;
  case SPLIT_TWICE:
    launchChoreo(splitTwiceChoreo);
    break;
  case TEST_CHOREO:
    launchChoreo(testChoreo);
    break;
  case SLOW_CHOREO:
    launchChoreo(slowChoreo);
    break;
  default:
    break;
  }
}

void ChoreoPlayer::launchChoreo(ChoreographyStep choreo[])
{
  currentChoreo = choreo;
  choreoStartTime = millis();
  choreoStepCounter = 0;
}

void ChoreoPlayer::stop()
{
  currentChoreoName = NONE;
}

bool ChoreoPlayer::choreoTimePassed(uint32_t time)
{
  return millis() - choreoStartTime > time;
}

bool ChoreoPlayer::stepTimeBetween(uint32_t timeStart, uint32_t timeEnd)
{
  uint32_t choreoTime = millis() - choreoStartTime;
  uint32_t stepStartTime = currentChoreo[choreoStepCounter - 1].time + timeStart;
  uint32_t stepEndTime = currentChoreo[choreoStepCounter - 1].time + timeEnd;

  // are we between start and end time?
  return choreoTime >= stepStartTime && choreoTime < stepEndTime;
}

uint32_t ChoreoPlayer::stepTimePassed()
{
  return millis() - choreoStartTime - currentChoreo[choreoStepCounter - 1].time;
}

// POSES

void ChoreoPlayer::pBowDirect(int16_t upperBodyDegrees)
{
  legL.setTarget(upperBodyDegrees, pL, pR);
  legR.setTarget(upperBodyDegrees, pL, pR);
}

void ChoreoPlayer::pStandDirect()
{
  pBowDirect(balanceAngle);
}

void ChoreoPlayer::pStepRightDirect(int8_t degrees)
{
  legL.setTarget(balanceAngle + degrees, pL, pR);
  legR.setTarget(balanceAngle - degrees, pL, pR);
}

void ChoreoPlayer::pStepLeftDirect(int8_t degrees)
{
  legL.setTarget(balanceAngle - degrees, pL, pR);
  legR.setTarget(balanceAngle + degrees, pL, pR);
}

void ChoreoPlayer::pKickRightDirect(int8_t degrees)
{
  legL.setTarget(balanceAngle, pL, pR);
  legR.setTarget(balanceAngle - degrees, pL, pR);
}

void ChoreoPlayer::pKickLeftDirect(int8_t degrees)
{
  legL.setTarget(balanceAngle - degrees, pL, pR);
  legR.setTarget(balanceAngle, pL, pR);
}

void ChoreoPlayer::pLRDirect(int16_t degreesL, int16_t degreesR)
{
  legL.setTarget(degreesL, pL, pR);
  legR.setTarget(degreesR, pL, pR);
}

void ChoreoPlayer::pBow(int16_t upperBodyDegrees, int16_t duration)
{

  float posL = moveLinear(posLStepStart, upperBodyDegrees, duration);
  float posR = moveLinear(posRStepStart, upperBodyDegrees, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pStepRight(int8_t degreesFromCenter, int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle + degreesFromCenter, duration);
  float posR = moveLinear(posRStepStart, balanceAngle - degreesFromCenter, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pStepLeft(int8_t degreesFromCenter, int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle - degreesFromCenter, duration);
  float posR = moveLinear(posRStepStart, balanceAngle + degreesFromCenter, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pKickRight(int8_t degrees, int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle, duration);
  float posR = moveLinear(posRStepStart, degrees, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pKickLeft(int8_t degrees, int16_t duration)
{
  float posL = moveLinear(posLStepStart, degrees, duration);
  float posR = moveLinear(posRStepStart, balanceAngle, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pStand(int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle, duration);
  float posR = moveLinear(posRStepStart, balanceAngle, duration);
  pLRDirect(posL, posR);

}

void ChoreoPlayer::pLR(int16_t degreesL, int16_t degreesR, int16_t duration)
{
  float posL = moveLinear(posLStepStart, degreesL, duration);
  float posR = moveLinear(posRStepStart, degreesR, duration);
  pLRDirect(posL, posR);
}

// - MOVES

void ChoreoPlayer::moveStand()
{
  pL = 15;
  pR = 15;
  dL = 5;
  dR = 5;
  pStandDirect();
}

void ChoreoPlayer::moveWalk()
{
  pL = 8;
  pR = 8;
  dL = 5;
  dR = 5;
  if (stepTimeBetween(0, 800))
  {
    pStepRightDirect(20);
  }

  if (stepTimeBetween(800, 1600))
  {
    pStepLeftDirect(20);
  }
}

void ChoreoPlayer::moveWalkCont(int stepSize, float p)
{
  pL = p;
  pR = p;
  dL = 5;
  dR = 5;
  int timeStep = 1600;

  for (int i = 0; i < 100; i++)
  {
    if (stepTimeBetween(timeStep * i, timeStep * i + timeStep / 2))
    { // 0, 800
      pStepRightDirect(stepSize);
    }
    if (stepTimeBetween(timeStep * i + timeStep / 2, timeStep * i + timeStep))
    { // 800, 1600
      pStepLeftDirect(stepSize);
    }
  }
}

void ChoreoPlayer::moveSplitTwice()
{
  int duration = 1000;
  pL = 10;
  pR = 10;
  dL = 5;
  dR = 5;

  if (stepTimeBetween(0, 2000))
  {

    float posR = moveLinear(posRStepStart, 90, duration);
    float posL = moveLinear(posLStepStart, 270, duration);

    pLRDirect(posL, posR);
  }

  if (stepTimeBetween(2000, 4000))
  {

    float posR = moveLinear(90, 270, duration, 2000);
    float posL = moveLinear(270, 90, duration, 2000);

    pLRDirect(posL, posR);
  }
}

// void ChoreoPlayer::moveBowThrice(){

//   // doesn't work correctly because it doesn't know the new starting positions after the first bow. May be fixed later, for now let's not use the move system.

//   if (stepTimeBetween(0, 2000))
//   {
//     pBow(225);
//   }

//   if (stepTimeBetween(2000, 4000))
//   {
//     pBow(135);
//   }

//   if (stepTimeBetween(4000, 6000)){
//     pBow(225);
//   }

// }

// - HELPERS

float ChoreoPlayer::moveLinear(float startPos, float endPos, int duration, uint32_t startTime)
{
  float position = fMap(stepTimePassed(), startTime, startTime + duration, startPos, endPos);
  position = constrain(position, min(startPos, endPos), max(startPos, endPos));
  return position;
}

void ChoreoPlayer::setPPDDLR(float pL, float pR, float dL, float dR)
{
  this->pL = pL;
  this->pR = pR;
  this->dL = dL;
  this->dR = dR;
}

void ChoreoPlayer::setPD(float p, float d)
{
  setPPDDLR(p, p, d, d);
}