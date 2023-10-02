#ifndef LEG_H
#define LEG_H

#include "Motor.h"

class Leg
{
public:
  Leg(Motor &motor, float offset180, bool inverted);
  void setTarget(float posDegrees, float kp, float kd);
  void start();
  void stop();
  void update();

  float getTarget();
  float getPosition();
  float getVelocity();
  float getTorque();
  uint8_t getTemperature();
  uint8_t getErrorCode();

private:
  Motor &motor;
  float kpLimit = 100; // safety feature, can be removed later
  float kdMinimum = 1; // safety feature, can be removed later
  float posMin = 90;
  float posMax = 270;
  float offset180;
  float target;
  bool inverted;

  float radToDegrees(float rad);
  float degreesToRad(float degrees);
  float correctOffsetShaftToMotor(float degrees);
  float correctOffsetMotorToShaft(float degrees);
};

#endif // LEG_H