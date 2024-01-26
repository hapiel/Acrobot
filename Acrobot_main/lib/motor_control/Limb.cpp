#include "Limb.h"

Limb::Limb(Motor &motor, HallSensor &hallSensor, int motorID, float offset180, bool inverted) : motor(motor), hallSensor(hallSensor), motorID(motorID), offset180(offset180), inverted(inverted) {}

void Limb::setTarget(float posDegrees, float kp, float kd)
{

  if (state != STATE_ON)
  {
    return;
  }

  if (lastControlMode != CONTROL_MODE_TARGET)
  {
    lastTarget = getPosition();
  }
  
  lastControlMode = CONTROL_MODE_TARGET;

  int safeRange = constrain(SAFE_TARGET_RANGE_MAX - 0.6 * kp   , SAFE_TARGET_RANGE_MIN, SAFE_TARGET_RANGE_MAX);

  // check if target is within safe range
  if (lastTarget + safeRange < posDegrees || lastTarget - safeRange > posDegrees)
  {
    // if not, start lerp to target
    targetSafetyLerpOriginalTarget = lastTarget;
    targetSafetyLerpStartTime = millis();
    targetSafetyLerpDuration = abs(lastTarget - posDegrees) * TARGET_SAFETY_DURATION_FACTOR;
  }

  // within lerp time, lerp to target
  if (targetSafetyLerpStartTime + targetSafetyLerpDuration > millis())
  {
    

    float lerpFactor = fMap(millis() - targetSafetyLerpStartTime, 0, targetSafetyLerpDuration, 0, 100);

    posDegrees = fMap(lerpFactor, -1, 100, targetSafetyLerpOriginalTarget, posDegrees);

  }

  lastTarget = posDegrees;

  if (inverted)
  {
    posDegrees = 360 - posDegrees;
  }

  float posOffsetCorrected = correctOffsetShaftToMotor(constrain(posDegrees, posMin, posMax));

  float posToSend = degreesToRad(posOffsetCorrected);

  float kpToSend = constrain(kp, 0, kpLimit);

  // ramp up kp after start
  if (millis() - startTime < startRampDuration)
  {
    kpToSend = min(kpToSend, fMap(millis() - startTime, 0, startRampDuration, kPLimitStart, kPlimitRampEnd));
  }

  float kdToSend = constrain(kd, kdMinimum, 5);
  // ramp down kd after start
  if (millis() - startTime < startRampDuration)
  {
    kdToSend = max(kdToSend, fMap(millis() - startTime, 0, startRampDuration, kDMinimumStart, kdMinimum));
  }

  motor.setPosition(posToSend, kpToSend, kdToSend);
}

void Limb::setTorqueUnprotected(float torque)
{
  lastControlMode = CONTROL_MODE_TORQUE;
  
  if (inverted)
  {
    torque = -torque;
  }
  motor.setTorque(torque);
}

float Limb::getTarget()
{
  return lastTarget;
}

void Limb::start()
{
  motor.start();
  state = STATE_ON;
  lastControlMode = CONTROL_MODE_NONE;
  startTime = millis();

  // set motor inactive. Can't be set active immediately after start because of ramping feature in setTarget.
  motor.setPosition(0, 0, 0);
  
}

void Limb::stop()
{
  motor.stop();
  state = STATE_OFF;
  lastControlMode = CONTROL_MODE_NONE; // possibly redundant, better safe than sorry
}

void Limb::startCalibration()
{
  if (state == STATE_ON)
  {
    return;
  }
  state = STATE_CALIBRATION;
}

void Limb::tryCalibration()
{
  if (hallSensor.getReadyFromID(motorID, calibrationTreshold))
  {
    if (motor.isOnline())
    {

      int offsetSteps = floor((getPosition() - calibrationDegreesLow) / offsetDegrees);
      if (inverted)
      {
        offsetGearbox = (offsetSteps)*offsetDegrees;
      }
      else
      {
        offsetGearbox = (offsetSteps * -1) * offsetDegrees;
      }
      start();
    }
  }
}

State Limb::getState()
{
  return state;
}

float Limb::getPosition()
{

  float posDegrees = radToDegrees(motor.getPosition());
  float posOffsetCorrected = correctOffsetMotorToShaft(posDegrees);

  if (inverted)
  {
    posOffsetCorrected = 360 - posOffsetCorrected;
  }

  return posOffsetCorrected;
}

float Limb::getVelocity()
{
  float vel = motor.getVelocity();
  if (inverted)
  {
    vel = -vel;
  }
  return vel;
}

float Limb::getTorque()
{
  return motor.getTorque();
}

uint8_t Limb::getTemperature()
{
  return motor.getTemperature();
}

uint8_t Limb::getErrorCode()
{
  return motor.getErrorCode();
}

float Limb::radToDegrees(float rad)
{
  return rad * 180.0 / PI;
}

float Limb::degreesToRad(float degrees)
{
  return degrees * PI / 180.0;
}

float Limb::correctOffsetShaftToMotor(float shaftDegrees)
{
  return shaftDegrees - 180 + offset180 - offsetGearbox;
}

float Limb::correctOffsetMotorToShaft(float motorDegrees)
{
  return motorDegrees + 180 - offset180 + offsetGearbox;
  ;
}

void Limb::update()
{
  if (!motor.isOnline())
  {
    offsetGearbox = 0; // reset offset when power is off.
    state = STATE_OFF;
  }

  if (state == STATE_CALIBRATION)
  {
    tryCalibration();
  }
  motor.update();
}
