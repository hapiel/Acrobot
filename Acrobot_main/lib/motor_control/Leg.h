#ifndef LEG_H
#define LEG_H

#include "Motor.h"

class Leg
{
public:
  Leg(Motor &motor);
  void setPosition(float posDegrees, float kp, float kd);
  void start();
  void stop();
  void update();

  float getPosition();
  float getVelocity();
  float getTorque();
  float getTemperature();
  float getErrorCode();

private:
  Motor &motor;
  float kpLimit = 100; // safety feature, can be removed later
  float kdMinimum = 1; // safety feature, can be removed later
  float posMin = 90;
  float posMax = 270;

  float radToDegrees(float rad);
  float degreesToRad(float degrees);

};

#endif // LEG_H 