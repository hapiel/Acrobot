#ifndef CHOREOPLAYER_H
#define CHOREOPLAYER_H

#include "Arduino.h"
#include "Leg.h"

/*
to add new choreography:
1. add enum to ChoreoName below this comment
2. add the choreo in a Choreographystep in this file, between the clang off & on. (don't forget to set correct array size)
3. add the choreo to the switch in startChoreo
*/

enum ChoreoName
{
  NONE,
  STANDING,
  THREE_STEP,
  WALK_CONT,
  WALK_CONT_FORCE
};

class ChoreoPlayer
{
public:
  ChoreoPlayer(Leg &legL, Leg &legR);

  void init();
  void update();
  void startChoreo(ChoreoName choreoName);
  void stopChoreo();

private:
  Leg &legL;
  Leg &legR;

  struct ChoreographyStep
  {
    uint32_t time;
    std::function<void()> move;
  };

  uint16_t choreoStepCounter = 0;
  uint32_t choreoTimer;
  ChoreographyStep *currentChoreo;

  ChoreoName currentChoreoName = NONE;

  void launchChoreo(ChoreographyStep choreo[]);
  bool choreoTimePassed(uint32_t time);
  bool stepTimeBetween(uint32_t timeStart, uint32_t timeEnd);

  float pL = 0;
  float pR = 0;
  float dL = 5;
  float dR = 5;

  float balanceAngle = 187;

  // poses
  void pBow(int16_t upperBodyDegrees = 45);
  void pStand();
  void pStepRight(int8_t degrees = 20);
  void pStepLeft(int8_t degrees = 20);
  void pKickRight(int8_t degrees = 90);
  void pKickLeft(int8_t degrees = 90);
  void pSpecial(int16_t degreesL, int16_t degreesR);

  // moves
  void moveStand();
  void moveWalk();
  void moveWalkCont(int stepSize = 25, float p = 8);

  // clang-format off

  ChoreographyStep standingChoreo[2] = {
    {0, [this](){ moveStand(); }},
    {UINT32_MAX, nullptr} // Sentinel value indicating the end of the array
  };

  ChoreographyStep threeStepChoreo[4] = {

    {0, [this](){ moveWalk(); }},
    {2000, [this](){ moveWalk(); }},
    {4000, [this](){ moveWalk(); }},
    {UINT32_MAX, nullptr} // Sentinel value indicating the end of the array
  };

  ChoreographyStep walkContChoreo[2] = {

    {0, [this](){ moveWalkCont(); }},
    {UINT32_MAX, nullptr} // Sentinel value indicating the end of the array
  };

  ChoreographyStep walkContForceChoreo[2] = {

    {0, [this](){ moveWalkCont(20, 30); }},
    {UINT32_MAX, nullptr} // Sentinel value indicating the end of the array
  };

  // clang-format on
};

#endif // CHOREOPLAYER_H
