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
  float kp = 5.0;
  float ki = 2.0;
  float legLBezier_testdata[8] = {0,3000,193.677978515625,750,0.0,166.48681640625,-750,0.0};
  float legRBezier_testdata[8] = {0,3000,166.48681640625,750,0.0,193.51318359375,-750,0.0};
  float armLBezier_testdata[8] = {0,3000,158.37890625,750,0.0,201.62109375,-175,0.0};
  float armRBezier_testdata[8] = {0,3000,201.62109375,750,0.0,158.37890625,-750,-1.93359375};  
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
