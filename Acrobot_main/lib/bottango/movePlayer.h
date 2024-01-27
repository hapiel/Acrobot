#ifndef MOVEPLAYER_H
#define MOVEPLAYER_H

#include "Arduino.h"
#include "Arm.h"
#include "Leg.h"

#include "RemoteDebug.h"
#include "FloatBezierCurve.h"
#include "SD.h"
#include <CSV_Parser.h>

enum MovePlayerState
{
  IDLE,
  MOVE_TO_BEGIN,
  MOVE_CURVES
};

class MovePlayer
{
public:
  MovePlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp, CSV_Parser &cpStart);
  void update();
  void stop();
  void startMove(const char *csvDir, bool beginPosOnly = false, float moveKp = 5.0, float moveKi = 2.0);

private:
  // FloatBezierCurve armLBezier = FloatBezierCurve(); // replace with array
  // FloatBezierCurve armRBezier = FloatBezierCurve();
  // FloatBezierCurve legLBezier = FloatBezierCurve();
  // FloatBezierCurve legRBezier = FloatBezierCurve();

  FloatBezierCurve *curves[4] = {nullptr, nullptr, nullptr, nullptr};

  RemoteDebug &Debug;
  File &file;
  CSV_Parser &cp;
  CSV_Parser &cpStart;
  Limb *limbs[4]; // order: 0: ArmL, 1: armR, 2: legL, 3: legR

  MovePlayerState state = IDLE;

  const float START_DISTANCE_TOLERANCE = 3.0;
 

  float kp = 5.0;
  float ki = 2.0;

  float startPositions[4];
  float startMoveSpeed = 50; // degrees/s

  bool moveToBeginOnly = false;
  char currentFileDir[256];

  bool fileEnded = true;

  uint32_t moveStartTime = 0;

  uint32_t lastStartMovementTime = 0;
  uint32_t updateMillis = 0;
  uint32_t moveMillis();
  bool needToReadCurve();

  float nextCurve[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  const u8_t NO_INDEX = 255;
  u8_t nextCurveIndex = 255;

  bool limbActive(int limbIndex);
  bool loadFile(const char *csvDir);
  void parseStartCSV();
  void moveTowardsStart(int limbIndex);
  void startCurves();
  void readCurve();
};

#endif // MOVEPLAYER_H