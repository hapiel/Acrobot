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
  HallSensor(TwoWire &Wire, RemoteDebug &Debug);
  void init();
  void update();

  int16_t getValFromID(int motorID);
  bool getReadyFromID(int motorID, int16_t calibrationTreshold);


private:
  RemoteDebug &Debug;
  TwoWire &Wire;
  ADS1115 *ADS;
  uint8_t address = 0x48;
  static const uint8_t SENSOR_COUNT = 4;
  int motorIDToSensorTable[SENSOR_COUNT]; // ID-1, so motor ID 1 is stored in array[0]. Values are set in constructor
  int16_t sensorValueOfId[SENSOR_COUNT] ;
  SemaphoreHandle_t mutex; 

};

#endif // HALL_SENSOR_H
