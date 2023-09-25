#ifndef BATTERY_SENSOR_H
#define BATTERY_SENSOR_H

#include <Arduino.h>
#include <RunningMedian.h>

class BatterySensor
{
public:
  BatterySensor(uint8_t pin);
  void init();
  int getPercentage();
  void update();

private:
  RunningMedian batterySamples = RunningMedian(64);
  uint8_t pin;
};

#endif
