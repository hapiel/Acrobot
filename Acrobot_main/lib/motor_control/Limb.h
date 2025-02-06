#ifndef LIMB_H
#define LIMB_H

#include "DebugLed.h"
#include "HallSensor.h"
#include "Motor.h"
#include "RemoteDebug.h"
#include "utilsAcrobot.h"

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

struct LimbStatus
{
  float target;
  float position;
  float velocity;
  float torque;
  uint8_t temperature;
  bool isCalibrating;
  bool isOnline;
  uint8_t errorCode;
};

class Limb
{
public:
  Limb(Motor &motor, HallSensor &hallSensor, RemoteDebug &Debug,
       DebugLed &debugLed, int motorID, float offset180, bool inverted);
  void setTarget(float posDegrees, float kp, float kd);
  void stop();
  void update();
  void startCalibration();

  // named unprotected to remind user that this function goes
  // straight to the motor. Use with caution.
  void setTorqueUnprotected(float torque);

  float getTarget() const;
  float getPosition() const;
  float getVelocity() const;
  float getTorque() const;
  uint8_t getTemperature() const;
  uint8_t getErrorCode() const;
  State getState() const;

  const LimbStatus getStatus() const;

protected:
  Motor &motor;
  HallSensor &hallSensor;
  RemoteDebug &Debug;
  DebugLed &debugLed;
  int motorID;
  float kpLimit = 150; // safety feature, can be increased later to 500
  float kPLimitStart =
      0.1; // during calibration this value is ramped up to end, then limit.
  float kPlimitRampEnd = 16;
  float kdMinimum = 0; // safety feature, used to be set to 0.2 
  float kDMinimumStart =
      5;                        // after starting this value is ramped down to kpMinimum
  int startRampDuration = 2000; // duration in ms to ramp kp and kd
  uint32_t startTime = 0;       // moment the motor starts. Used to ramp
  float posMin;                 // set in children
  float posMax;                 // set in children
  float offset180;
  float lastTarget;
  float lastKp;
  uint32_t lastSetTargetTime = 0;
  bool inverted;
  const int SAFE_TARGET_RANGE_MIN = 15;
  const int SAFE_TARGET_RANGE_MAX = 30;
  State state = STATE_OFF;
  LastControlMode lastControlMode = CONTROL_MODE_NONE;
  int revolutionCount = 0;
  const int revolutionMax = 5;

  float safeMoveSpeed = 180;  // degrees/s
  float safeKpIncrease = 10; // kp per second

  void start();
  void tryCalibration();

  float radToDegrees(float rad) const;
  float degreesToRad(float degrees) const;
  float correctOffsetShaftToMotor(float degrees) const;
  float correctOffsetMotorToShaft(float degrees) const;

  int16_t calibrationDegreesLow;
  const int offsetDegrees = 36;
  int16_t offsetGearbox = 0;
  int16_t calibrationTreshold;
};

#endif // LIMB_H
