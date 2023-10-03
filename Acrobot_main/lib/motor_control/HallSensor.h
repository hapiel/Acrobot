#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include "ADS1X15.h"
#include "Wire.h"
#include "motorIDs.h"

class HallSensor
{
public:
  HallSensor(TwoWire &wire, RemoteDebug &Debug);
  void init();
  void update();

  int16_t getValFromID(int motorID);
  bool getReadyFromID(int motorID);


private:
  RemoteDebug &Debug;
  TwoWire &wire;
  ADS1115 *ADS;
  uint8_t address = 0x48;
  int motorIDToSensorTable[4]; // ID-1, so motor ID 1 is stored in array[0]. Values are set in constructor
  int16_t sensorValueOfId[4] ;
  int16_t calibrationTreshold = 7500; // 7500 for sleevless, 9000 for sleeved

};

#endif // HALL_SENSOR_H
