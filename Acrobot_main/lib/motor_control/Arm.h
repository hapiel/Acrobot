#pragma once 

#include "Limb.h"

class Arm : public Limb
{
public:
  Arm(Motor &motor, HallSensor &hallSensor, RemoteDebug &Debug,DebugLed &debugLed, int motorID, float offset180, bool inverted, int16_t _calibrationTreshold);


  
};


