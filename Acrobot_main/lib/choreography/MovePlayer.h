#ifndef MOVEPLAYER_H
#define MOVEPLAYER_H

#include "Arduino.h"
#include "Arm.h"
#include "Leg.h"

#include "RemoteDebug.h"
#include "FloatBezierCurve.h"
#include "SD.h"
#include <CSV_Parser.h>
#include "ChoreoPlayer.h"

enum MovePlayerState
{
  IDLE,
  MOVE_TO_BEGIN,
  MOVE_CURVES
};

class MovePlayer
{
public:
  MovePlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp, ChoreoPlayer &choreoPlayer);
  void update();
  void stop();
  void startMove(const char *csvDir, bool beginPosOnly = false, bool repeat = false, float moveKp = 20.0, float moveKi = 2.0, float _startMoveSpeed = 45);
  bool isIdle();

private:
  RemoteDebug &Debug;
  File &file;
  CSV_Parser &cp;
  ChoreoPlayer &choreoPlayer;

  static const uint8_t NUM_CURVES = 4;

  Limb *limbs[NUM_CURVES]; // order: 0: ArmL, 1: armR, 2: legL, 3: legR
  FloatBezierCurve *curves[NUM_CURVES] = {nullptr, nullptr, nullptr, nullptr};

  MovePlayerState state = IDLE;

  const float START_DISTANCE_TOLERANCE = 1.0;

  float kp = 20.0;
  float ki = 2.0;

  float startPositions[4];
  float startMoveSpeed = 45; // degrees/s

  bool moveToBeginOnly = false;
  char currentFileDir[256];

  bool fileEnded = true;
  bool repeatMove = false;

  uint32_t moveStartTime = 0;
  uint32_t lastStartMovementTime[NUM_CURVES] = {0,0,0,0};
  uint32_t updateMillis = 0;
  uint32_t moveMillis();

  float nextCurve[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  const u8_t NO_INDEX = 255;
  u8_t nextCurveIndex = 255;

  bool limbActive(int limbIndex);
  bool loadFile(const char *csvDir);
  void parseStartCSV();
  void moveTowardsStart(int limbIndex);
  void startCurves();
  void readCurve();
  bool needToReadCurve();
  void moveFinished();
  void resetCurve(uint8_t index);
  void resetAllCurves();
};

#endif // MOVEPLAYER_H