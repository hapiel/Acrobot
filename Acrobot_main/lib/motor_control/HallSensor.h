#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include "ADS1X15.h"
#include "Wire.h"

class HallSensor
{
public:
  HallSensor(TwoWire &wire, RemoteDebug& Debug);
  void init();
  int16_t getArmL();
  int16_t getArmR();
  int16_t getLegL();
  int16_t getLegR();

private:
  RemoteDebug& Debug;
  TwoWire &wire;
  ADS1115* ADS;
  uint8_t address = 0x48;

};

#endif // HALL_SENSOR_H

