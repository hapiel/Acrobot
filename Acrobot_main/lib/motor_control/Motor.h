#ifndef MOTOR_H
#define MOTOR_H

#include "ACAN_ESP32.h"
#include "RemoteDebug.h"
#include "CANHandler.h"

class Motor {
public:
  Motor(uint16_t ID, CANHandler &canHandler,RemoteDebug& Debug);

  void start();
  void stop();
  void setPosition(float pos, float kp, float kd);
  void setVelocity(float vel, float kd);
  void setTorque(float torque);
  void reZero();

  void sendCommand(float p_des, float v_des, float kp, float kd, float t_ff);

  float getPosition();
  float getVelocity();
  float getTorque();
  uint8_t getTemperature();
  uint8_t getErrorCode();

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

  float pOut;
  float vOut;
  float iOut;
  uint8_t temperature;
  uint8_t errorCode;

  int floatToUInt(float x, float x_min, float x_max, unsigned int bits);
  float uintToFloat(int x_int, float x_min, float x_max, int bits);
  void packCommand(CANMessage &msg, float p_des, float v_des, float kp, float kd, float t_ff);

  void unpackCommand(const CANMessage &msg);
  CANMessage latestFrame;
  CANMessage latestRecievedFrame;
  CANHandler &canHandler;
  uint32_t lastSendTime = 0;
  int sendInterval = 1; // ms
  RemoteDebug& Debug;
};

#endif
