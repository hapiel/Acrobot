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

#endif // UTILS_ACROBOT_H