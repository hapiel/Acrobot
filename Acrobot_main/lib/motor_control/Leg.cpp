#include "Leg.h"

Leg::Leg(Motor &motor, HallSensor &hallSensor, int motorID, float offset180, bool inverted)
    : Limb(motor, hallSensor, motorID, offset180, inverted)
{
  posMin = 30;
  posMax = 330;
  calibrationDegreesLow = 167; // minimum value just outside of calibration range. 167 for sleeveless, 170 for sleeved. 167 is probably fine for both.
  calibrationTreshold = 10000; //  7900 for sleevles legs, 9400 for sleeved legs
}
