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
  CHOREO_NONE,
  CHOREO_STANDING,
  CHOREO_THREE_STEP,
  CHOREO_WALK_CONT,
  WALK_CONT_FORCE,
  SPLIT_TWICE,
  TEST_CHOREO,
  SLOW_CHOREO,
  ACT_MILA,
  TEXT_SEQUENCE_0,
  TEXT_SEQUENCE_1,
  MUSIC_SEQUENCE_0,
  MUSIC_SEQUENCE_1,
  MUSIC_SEQUENCE_2,
  MUSIC_SEQUENCE_3,
  MUSIC_SEQUENCE_4,
  MUSIC_SEQUENCE_5,
  MUSIC_SEQUENCE_6,
  MUSIC_SEQUENCE_7,
  MUSIC_SEQUENCE_8
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

  ChoreoName currentChoreoName = CHOREO_NONE;

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

  float balanceAngle = 186;

  // poses
  void pBowDirect(int16_t upperBodyDegrees = 135);

  void pStandDirect();
  void pStepRightDirect(int16_t degrees = 20);
  void pStepLeftDirect(int16_t degrees = 20);
  void pKickRightDirect(int16_t degrees = 90);
  void pKickLeftDirect(int16_t degrees = 90);
  void pLRDirect(int16_t degreesL, int16_t degreesR);

  // linear move versions
  void pBow(int16_t upperBodyDegrees = 135, int16_t duration = 600);
  void pStepRight(int16_t degreesFromCenter = 20, int16_t duration = 600);
  void pStepLeft(int16_t degreesFromCenter = 20, int16_t duration = 600);
  void pKickRight(int16_t degrees = 90, int16_t duration = 600);
  void pKickLeft(int16_t degrees = 90, int16_t duration = 600);
  void pStand(int16_t duration = 600);
  void pLR(int16_t degreesL, int16_t degreesR, int16_t duration = 600);

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
    {0, [this](){ setPD(12, 2); pStand(1000); }},
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

  ChoreographyStep actMila[3] = {
    {0, [this](){ setPD(10); pBow(180); }},
    {1000, [this](){ launchChoreo(textSequence0); }}, 
    {UINT32_MAX, nullptr} 
  };

  // text sequence 0

  ChoreographyStep textSequence0[20] = {
    {0, [this](){ setPD(10); pBow(125, 500); }},
    {7600, [this](){ setPD(10); pBow(180, 500); }},
    {9250, [this](){ setPD(8); pKickLeft(90, 500); }},
    {19750, [this](){ setPD(10); pLR(90, 120, 200); }}, // swing excited
    {20550, [this](){ setPPDDLR(10, 6); pLR(90, 220, 300); }},
    {21190, [this](){ pLR(90, 120, 200); }},
    {29800, [this](){ setPD(8); pBow(90); }},
    {37680, [this](){ setPD(6); pKickRight(90); }}, // lower left
    {41760, [this](){ setPD(7); pLR(120, 90, 200); }}, // swing left
    {42350, [this](){ setPD(6); pLR(220, 90, 400); }},
    {43250, [this](){ setPD(6); pKickRight(90); }},
    {50830, [this](){ setPD(8); pLR(90, 235, 300); }}, // left back
    {56360, [this](){ setPD(4); pBow(180, 1000); }}, // left back
    {59920, [this](){ setPD(8); pBow(60); }}, // hello people bow
    {60000, [this](){ launchChoreo(textSequence1); }}, 
    {UINT32_MAX, nullptr} 
  };

  // text sequence 1
  
  ChoreographyStep textSequence1[50] = {
    {0, [this](){ setPD(8); pBow(60); }}, // hello people bow
    {1500, [this](){ setPD(8); pBow(90, 200); }}, // hello people bow
    {2860, [this](){ setPD(8); pBow(70, 600); }},
    {3305, [this](){ setPD(9); pBow(90, 300); }},
    {4500, [this](){ setPD(8); pBow(70, 800); }},
    {4900, [this](){ setPD(8); pBow(90, 200); }},
    {6150, [this](){ setPD(8); pBow(70, 800); }},
    {7070, [this](){ setPD(8); pBow(90, 250); }},
    {8700, [this](){ setPD(8); pBow(90, 200); }},
    {9880, [this](){ setPD(8); pBow(65, 600); }},
    {10600, [this](){ setPD(8); pBow(90, 300); }},
    {12560, [this](){ setPD(8); pBow(70, 600); }},
    {13640, [this](){ setPD(8); pBow(90, 300); }},
    {14680, [this](){ setPD(8); pBow(70, 600); }},
    {15800, [this](){ setPD(8); pLR(60, 90, 200); }} ,// wiggle
    {16130, [this](){ setPD(8); pLR(90, 60, 200); }},
    {16430, [this](){ setPD(8); pLR(60, 90, 200); }},
    {17680, [this](){ setPD(8); pLR(90, 60, 200); }},
    {17940, [this](){ setPD(8); pLR(60, 90, 200); }},
    {18230, [this](){ setPD(8); pLR(90, 60, 200); }},
    {19090, [this](){ setPD(8); pBow(90, 800); }},
    {21700, [this](){ setPD(4); pBow(165, 800); }},
    {56770, [this](){ setPD(8); pBow(135, 200); }} ,// but wait
    {58070, [this](){ setPD(10); pStand(800); }},
    {70000, [this](){ launchChoreo(musicSequence0); }},
    {UINT32_MAX, nullptr} 
  };

  ChoreographyStep musicSequence0[50] = {
    {0, [this](){ setPD(15); pBow(174, 1000); }},  // bow - kP = 1
    {2200, [this](){ setPD(15); pStand(1000); }},  // kP = 1
    {6300, [this](){ setPD(10); pKickRight(135, 200); }},  // raise leg, walk - kP = 1
    {8670, [this](){  setPD(12); pStepRight(16, 400); }},  // kP = 1.4
    {9570, [this](){  setPD(12); pStepLeft(15, 800); }},  // kP = 1.4
    {10560, [this](){ setPD(12); pStepRight(15, 800); }},  // kP = 1.4
    {11470, [this](){ setPD(12); pStand(800); }},  // kP = 1.6
    {14600, [this](){ setPD(12); pKickLeft(135, 200); }},  // raise leg, walk - kP = 1.2
    {16550, [this](){ setPD(12); pStepLeft(20, 400); }},  // kP = 1.4
    {17600, [this](){ setPD(12); pStepRight(20, 800); }},  // kP = 1.4
    {18650, [this](){ setPD(12); pStepLeft(20, 800); }},  // kP = 1.4
    {19600, [this](){ setPD(12); pStand(600); }},  // kP = 1.6
    {20700, [this](){ setPD(12); pBow(180); }},  // breathe - kP = 0.8
    {22800, [this](){ setPD(12); pStand(); }},  // kP = 1
    {24800, [this](){ setPD(12); pStepRight(20); }},  // walk, pirouette - kP = 1.4
    {25900, [this](){ setPD(12); pStepLeft(20); }},  // kP = 1.4
    {26900, [this](){ setPD(12); pStepRight(20); }},  // kP = 1.4
    {27900, [this](){ setPD(12); pStepLeft(20); }},  // kP = 1
    {28900, [this](){ setPD(12); pStand(); }},  // pirouette - kP = 1.8
    {29900, [this](){ setPD(12); pLR(170, 200, 200);}},  // kP = 3
    {30900, [this](){ setPD(30); pKickRight(90, 300); }},  // kP = 2
    {31350, [this](){ setPD(20); pBow(180, 100); }},  // kP = 2
    {31700, [this](){ setPD(12); pStand(); }},  // kP = 1.8
    {32870, [this](){ setPD(12); pBow(180); }},  // breathe - kP = 0.8
    {34950, [this](){ setPD(12); pStand(); }},  // kP = 1
    {37150, [this](){ setPD(12); pBow(100, 600); }},  // jump - kP = 0.6
    {38280, [this](){ setPD(80); pBow(190, 20); }},  // kP = 4
    {39000, [this](){ setPD(12); pBow(170, 300); }},  // kP = 2
    {40200, [this](){ setPD(12); pStand(); }},  // kP = 0.8
    {43550, [this](){ setPD(12); pStepRight(20); }},  // step step flip - kP = 1.4
    {44600, [this](){ setPD(12); pStepLeft(10); }},  // kP = 1.6
    {45900, [this](){ setPD(15); pLR(192, 90, 200 ); }},  // kP = 3
    {46100, [this](){ setPD(20); pStepRight(90, 100); }},  // kP = 2
    {46900, [this](){ setPD(18); pBow(120, 100); }},  // kP = 1.5
    {47840, [this](){ setPD(12); pStand(); }},  // kP = 0.8
    {54090, [this](){ setPD(12); pStepRight(20); }},  // again step step flip - kP = 1.4
    {55120, [this](){ setPD(12); pStepLeft(10); }},  // kP = 1.6
    {56500, [this](){ setPD(15); pLR(192, 90, 200 );}},  // kP = 3
    {56700, [this](){ setPD(20); pStepRight(120, 100); }},  // kP = 2
    {57500, [this](){ setPD(18); pBow(120), 100; }},  // kP = 1.5
    {58500, [this](){ setPD(12); pStand(); }},  // kP = 0.8
    {60000, [this](){ launchChoreo(musicSequence1); }},  // kP = Original Value
    {UINT32_MAX, nullptr}  // End of sequence marker
  };



  ChoreographyStep musicSequence1[50] = {
    {0, [this](){ setPD(12); pStand(400); }},  // kP = 0.8

    // bow
    {1600, [this](){ setPD(8); pBow(135, 1000); }},  // kP = 0.5
    {3160, [this](){ setPD(10); pStand(1000); }},  // kP = 0.8

    // snoek
    {8200, [this](){ setPD(12); pBow(155, 200); }},  // kP = 1.8
    {9200, [this](){ setPD(12); pBow(165, 400); }},  // kP = 1.2
    {10000, [this](){ setPD(20); pBow(180, 1200); }},  // kP = 2

    // swimming
    {12100, [this](){ setPD(20); pLR(190, 170, 800); }},  // kP = 2
    {14000, [this](){ setPD(20); pLR(170, 190, 800); }},  // kP = 2
    {15985, [this](){ setPD(20); pLR(190, 170, 800); }},  // kP = 2
    {17890, [this](){ setPD(20); pLR(170, 190, 800);}},  // kP = 2
    {19750, [this](){ setPD(20); pLR(190, 170, 800); }},  // kP = 2
    {21665, [this](){ setPD(20); pLR(170, 190, 800); }},  // kP = 2
    {22650, [this](){ setPD(20); pBow(180, 800); }},  // kP = 0.5

    // cloth hanger
    {25240, [this](){ setPD(4); pBow(90); }},  // kP = 0.3
    {28800, [this](){ setPD(12); pBow(105); }},  // kP = 2

    // kick naar bolkje
    {29860, [this](){ setPD(12); pKickRight(90); }},  // kP = 1.2
    {33270, [this](){ setPD(7); pStepRight(75, 1000); }},  // kP = 0.7
    {35460, [this](){ setPD(10); pStepRight(60); }},  // kP = 1
    {36000, [this](){ setPD(10); pStepRight(50); }},  // kP = 1.2
    {36500, [this](){ setPD(10); pStepRight(35); }},  // kP = 1
    {37000, [this](){ setPD(18); pStepRight(5); }},  // kP = 1.8

    // swim in bolk
    {38500, [this](){ setPD(10); pStepLeft(10); }},  // kP = 1
    {40000, [this](){ setPD(10); pStepRight(10); }},  // kP = 1
    {40800, [this](){ setPD(12); pStepLeft(10); }},  // kP = 1.2
    {41600, [this](){ setPD(12); pStepRight(10); }},  // kP = 1.2
    {42400, [this](){ setPD(12); pStepLeft(13); }},
    // to knees
    {43600, [this](){ setPD(16); pStepRight(90, 2500); }},  // kP = 1
    {48590, [this](){ setPD(12); pKickRight(90); }},  // kP = 0.5
    {52600, [this](){ setPD(12); pBow(90); }},  // kP = 0.6

    {60000, [this](){ launchChoreo(musicSequence2); }},
    {UINT32_MAX, nullptr}
  };

  ChoreographyStep musicSequence2[20] = {
    {0, [this](){ setPD(12); pBow(90); }},  // kP = 0.6
    {450, [this](){   setPD(10); pKickRight(90, 3000); }},  // back to bird - kP = 1.4
    {4600, [this](){  setPD(12); pBow(180, 3000); }},  // kP = 1
    {11140, [this](){ setPD(20); pBow(170); }},  // kP = 2
    {19870, [this](){ setPD(20); pBow(173); }},  // kP = 2
    {22000, [this](){ setPD(20); pBow(178); }},  // kP = 1.8
    {24300, [this](){ setPD(20); pStand(); }},  // val naar achteren - kP = 1.6
    {32470, [this](){ setPD(25); pBow(205, 30); }},  // kP = 1.6
    {33470, [this](){ setPD(25); pBow(180 ); }},  // kP = 1
    {38880, [this](){ setPD(25); pKickRight(90, 1150); }},  // kP = 1.6
    {43740, [this](){ setPD(25); pStand(1150); }},  // kP = 1.2
    {46870, [this](){ setPD(12); pBow(135); }},  // kP = 0.2
    {47250, [this](){ setPD(14); pBow(90); }},  // lig - kP = 0.8
    {56370, [this](){ setPD(15); pBow(180, 2200); }},  // kP = 0.4
    {59700, [this](){ setPD(12); pKickRight(90, 2000); }},  // rechts omhoog - kP = 0.6
    {60000, [this](){ launchChoreo(musicSequence3); }},  
    {UINT32_MAX, nullptr}  // End of sequence marker
  };

  ChoreographyStep musicSequence3[30] = {
    {0, [this](){     setPD(15); pKickRight(90, 1700); }},  // kP = 0.6
    {7600, [this](){  setPD(12); pLR(135, 90); }},  // trap naar split - kP = 0.5
    {8200, [this](){  setPD(12); pBow(90); }},  // kP = 0.5
    {8800, [this](){  setPD(12); pStepLeft(90, 1800); }},  // kP = 0.6
    {17500, [this](){ setPD(60); pStepLeft(40, 500); }},  // split opduwen - kP = 3
    {20720, [this](){ setPD(40); pStepLeft(90, 1000); }},  // kP = 0.6
    {31580, [this](){ setPD(30); pKickLeft(90, 0); }},  // split wissel - kP = 0.5
    {32580, [this](){ setPD(30); pStepRight(80, 1500); }},  // kP = 0.6
    {35650, [this](){ setPD(30); pBow(150); }},  // naar rug - kP = 1
    {36200, [this](){ setPD(30); pLR(150, 200, 1000); }},  // kP = 0.6
    {37200, [this](){ setPD(30); pLR(170, 180);} },  // kP = 1.2
    {37500, [this](){ setPD(30); pStand(); }},  // kP = 1.2
    {39700, [this](){ setPD(30); pStepRight(25, 1); }},  // rol naar zij - kP = 1.4
    {41550, [this](){ setPD(30); pLR(210, 120); }},  // kP = 1
    {43700, [this](){ setPD(30); pLR(190, 200); }},  // kP = 1.2
    {44630, [this](){ setPD(30); pLR(200, 130); }},  // kP = 1
    {45630, [this](){ setPD(30); pBow(190); }},  // buik - kP = 0.8
    {51950, [this](){ setPD(30); pKickRight(260); }},  // been omhoog - kP = 0.7
    {54020, [this](){ setPD(30); pBow(260); }},  // kP = 0.8
    {55020, [this](){ setPD(30); pBow(270); }},  // kP = 1.2
    {58090, [this](){ setPD(30); pStand(1200); }},  // staan - kP = 0.5
    {60000, [this](){ launchChoreo(musicSequence4); }},  // Original Value
    {UINT32_MAX, nullptr}  // End of sequence marker
  };

  ChoreographyStep musicSequence4[30] = {
    {0, [this](){     setPD(12); pStand(); }},  // Original KP: 1.2
    {185, [this](){   setPD(12); pStepRight(20); }},  // walk (kP = 1.4)
    {1250, [this](){  setPD(12); pStepLeft (20); }},
    {2215, [this](){  setPD(12); pStepRight(20); }},
    {3265, [this](){  setPD(12); pStepLeft(20); }},
    {4305, [this](){  setPD(12); pStepRight(20); }},
    {5350, [this](){  setPD(12); pStepLeft(20); }},
    {6435, [this](){  setPD(12); pStand(400); }},  // rug rol (kP = 1.7)
    {9790, [this](){  setPD(12); pBow(90,3000); }},  // (kP = 0.8)
    {15150, [this](){ setPD(12); pStand(400); }},  //  (kP = 0.6)
    {21740, [this](){ setPD(12); pBow(165); }},  // (kP = 0.8) // shoulder sit
    {22780, [this](){ setPD(12); pBow(150); }},
    {24380, [this](){ setPD(25); pBow(100); }},
    {28780, [this](){ setPD(12); pKickLeft(100); }},  // (kP = 0.8)
    {30885, [this](){ setPD(12); pStand(); }},  // uitbouw (kP = 1)
    {32120, [this](){ setPD(20); pBow(205); }},  // (kP = 1.5)
    {35436, [this](){ setPD(20); pStand(2000); }},  // (kP = 1.2)
    {40500, [this](){ setPD(20); pKickRight(200, 300 ); }},  // schouder snoek (kP = 1.8)
    {41100, [this](){ setPD(20); pKickRight(80, 300); }},
    {41400, [this](){ setPD(20); pStepRight(95, 300); }},
    {42100, [this](){ setPD(20); pBow(150, 400); }},  // (kP = 1.0)
    {43000, [this](){ setPD(25); pBow(190, 1000); }},  // kopstand (kP = 1.0)
    {48335, [this](){ setPD(20); pBow(130); }},  // (kP = 0.2)
    {48800, [this](){ setPD(12); pLR(110, 90); }},  // (kP = 0.5)
    {49999, [this](){ launchChoreo(musicSequence5); }},  // jump in time, 50 second sequence to match with full act sound timing
    {UINT32_MAX, nullptr}
  };

  ChoreographyStep musicSequence5[20] = {
    {0, [this](){ setPD(12); pLR(110, 90); }},
    {1925, [this](){  setPD(12); pLR(150, 90); }},  // starts going into headstand split (kP = 1.2)
    {3600, [this](){  setPD(20); pLR(270, 90, 3000); }},  // (kP = 0.8)
    {10335, [this](){ setPD(20); pBow(110, 2000); }},  // coming down (kP = 1.3)
    {14425, [this](){ setPD(12); pBow(150); }},  // (kP = 0.8)
    {15646, [this](){ setPD(12); pStand(); }},  // (kP = 1)
    {19309, [this](){ setPD(30); pBow(206, 10); }},  // (kP = 1.6) fall
    {21050, [this](){ setPD(30); pBow(170, 2000); }},  // (kP = 0.6)
    {52500, [this](){ setPD(30); pLR(150, 175); }},  // I don't care 2 (kP = 1.2)
    {53700, [this](){ setPD(30); pLR(205, 170);  }},  // (kP = 1.2)
    {54100, [this](){ setPD(30); pLR(185, 175);}},  // (kP = 0.4)
    {60000, [this](){ launchChoreo(musicSequence6); }},  // jump in time, 50 second sequence to match with full act sound timing
    {UINT32_MAX, nullptr}
  };

  ChoreographyStep musicSequence6[20] = {
    {0, [this](){ setPD(12); pLR(185, 175);}},
    {2450, [this](){  setPD(30); pBow(155); }},  // I want to see them (kP = 0.9)
    {4340, [this](){  setPD(30); pBow(175); }},  // (kP = 0.9)
    {8025, [this](){  setPD(30); pKickRight(140, 600); }},  // (kP = 0.9)
    {8800, [this](){  setPD(12); pStand(); }},  // (kP = 0.8)
    {14500, [this](){ setPD(30); pKickRight(160); }},  // (kP = 1.5)
    {14930, [this](){ setPD(30); pKickRight(140); }},  // (kP = 1.6)
    {15280, [this](){ setPD(30); pStepRight(12); }},  // (kP = 1.2)
    {22222, [this](){ setPD(30); pStand(); }},  // hello people (kP = 1.4)
    {29635, [this](){ setPD(30); pKickRight(130); }},  // (kP = 1)
    {31735, [this](){ setPD(12); pStand(); }},  // (kP = 1)
    {40575, [this](){ setPD(30); pBow(167); }},  // I'm ready (kP = 1)
    {41600, [this](){ setPD(12); pStand(); }},  // (kP = 1.2)
    {55485, [this](){ setPD(12); pKickRight(130); }},  // finale (kP = 1.2)
    {57240, [this](){ setPD(12); pLR(190, 120); }},  // (kP = 1.2)
    {59400, [this](){ setPD(20); pBow(160, 300); }},  // (kP = 1.2)
    {60000, [this](){ launchChoreo(musicSequence7); }},  // jump in time, 50 second sequence to match with full act sound timing
    {UINT32_MAX, nullptr}
  };

  ChoreographyStep musicSequence7[35] = {
    {0, [this](){     setPD(25); pBow(160, 100); }},  // (kP = 1.2)
    {1765, [this](){  setPD(23); pLR(100, 160, 300);}},
    {2360, [this](){  setPD(25); pLR(110, 170, 300);}},
    {3435, [this](){  setPD(30); pStand(400); }},  // (kP = 1.3)
    {12204, [this](){ setPD(40); pKickRight(70, 2800); }},  // (kP = 1) // leg up
    {16426, [this](){ setPD(20); pStand(1200); }},  // stand (kP = 0.4)
    {23188, [this](){ setPD(12); pBow(100); }},  // bows (kP = 0.6)
    {24741, [this](){ setPD(12); pStand(); }},  // (kP = 1.0)
    {28740, [this](){ setPD(12); pStepRight(18); }},  // walk (kP = 1.6)
    {29245, [this](){ setPD(12); pStepLeft(18); }},  // (kP = 1.5)
    {29995, [this](){ setPD(12); pStepRight(18); }},  // (kP = 1.5)
    {30680, [this](){ setPD(12); pStand(); }},  // (kP = 1.7)
    {32190, [this](){ setPD(12); pBow(100); }},  // (kP = 0.6) breathe?
    {33765, [this](){ setPD(12); pStand(); }},  // (kP = 1.5)
    {36975, [this](){ setPD(25); pBow(175, 1000); }},  // mini bow to me(kP = 0.8)
    {38170, [this](){ setPD(25); pStand(); }},  // (kP = 1.2)
    {41885, [this](){ setPD(25); pBow(160, 200); }},  // hug (kP = 2)
    {44625, [this](){ setPD(12); pStand(); }},  // (kP = 1)
    {49526, [this](){ setPD(12); pStepRight(20); }},  // walk (kP = 1.5)
    {50250, [this](){ setPD(12); pStepLeft(20); }},  // (kP = 1.5)
    {51025, [this](){ setPD(12); pStepRight(20); }},  // (kP = 1.5)
    {51740, [this](){ setPD(12); pStepLeft(20); }},  // (kP = 1.5)
    {52515, [this](){ setPD(12); pStepRight(20); }},  // (kP = 1.5)
    {53245, [this](){ setPD(12); pStepLeft(20); }},  // (kP = 1.5)
    {53960, [this](){ setPD(12); pStepRight(20); }},  // (kP = 1.5)
    {54690, [this](){ setPD(12); pStand(); }},  // (kP = 1.5)
    {60000, [this](){ setPD(12); launchChoreo(musicSequence8); }} ,
    {UINT32_MAX, nullptr}
  };

  ChoreographyStep musicSequence8[16] = {
    {0, [this](){     setPD(9);     pStand(); }},  // (kP = 1.2)
    {8090, [this](){  setPD(9);  pKickRight(80, 300); }},  // (kP = 1.5)
    {8790, [this](){  setPD(9);  pKickLeft(80, 300); }},  // (kP = 1.5)
    {9480, [this](){  setPD(12);  pBow(80,400); }},  // (kP = 1.5)
    {10320, [this](){ setPD(12); pStand(2000); }},  // (kP = 0.2)
    {17500, [this](){ setPD(20); pStepRight(80, 400); }},  // splits (kP = 1.2)
    {18400, [this](){ setPD(30); pStepLeft(80, 400); }},  // (kP = 1.2)
    {19300, [this](){ setPD(30); pStepRight(80, 400); }},  // (kP = 1.2)
    {20200, [this](){ setPD(30); pStepLeft(80, 400); }},  // (kP = 1.2)
    {21475, [this](){ setPD(20); pBow(110, 400); }},  // forward backward (kP = 1.0)
    {22200, [this](){ setPD(30); pBow(250, 400); }},  // (kP = 1.0)
    {22600, [this](){ setPD(40); pBow(110, 400); }},  // (kP = 1.0)
    {23455, [this](){ setPD(50); pBow(250, 400); }},  // (kP = 1.4)
    {23725, [this](){ setPD(50); pBow(110, 400); }},  // (kP = 1.0)
    {24900, [this](){ setPD(20); pStand(1000); }} , // (kP = 0.3)
    {UINT32_MAX, nullptr}
  };

  // clang-format on
};

#endif // CHOREOPLAYER_H
