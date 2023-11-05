#ifndef UTILS_ACROBOT_H
#define UTILS_ACROBOT_H

#include <Arduino.h>


// for testing & sending periodical messages
// usage:
//static long executionTimer = 0;
//  if (runEvery(1000, executionTimer))
//  {
//    doThisEverySecond();
//  }

inline bool runEvery(int interval, long &nextExecutionMillis)
{
  long currentMillis = millis();
  if (nextExecutionMillis - currentMillis >= 0)
  {
    return false;
  }
  nextExecutionMillis = ((currentMillis / interval) + 1) * interval;
  return true;
}

inline float fMap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




#endif // UTILS_ACROBOT_H