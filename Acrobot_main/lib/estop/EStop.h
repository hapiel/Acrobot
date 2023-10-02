#ifndef ESTOP_H
#define ESTOP_H

#include <Arduino.h>
#include "RemoteDebug.h"

class EStop
{
public:
  EStop(int pin, RemoteDebug &Debug);
  void set();
  void update();
  bool getSet();
  void init();

private:
  void unSet();
  uint32_t lastTimeSet;
  int pin;
  bool isSet = false;
  bool initialUnset = false;
  int setDuration = 300; // time in ms before it can be manually released
  RemoteDebug &Debug;
};

#endif