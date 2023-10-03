#ifndef STATUS_CHECKER_H
#define STATUS_CHECKER_H

#include <Arduino.h>
#include "WiFi.h"
#include "RemoteDebug.h"
#include "BatterySensor.h"
#include "Buzzer.h"
#include "DebugLed.h"
#include "Joystick.h"
#include "EStop.h"

class StatusChecker
{
public:
  StatusChecker(RemoteDebug& Debug, BatterySensor &batterySensor, Buzzer &buzzer, DebugLed &debugLed, Joystick &joystick, EStop &eStop);
  void update();

private:
  RemoteDebug& Debug;
  BatterySensor &batterySensor;
  Buzzer &buzzer;
  DebugLed &debugLed;
  Joystick &joystick;
  EStop &eStop;
  uint32_t lastInfoTime = 0;
  uint32_t lastWarningTime = 0;
  uint32_t lastFlickerTime = 0;
  int flickerRate = 1000;

  void checkBattery();

};

#endif //STATUS_CHECKER_H
