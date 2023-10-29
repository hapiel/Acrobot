#ifndef BOTTANGOPLAYER_H
#define BOTTANGOPLAYER_H

#include "Arduino.h"
#include "Leg.h"
#include "Arm.h"
#include "utilsAcrobot.h"
#include "RemoteDebug.h"
#include "FloatBezierCurve.h"

class BottangoPlayer
{
public:
  BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR);
  void update();
  void start();
  void stop();
  bool enabled = 0;

private:

  unsigned long int starttime;
  unsigned long int currenttime;
  float kp = 10.0;
  float ki = 2.0;
  int legLBezier_testdata[8] = {0, 733,4511,183,0,3686,-183,0};
  int legRBezier_testdata[8] = {0, 733,3686,183,0,4506,-183,0};
  int armLBezier_testdata[8] = {0, 733, 4342, 183, 0, 3850, -183,0};
  int armRBezier_testdata[8] = {0, 733, 3850, 183, 0, 4342, -183,0};
  FloatBezierCurve legLBezier = FloatBezierCurve(legLBezier_testdata[0], legLBezier_testdata[1], legLBezier_testdata[2], legLBezier_testdata[3], legLBezier_testdata[4], legLBezier_testdata[5], legLBezier_testdata[6], legLBezier_testdata[7]);
  FloatBezierCurve legRBezier = FloatBezierCurve(legRBezier_testdata[0], legRBezier_testdata[1], legRBezier_testdata[2], legRBezier_testdata[3], legRBezier_testdata[4], legRBezier_testdata[5], legRBezier_testdata[6], legRBezier_testdata[7]);
  FloatBezierCurve armLBezier = FloatBezierCurve(armLBezier_testdata[0], armLBezier_testdata[1], armLBezier_testdata[2], armLBezier_testdata[3], armLBezier_testdata[4], armLBezier_testdata[5], armLBezier_testdata[6], armLBezier_testdata[7]);
  FloatBezierCurve armRBezier = FloatBezierCurve(armRBezier_testdata[0], armRBezier_testdata[1], armRBezier_testdata[2], armRBezier_testdata[3], armRBezier_testdata[4], armRBezier_testdata[5], armRBezier_testdata[6], armRBezier_testdata[7]);
  RemoteDebug &Debug;
  Leg &legL;
  Leg &legR;
  Arm &armL;
  Arm &armR;

  float mapBeziervalue(float value);
};

#endif // BOTTANGOPLAYER_H
