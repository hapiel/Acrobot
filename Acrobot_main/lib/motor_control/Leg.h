#ifndef LEG_H
#define LEG_H

#include "Limb.h"

class Leg : public Limb
{
public:
  Leg(Motor &motor, HallSensor &hallSensor, int motorID, float offset180, bool inverted);
  // Additional functions specific to Leg can be declared here


  
};

#endif // LEG_H
