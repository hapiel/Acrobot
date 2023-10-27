#include "Arm.h"

Arm::Arm(Motor &motor, HallSensor &hallSensor, int motorID, float offset180, bool inverted)
  : Limb(motor, hallSensor, motorID, offset180, inverted)
{
  posMin = -180;
  posMax = 360;
  calibrationDegreesLow = 167; // minimum value just outside of calibration range. 167 for sleeveless, 170 for sleeved. 167 is probably fine for both.
  calibrationTreshold = 9400; //  7900 for sleevles legs, 9400 for sleeved legs
}
