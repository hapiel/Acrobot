#include "Leg.h"

Leg::Leg(Motor &motor, float offset180, bool inverted) : motor(motor), offset180(offset180), inverted(inverted) {}

void Leg::setTarget(float posDegrees, float kp, float kd) {

  target = posDegrees;
  if (inverted) {
    posDegrees = 360 - posDegrees;
  }

  float posOffsetCorrected = correctOffsetShaftToMotor(constrain(posDegrees, posMin, posMax));
  float posToSend = degreesToRad(posOffsetCorrected);
  float kpToSend = constrain(kp, 0, kpLimit);
  float kdToSend = constrain(kd, kdMinimum, 5);

  motor.setPosition(posToSend, kpToSend, kd);
}

float Leg::getTarget(){
  return target;
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


  float posDegrees = radToDegrees(motor.getPosition());
  float posOffsetCorrected = correctOffsetMotorToShaft(posDegrees);

  if (inverted) {
    posOffsetCorrected = 360 - posOffsetCorrected;
  }

  return posOffsetCorrected;
}

float Leg::getVelocity() {
  return motor.getVelocity();
}

float Leg::getTorque() {
  return motor.getTorque();
}

uint8_t Leg::getTemperature() {
  return motor.getTemperature();
}

uint8_t Leg::getErrorCode() {
  return motor.getErrorCode();
}

float Leg::radToDegrees(float rad) {
  return rad * 180.0 / PI;
}

float Leg::degreesToRad(float degrees) {
  return degrees * PI / 180.0;
}

float Leg::correctOffsetShaftToMotor(float shaftDegrees) {
  return shaftDegrees - 180 + offset180;
}

float Leg::correctOffsetMotorToShaft(float motorDegrees) {
  return motorDegrees + 180 - offset180;
}