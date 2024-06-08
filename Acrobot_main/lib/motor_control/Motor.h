#ifndef MOTOR_H
#define MOTOR_H

#include "ACAN_ESP32.h"
#include "CANHandler.h"
#include "RemoteDebug.h"

struct MotorStatus
{
  float position;
  float velocity;
  float torque;
  uint8_t temperature;
  bool isOnline;
  uint8_t errorCode;
};

class Motor
{
public:
  Motor(uint16_t ID, CANHandler &canHandler, RemoteDebug &Debug);

  void start();
  void stop();
  void setPosition(float pos, float kp, float kd);
  void setVelocity(float vel, float kd);
  void setTorque(float torque);
  void reZero();

  void sendCommand(float p_des, float v_des, float kp, float kd, float t_ff);

  float getPosition() const;
  float getVelocity() const;
  float getTorque() const;
  uint8_t getTemperature() const;
  uint8_t getErrorCode() const;
  bool isOnline() const;

  const MotorStatus getStatus() const;

  void update();

private:
  uint16_t canID;
  float P_MIN = -40;
  float P_MAX = 40;
  float V_MIN = -50;
  float V_MAX = 50;
  float T_MIN = -25;
  float T_MAX = 25;
  float Kp_MIN = 0;
  float Kp_MAX = 500;
  float Kd_MIN = 0;
  float Kd_MAX = 5;

  float pOut = 0;
  float vOut = 0;
  float iOut = 0;
  uint8_t temperature = 0;
  uint8_t errorCode = 0;

  int floatToUInt(float x, float x_min, float x_max, unsigned int bits);
  float uintToFloat(int x_int, float x_min, float x_max, int bits);
  void packCommand(CANMessage &msg, float p_des, float v_des, float kp,
                   float kd, float t_ff);

  void unpackCommand(const CANMessage &msg);
  CANMessage latestFrame;
  CANMessage latestRecievedFrame;
  CANHandler &canHandler;
  uint32_t lastSendTime = 0;
  int sendInterval = 1; // ms
  RemoteDebug &Debug;
};

#endif
