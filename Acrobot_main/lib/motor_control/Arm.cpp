#include "Arm.h"

Arm::Arm(Motor &motor, HallSensor &hallSensor, RemoteDebug &Debug,DebugLed &debugLed, int motorID, float offset180, bool inverted)
  : Limb(motor, hallSensor, Debug, debugLed, motorID, offset180, inverted)
{
  posMin = -180;
  posMax = 540;
  if (inverted){
    posMax = 360 - posMin;
    posMin = 360 - posMax;
  }
  calibrationDegreesLow = 167; // minimum value just outside of calibration range. 167 for sleeveless, 170 for sleeved. 167 is probably fine for both.
  calibrationTreshold = 10500; //  11500
}
