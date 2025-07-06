#include "Arm.h"

Arm::Arm(Motor &motor, HallSensor &hallSensor, RemoteDebug &Debug,DebugLed &debugLed, int motorID, float offset180, bool inverted, int16_t _calibrationTreshold)
  : Limb(motor, hallSensor, Debug, debugLed, motorID, offset180, inverted)
{
  posMin = -1980;
  posMax = 2340;
  if (inverted){
    posMax = 360 - posMin;
    posMin = 360 - posMax;
  }
  calibrationDegreesLow = 167; // minimum value just outside of calibration range.
  calibrationTreshold = _calibrationTreshold;
  // calibrationTreshold = 10750; //  10500
}
