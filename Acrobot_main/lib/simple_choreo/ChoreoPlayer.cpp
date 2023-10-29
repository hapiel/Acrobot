#include "ChoreoPlayer.h"

ChoreoPlayer::ChoreoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR) : Debug(Debug), legL(legL), legR(legR), armL(armL), armR(armR)
{
}

void ChoreoPlayer::update()
{
  if (currentChoreoName == CHOREO_NONE)
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
  currentChoreo[max(0, choreoStepCounter - 1)].move(); // catch weird exception with max();
}

void ChoreoPlayer::start(ChoreoName choreoName)
{
  currentChoreoName = choreoName;

  switch (choreoName)
  {
  case CHOREO_NONE:
    break;
  case CHOREO_STANDING:
    launchChoreo(standingChoreo);
    break;
  case CHOREO_THREE_STEP:
    launchChoreo(threeStepChoreo);
    break;
  case CHOREO_WALK_CONT:
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
  case ACT_MILA:
    launchChoreo(actMila);
    break;
  case TEXT_SEQUENCE_0:
    launchChoreo(textSequence0);
    break;
  case TEXT_SEQUENCE_1:
    launchChoreo(textSequence1);
    break;
  case MUSIC_SEQUENCE_0:
    launchChoreo(musicSequence0);
    break;
  case MUSIC_SEQUENCE_1:
    launchChoreo(musicSequence1);
    break;
  case MUSIC_SEQUENCE_2:
    launchChoreo(musicSequence2);
    break;
  case MUSIC_SEQUENCE_3:
    launchChoreo(musicSequence3);
    break;
  case MUSIC_SEQUENCE_4:
    launchChoreo(musicSequence4);
    break;
  case MUSIC_SEQUENCE_5:
    launchChoreo(musicSequence5);
    break;
  case MUSIC_SEQUENCE_6:
    launchChoreo(musicSequence6);
    break;
  case MUSIC_SEQUENCE_7:
    launchChoreo(musicSequence7);
    break;
  case MUSIC_SEQUENCE_8:
    launchChoreo(musicSequence8);
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
  currentChoreoName = CHOREO_NONE;
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

void ChoreoPlayer::pStepRightDirect(int16_t degrees)
{
  legL.setTarget(balanceAngle + degrees, pL, pR);
  legR.setTarget(balanceAngle - degrees, pL, pR);
}

void ChoreoPlayer::pStepLeftDirect(int16_t degrees)
{
  legL.setTarget(balanceAngle - degrees, pL, pR);
  legR.setTarget(balanceAngle + degrees, pL, pR);
}

void ChoreoPlayer::pKickRightDirect(int16_t degrees)
{
  legL.setTarget(balanceAngle, pL, pR);
  legR.setTarget(balanceAngle - degrees, pL, pR);
}

void ChoreoPlayer::pKickLeftDirect(int16_t degrees)
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

void ChoreoPlayer::pStepRight(int16_t degreesFromCenter, int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle + degreesFromCenter, duration);
  float posR = moveLinear(posRStepStart, balanceAngle - degreesFromCenter, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pStepLeft(int16_t degreesFromCenter, int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle - degreesFromCenter, duration);
  float posR = moveLinear(posRStepStart, balanceAngle + degreesFromCenter, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pKickRight(int16_t degrees, int16_t duration)
{
  float posL = moveLinear(posLStepStart, balanceAngle, duration);
  float posR = moveLinear(posRStepStart, degrees, duration);
  pLRDirect(posL, posR);
}

void ChoreoPlayer::pKickLeft(int16_t degrees, int16_t duration)
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
  pL = 12;
  pR = 12;
  iL = 3;
  iR = 3;
  pStand(1000);
}

void ChoreoPlayer::moveWalk()
{
  pL = 8;
  pR = 8;
  iL = 5;
  iR = 5;
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
  iL = 5;
  iR = 5;
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
  iL = 5;
  iR = 5;

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

void ChoreoPlayer::setPPDDLR(float pL, float pR, float iL, float iR)
{
  this->pL = pL;
  this->pR = pR;
  this->iL = iL;
  this->iR = iR;
}

void ChoreoPlayer::setPD(float p, float d)
{
  setPPDDLR(p, p, d, d);
}