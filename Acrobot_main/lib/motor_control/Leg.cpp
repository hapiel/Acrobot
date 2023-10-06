#include "Leg.h"

Leg::Leg(Motor &motor, HallSensor &hallSensor, int motorID, float offset180, bool inverted) : motor(motor), hallSensor(hallSensor), motorID(motorID), offset180(offset180), inverted(inverted) {}

void Leg::setTarget(float posDegrees, float kp, float kd) {

  if (state != ON) {
    return;
  }

  target = posDegrees;
  if (inverted) {
    posDegrees = 360 - posDegrees;
  }

  float posOffsetCorrected = correctOffsetShaftToMotor(constrain(posDegrees, posMin, posMax));


  float posToSend = degreesToRad(posOffsetCorrected);

  float kpToSend = constrain(kp, 0, kpLimit);
  // ramp up kp after start
  if (millis() - startTime < startRampDuration) {
    kpToSend = min(kpToSend, fMap(millis() - startTime, 0, startRampDuration, kPLimitStart, kPlimitRampEnd));
  }

  float kdToSend = constrain(kd, kdMinimum, 5);
  // ramp down kd
  if (millis() - startTime < startRampDuration) {
    kdToSend = max(kdToSend, fMap(millis() - startTime, 0, startRampDuration, kDMinimumStart, kdMinimum));
  }

  motor.setPosition(posToSend, kpToSend, kdToSend);
}

float Leg::getTarget(){
  return target;
}

void Leg::start() {
  motor.start();
  state = ON;
  startTime = millis();
  motor.setPosition(0, 0, 0);
}

void Leg::stop() {
  motor.stop();
  state = OFF;
}


void Leg::startCalibration() {
  state = CALIBRATION;
}

void Leg::tryCalibration() {
  if (hallSensor.getReadyFromID(motorID)){
    if (motor.isOnline()){

      int offsetSteps = floor((getPosition() - calibrationDegreesLow)/offsetDegrees);
      if (inverted){
        offsetGearbox = (offsetSteps) * offsetDegrees;
      } else {
        offsetGearbox = (offsetSteps * -1) * offsetDegrees;
      }
      start();
    }
  }
}

State Leg::getState(){
  return state;
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
  return shaftDegrees - 180 + offset180 - offsetGearbox;
}

float Leg::correctOffsetMotorToShaft(float motorDegrees) {
  return motorDegrees + 180 - offset180 + offsetGearbox;;
}

void Leg::update() {
  if (!motor.isOnline()){
    offsetGearbox = 0; // reset offset when power is off.
    state = OFF;
  }

  if (state == CALIBRATION) {
    tryCalibration();
  }
  motor.update();
}
