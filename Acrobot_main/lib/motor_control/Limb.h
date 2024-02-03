#ifndef LIMB_H
#define LIMB_H

#include "Motor.h"
#include "HallSensor.h"
#include "utilsAcrobot.h"
#include "DebugLed.h"
#include "RemoteDebug.h"

enum State
{
  STATE_OFF,
  STATE_CALIBRATION,
  STATE_ON
};

enum LastControlMode
{
  CONTROL_MODE_NONE,
  CONTROL_MODE_TARGET,
  CONTROL_MODE_VELOCITY,
  CONTROL_MODE_TORQUE
};

class Limb
{
public:
  Limb(Motor &motor, HallSensor &hallSensor, RemoteDebug &Debug,DebugLed &debugLed, int motorID, float offset180, bool inverted);
  void setTarget(float posDegrees, float kp, float kd);
  void stop();
  void update();
  void startCalibration();

  void setTorqueUnprotected(float torque); // named unprotected to remind user that this function goes straight to the motor. Use with caution.

  float getTarget();
  float getPosition();
  float getVelocity();
  float getTorque();
  uint8_t getTemperature();
  uint8_t getErrorCode();
  State getState();

protected:
  Motor &motor;
  HallSensor &hallSensor;
  RemoteDebug &Debug;
  DebugLed &debugLed;
  int motorID;
  float kpLimit = 100;      // safety feature, can be increased later to 500
  float kPLimitStart = 0.1; // during calibration this value is ramped up to end, then limit.
  float kPlimitRampEnd = 8;
  float kdMinimum = 0.2;        // safety feature, can be decreased later to 0
  float kDMinimumStart = 5;     // after starting this value is ramped down to kpMinimum
  int startRampDuration = 2000; // duration in ms to ramp kp and kd
  uint32_t startTime = 0;       // moment the motor starts. Used to ramp
  float posMin; // set in children
  float posMax; // set in children
  float offset180;
  float lastTarget;
  float lastKp;
  uint32_t lastSetTargetTime = 0;
  bool inverted;
  const int SAFE_TARGET_RANGE_MIN = 10;
  const int SAFE_TARGET_RANGE_MAX = 25;
  State state = STATE_OFF;
  LastControlMode lastControlMode = CONTROL_MODE_NONE;
  

  float safeMoveSpeed = 90; // degrees/s
  float safeKpIncrease = 10; // kp per second

  void start();
  void tryCalibration();

  float radToDegrees(float rad);
  float degreesToRad(float degrees);
  float correctOffsetShaftToMotor(float degrees);
  float correctOffsetMotorToShaft(float degrees);

  int16_t calibrationDegreesLow;
  const int offsetDegrees = 36;
  int16_t offsetGearbox = 0;
  int16_t calibrationTreshold;
};

#endif // LIMB_H