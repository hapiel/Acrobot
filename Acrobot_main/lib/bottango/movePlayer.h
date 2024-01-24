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
  MovePlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL,  Arm &armR, File &file, CSV_Parser &cp);
  void update();
  void stop();
  void startMove(const char *csvDir, float kp = 5.0, float ki = 2.0);
  void moveToBegin(const char *csvDir, float kp = 5.0, float ki = 2.0);

private:

  FloatBezierCurve armLBezier = FloatBezierCurve();
  FloatBezierCurve armRBezier = FloatBezierCurve();
  FloatBezierCurve legLBezier = FloatBezierCurve();
  FloatBezierCurve legRBezier = FloatBezierCurve();

  RemoteDebug &Debug;

  Limb *limbs[4]; // order: ArmL, armR, legL, legR

  File &file;
  CSV_Parser &cp;

  MovePlayerState state = IDLE;



}

#endif // MOVEPLAYER_H