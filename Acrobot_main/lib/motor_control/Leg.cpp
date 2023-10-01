#include "Leg.h"

Leg::Leg(Motor &motor) : motor(motor) {}

void Leg::setPosition(float posDegrees, float kp, float kd) {

  float posToSend = degreesToRad(constrain(posDegrees, posMin, posMax));
  float kpToSend = constrain(kp, 0, kpLimit);
  float kdToSend = constrain(kd, kdMinimum, 5);

  motor.setPosition(posToSend, kpToSend, kd);
}

void Leg::start() {
  motor.start();
}

void Leg::stop() {
  motor.stop();
}

void Leg::update() {
  motor.update();
}

float Leg::getPosition() {
  return radToDegrees(motor.getPosition());
}

float Leg::getVelocity() {
  return motor.getVelocity();
}

float Leg::getTorque() {
  return motor.getTorque();
}

float Leg::getTemperature() {
  return motor.getTemperature();
}

float Leg::getErrorCode() {
  return motor.getErrorCode();
}

float Leg::radToDegrees(float rad) {
  return rad * 180.0 / PI;
}

float Leg::degreesToRad(float degrees) {
  return degrees * PI / 180.0;
}