#ifndef LEG_H
#define LEG_H

#include "Motor.h"
#include "HallSensor.h"

enum State {
    OFF,
    CALIBRATION,
    ON
};

const int STATE_OFF = 0;
const int STATE_CALIBRATION = 1;
const int STATE_ON = 2;

class Leg
{
public:
  Leg(Motor &motor, HallSensor &hallSensor, int motorID, float offset180, bool inverted);
  void setTarget(float posDegrees, float kp, float kd);
  void stop();
  void update();
  void startCalibration();

  float getTarget();
  float getPosition();
  float getVelocity();
  float getTorque();
  uint8_t getTemperature();
  uint8_t getErrorCode();
  State getState();

private:
  Motor &motor;
  HallSensor &hallSensor;
  int motorID;
  float kpLimit = 100; // safety feature, can be removed later
  float kdMinimum = 0.2; // safety feature, can be removed later
  float posMin = 90;
  float posMax = 270;
  float offset180;
  float target;
  bool inverted;
  State state = OFF;

  void start();
  void tryCalibration();

  float radToDegrees(float rad);
  float degreesToRad(float degrees);
  float correctOffsetShaftToMotor(float degrees);
  float correctOffsetMotorToShaft(float degrees);

  int16_t calibrationDegreesLow = 167; // minimum value just outside of calibration range. 167 for sleeveless, 170 for sleeved. 167 is probably fine for both.
  const int offsetDegrees = 36;
  int16_t offsetGearbox = 0;
};

#endif // LEG_H