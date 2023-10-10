#ifndef CHOREOPLAYER_H
#define CHOREOPLAYER_H

#include "Arduino.h"
#include "Leg.h"
#include "utilsAcrobot.h"
#include "RemoteDebug.h"

/*
to add new choreography:
1. add enum to ChoreoName below this comment
2. add the choreo in a Choreographystep in this file, between the clang off & on. (don't forget to set correct array size)
3. add the choreo to the switch in start
*/

enum ChoreoName
{
  NONE,
  STANDING,
  THREE_STEP,
  WALK_CONT,
  WALK_CONT_FORCE,
  SPLIT_TWICE,
  TEST_CHOREO,
  SLOW_CHOREO
};

class ChoreoPlayer
{
public:
  ChoreoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR);

  void update();
  void start(ChoreoName choreoName);
  void stop();

private:
  RemoteDebug &Debug;
  Leg &legL;
  Leg &legR;
  float posLStepStart;
  float posRStepStart;

  struct ChoreographyStep
  {
    uint32_t time;
    std::function<void()> move;
  };

  uint16_t choreoStepCounter = 0;
  uint32_t choreoStartTime;

  ChoreographyStep *currentChoreo;

  ChoreoName currentChoreoName = NONE;

  void launchChoreo(ChoreographyStep choreo[]);
  bool choreoTimePassed(uint32_t time);
  bool stepTimeBetween(uint32_t timeStart, uint32_t timeEnd);

  uint32_t stepTimePassed();
  float moveLinear(float startPos, float endPos, int duration, uint32_t startTime = 0);

  float pL = 0;
  float pR = 0;
  float dL = 5;
  float dR = 5;
  void setPPDDLR(float pL, float pR, float dL = 5, float dR = 5);
  void setPD(float p, float d = 5);

  float balanceAngle = 187;

  // poses
  void pBowDirect(int16_t upperBodyDegrees = 135);

  void pStandDirect();
  void pStepRightDirect(int8_t degrees = 20);
  void pStepLeftDirect(int8_t degrees = 20);
  void pKickRightDirect(int8_t degrees = 90);
  void pKickLeftDirect(int8_t degrees = 90);
  void pLRDirect(int16_t degreesL, int16_t degreesR);

  void pBow(int16_t upperBodyDegrees = 135, int16_t duration = 1000);

  void pStepRight(int8_t degreesFromCenter = 20, int16_t duration = 600);
  void pStepLeft(int8_t degreesFromCenter = 20, int16_t duration = 600);
  void pKickRight(int8_t degrees = 90, int16_t duration = 600);
  void pKickLeft(int8_t degrees = 90, int16_t duration = 600);
  void pStand(int16_t duration = 800);
  void pLR(int16_t degreesL, int16_t degreesR, int16_t duration = 800);

  // moves
  // OUTDATED, needs to be replaced with a multi levels of choreo (or nesting) system.
  void moveStand();
  void moveWalk();
  void moveWalkCont(int stepSize = 25, float p = 8);
  void moveSplitTwice();
  void moveBowThrice();

  // clang-format off

  ChoreographyStep testChoreo[40] = {
    {0, [this](){ setPD(6); pStand(300); }},
    {2000, [this](){ setPD(6); pBow(); }},
    {3000, [this](){ setPD(6); pStand(); }},
    {5000, [this](){ setPD(20); pBow(); }},
    {6000, [this](){ setPD(20); pStand(); }},
    {8000, [this](){ setPD(20); pBow(135, 200); }},
    {9000, [this](){ setPD(20); pStand(200); }},
    {11000, [this](){ setPD(15); pStepRight(); }},
    {11800, [this](){ setPD(12); pStepLeft(); }},
    {12600, [this](){ setPD(10); pStepRight(); }},
    {13400, [this](){ setPD(8); pStepLeft(); }},
    {14200, [this](){ setPD(6); pStepRight(); }},
    {15000, [this](){ setPD(4); pStepLeft(); }},
    {15800, [this](){ setPD(3); pStepRight(); }},
    {16400, [this](){ setPD(2); pStepLeft(); }},
    {17200, [this](){ setPD(1); pStepRight(); }},
    {18000, [this](){ setPD(12); pKickRight(); }},
    {20000, [this](){ setPD(50); pKickLeft(); }},
    {22000, [this](){ setPD(50); pStepRight(80, 700); }},
    {24000, [this](){ setPD(50); pStepLeft(80, 500); }},
    {26000, [this](){ setPD(50); pStepRight(80, 300); }},
    {27000, [this](){ setPD(50); pStepLeft(80, 100); }},
    {30000, [this](){ setPD(80); pStand(5000); }},
    {UINT32_MAX, nullptr} // Sentinel value indicating the end of the array
  };

  ChoreographyStep slowChoreo[10] = {
    {0, [this](){ setPD(6); pStand(300); }},
    {2000, [this](){ setPD(30); pKickRight(90, 10000); }},
    {14000, [this](){ setPD(6); pStand(800); }},
    {17000, [this](){ setPD(20); pKickRight(90, 20000); }},
    {39000, [this](){ setPD(6); pStand(800); }},
    {42000, [this](){ setPD(80); pKickRight(90, 20000); }},
    {65000, [this](){ setPD(6); pStand(800); }},
    {UINT32_MAX, nullptr} // Sentinel value indicating the end of the array
  };

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

  ChoreographyStep splitTwiceChoreo[2] = {
    {0, [this](){ moveSplitTwice(); }},
    {UINT32_MAX, nullptr} 
  };

  // ChoreographyStep fourBowsChoreo[4] = {
  //   {0, [this](){ setPPDDLR(10,10); pBow(135, 1000); }},
  //   {1000, [this](){ pKickRightDirect(90); }},
  //   {2000, [this](){ moveBowThrice(); }},
  //   {UINT32_MAX, nullptr} 
  // };

  // clang-format on
};

#endif // CHOREOPLAYER_H
