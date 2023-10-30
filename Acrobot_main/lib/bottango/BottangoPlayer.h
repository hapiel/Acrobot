#ifndef BOTTANGOPLAYER_H
#define BOTTANGOPLAYER_H

#include "Arduino.h"
#include "Leg.h"
#include "Arm.h"
#include "utilsAcrobot.h"
#include "RemoteDebug.h"
#include "FloatBezierCurve.h"
#include "SdFat.h"
#include "sdios.h"

const int bezierBufferLenght = 30;

class BottangoPlayer
{
public:
  BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, SdExFat &sd);
  void update();
  void start();
  void stop();
  void loadFileAndPlay(char csvDir [256]);


private:
  bool enabled = 0;
  unsigned long int starttime;
  unsigned long int currenttime;
  float kp = 5.0;
  float ki = 2.0;

  void checkEndOfCurve();

  bool armLEnabled = 0;
  bool armREnabled = 0;
  bool legLEnabled = 0;
  bool legREnabled = 0;

  float armLCurveArray[bezierBufferLenght][8];
  float armRCurveArray[bezierBufferLenght][8];
  float legLCurveArray[bezierBufferLenght][8];
  float legRCurveArray[bezierBufferLenght][8];

  unsigned int armLCurveRead = 0;
  unsigned int armRCurveRead = 0;
  unsigned int legLCurveRead = 0;
  unsigned int legRCurveRead = 0;
  unsigned int armLCurveWrite = 0;
  unsigned int armRCurveWrite = 0;
  unsigned int legLCurveWrite = 0;
  unsigned int legRCurveWrite = 0;
  unsigned int armLCurveLastMove = 0;
  unsigned int armRCurveLastMove = 0;
  unsigned int legLCurveLastMove = 0;
  unsigned int legRCurveLastMove = 0;

  FloatBezierCurve armLBezier = FloatBezierCurve();
  FloatBezierCurve armRBezier = FloatBezierCurve();
  FloatBezierCurve legLBezier = FloatBezierCurve();
  FloatBezierCurve legRBezier = FloatBezierCurve();

  RemoteDebug &Debug;
  Arm &armL;
  Arm &armR;
  Leg &legL;
  Leg &legR;
  SdExFat &sd;

};

#endif // BOTTANGOPLAYER_H
