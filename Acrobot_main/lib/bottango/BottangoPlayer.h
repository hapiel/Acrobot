#ifndef BOTTANGOPLAYER_H
#define BOTTANGOPLAYER_H

#include "Arduino.h"
#include "Leg.h"
#include "Arm.h"
#include "utilsAcrobot.h"
#include "RemoteDebug.h"
#include "FloatBezierCurve.h"
#include "SD.h"
#include <CSV_Parser.h>

const int bezierBufferLenght = 10;

class BottangoPlayer
{
public:
  BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL,  Arm &armR, File &file, CSV_Parser &cp);
  void update();

  void start();
  void stop();

  void loadFile(const char *csvDir);
  void loadNextFile(char csvDir[256]);
  bool checkNextFileReady();

  void setMotorKpKi(float kp = 5.0, float ki = 2.0);

  void closeCSV();
private:
  FloatBezierCurve armLBezier = FloatBezierCurve();
  FloatBezierCurve armRBezier = FloatBezierCurve();
  FloatBezierCurve legLBezier = FloatBezierCurve();
  FloatBezierCurve legRBezier = FloatBezierCurve();

  RemoteDebug &Debug;
  Arm &armL;
  Arm &armR;
  Leg &legL;
  Leg &legR;

  File &file;
  CSV_Parser &cp;

  float readNextCSVLine();
  void checkEndOfCurve();
  void openCSV();
  void readAndParseCSVRow();
  int sumAllReadWritePointers();

  char currentFileDir[256];
  char nextFileDir[256];
  bool enabled = false;
  bool csvDisabledFlag = false;
  bool fileReady = false;
  unsigned long int starttime;
  unsigned long int currenttime;
  float kp = 5.0;
  float ki = 2.0;

  int csvRowIndex = 0;

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
};

#endif // BOTTANGOPLAYER_H
