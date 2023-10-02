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
  int getAdc();
  float getVoltage();
  void update();
  float fmap(float x, float in_min, float in_max, float out_min, float out_max);

private:
  RunningMedian batterySamples = RunningMedian(64);
  uint8_t pin;
  static const int chartSize = 21;
  // these values are based on a lipo chart table and our measurements, can be found in our Notion/battery
  // voltage, adc output, percentage
  const float chart[chartSize][3] = {
      {22, 10, 9},
      {22.2, 20, 13},
      {22.3, 50, 17},
      {22.4, 208, 22},
      {22.6, 440, 30},
      {22.8, 650, 41},
      {23, 880, 50},
      {23.2, 1100, 58},
      {23.4, 1335, 63},
      {23.6, 1560, 67},
      {23.8, 1780, 72},
      {24, 2000, 77},
      {24.2, 2250, 81},
      {24.4, 2470, 84},
      {24.6, 2690, 90},
      {24.8, 2920, 93},
      {25, 3160, 97},
      {25.2, 3450, 99},
      {25.4, 3800, 99},
      {25.6, 4020, 99},
      {25.78, 4095, 99}};
};

#endif
