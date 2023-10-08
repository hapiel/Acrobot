#include "ChoreoPlayer.h"

ChoreoPlayer::ChoreoPlayer(Leg &legL, Leg &legR) : legL(legL), legR(legR)
{
  // Constructor implementation, if any
}

void ChoreoPlayer::init()
{
  // Initialization logic
}

void ChoreoPlayer::update()
{
  if(currentChoreoName == NONE){
    return;
  }

  if (choreoTimePassed(currentChoreo[choreoStepCounter].time)){
    if (currentChoreo[choreoStepCounter].time != UINT32_MAX){ // stops at last step
      choreoStepCounter ++;
    }
  }
  currentChoreo[choreoStepCounter - 1].move();
}

void ChoreoPlayer::startChoreo(ChoreoName choreoName)
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
  default:
    break;
  }
}

void ChoreoPlayer::launchChoreo(ChoreographyStep choreo[])
{
  currentChoreo = choreo;
  choreoTimer = millis();
  choreoStepCounter = 0;
}

void ChoreoPlayer::stopChoreo()
{
  currentChoreoName = NONE;
}

bool ChoreoPlayer::choreoTimePassed(uint32_t time)
{
  return millis() - choreoTimer > time;
}

bool  ChoreoPlayer::stepTimeBetween(uint32_t timeStart, uint32_t timeEnd)
{
  uint32_t choreoTime = millis() - choreoTimer;
  uint32_t stepStartTime = currentChoreo[choreoStepCounter - 1].time + timeStart;
  uint32_t stepEndTime = currentChoreo[choreoStepCounter - 1].time + timeEnd;

  // are we between start and end time?
  return choreoTime >= stepStartTime && choreoTime < stepEndTime;
}

// POSES

void ChoreoPlayer::pBow(int16_t upperBodyDegrees)
{
  legL.setTarget(upperBodyDegrees, pL, pR);
  legR.setTarget(upperBodyDegrees, pL, pR);
}

void ChoreoPlayer::pStand()
{
  pBow(balanceAngle);
}

void ChoreoPlayer::pStepRight(int8_t degrees)
{
  legL.setTarget(balanceAngle + degrees, pL, pR);
  legR.setTarget(balanceAngle - degrees, pL, pR);
}

void ChoreoPlayer::pStepLeft(int8_t degrees)
{
  legL.setTarget(balanceAngle - degrees, pL, pR);
  legR.setTarget(balanceAngle + degrees, pL, pR);
}

void ChoreoPlayer::pKickRight(int8_t degrees)
{
  legL.setTarget(balanceAngle, pL, pR);
  legR.setTarget(balanceAngle - degrees, pL, pR);
}

void ChoreoPlayer::pKickLeft(int8_t degrees)
{
  legL.setTarget(balanceAngle - degrees, pL, pR);
  legR.setTarget(balanceAngle, pL, pR);
}

void ChoreoPlayer::pSpecial(int16_t degreesL, int16_t degreesR)
{
  legL.setTarget(degreesL, pL, pR);
  legR.setTarget(degreesR, pL, pR);
}

// - MOVES

void ChoreoPlayer::moveStand()
{
  pL = 15;
  pR = 15;
  dL = 5;
  dR = 5;
  pStand();
}

void ChoreoPlayer::moveWalk()
{
  pL = 8;
  pR = 8;
  dL = 5;
  dR = 5;
  if (stepTimeBetween(0, 800)){
    pStepRight(20);
  }

  if (stepTimeBetween(800, 1600))
  {
    pStepLeft(20);
  }
}

void ChoreoPlayer::moveWalkCont(int stepSize , float p)
{
  pL = p;
  pR = p;
  dL = 5;
  dR = 5;
  int timeStep = 1600;

  for(int i = 0; i < 100; i++){
    if (stepTimeBetween(timeStep * i, timeStep * i + timeStep/2)){ // 0, 800
      pStepRight(stepSize);
    }
    if (stepTimeBetween(timeStep * i + timeStep/2, timeStep * i + timeStep)){ // 800, 1600
      pStepLeft(stepSize);
    }
  }
}