#include "Motor.h"

Motor::Motor(uint16_t ID, RemoteDebug &Debug) : canID(ID), Debug(Debug)
{
  latestFrame.id = canID;
  latestFrame.ext = false;
  latestFrame.rtr = false;
  latestFrame.len = 8;
  packCommand(latestFrame, 0, 0, 0, 0, 0);
}

void Motor::start()
{

  CANMessage startFrame;
  startFrame.id = canID;
  startFrame.ext = false;
  startFrame.rtr = false;
  startFrame.len = 8;
  startFrame.data[0] = 0xFF;
  startFrame.data[1] = 0xFF;
  startFrame.data[2] = 0xFF;
  startFrame.data[3] = 0xFF;
  startFrame.data[4] = 0xFF;
  startFrame.data[5] = 0xFF;
  startFrame.data[6] = 0xFF;
  startFrame.data[7] = 0xFC;

  if (ACAN_ESP32::can.tryToSend(startFrame))
  {
    debugI("MOTOR: Started MIT mode on ID: %d", canID);
  }
}

void Motor::stop()
{

  CANMessage EndFrame;
  EndFrame.id = canID;
  EndFrame.ext = false;
  EndFrame.rtr = false;
  EndFrame.len = 8;
  EndFrame.data[0] = 0xFF;
  EndFrame.data[1] = 0xFF;
  EndFrame.data[2] = 0xFF;
  EndFrame.data[3] = 0xFF;
  EndFrame.data[4] = 0xFF;
  EndFrame.data[5] = 0xFF;
  EndFrame.data[6] = 0xFF;
  EndFrame.data[7] = 0xFD;

  if (ACAN_ESP32::can.tryToSend(EndFrame))
  {
    debugI("MOTOR: Motor with ID %d stopped", canID);
  }
}

void Motor::setPosition(float pos, float kp, float kd)
{
  sendCommand(pos, 0, kp, kd, 0);
}

void Motor::setVelocity(float vel, float kd)
{
  sendCommand(0, vel, 0, kd, 0);
}

void Motor::setTorque(float torque)
{
  sendCommand(0, 0, 0, 0, torque);
}

void Motor::sendCommand(float p_des, float v_des, float kp, float kd, float t_ff)
{

  packCommand(latestFrame, p_des, v_des, kp, kd, t_ff);
}

void Motor::reZero()
{

  CANMessage ResetFrame;
  ResetFrame.id = canID;
  ResetFrame.ext = false;
  ResetFrame.rtr = false;
  ResetFrame.len = 8;
  ResetFrame.data[0] = 0xFF;
  ResetFrame.data[1] = 0xFF;
  ResetFrame.data[2] = 0xFF;
  ResetFrame.data[3] = 0xFF;
  ResetFrame.data[4] = 0xFF;
  ResetFrame.data[5] = 0xFF;
  ResetFrame.data[6] = 0xFF;
  ResetFrame.data[7] = 0xFE;

  if (ACAN_ESP32::can.tryToSend(ResetFrame))
  {
    debugI("MOTOR: Motor with ID %d re-zeroed", canID);
  }
}

int Motor::floatToUInt(float x, float x_min, float x_max, unsigned int bits)
{
  float span = x_max - x_min;
  float offset = x_min;
  unsigned int result = 0;

  if (bits == 12)
  {
    result = static_cast<unsigned int>((x - offset) * 4095.0 / span);
  }
  else if (bits == 16)
  {
    result = static_cast<unsigned int>((x - offset) * 65535.0 / span);
  }

  return result;
}

float Motor::uintToFloat(int x_int, float x_min, float x_max, int bits)
{
  float span = x_max - x_min;
  float offset = x_min;
  float result = 0.0;

  if (bits == 12)
  {
    result = static_cast<float>(x_int) * span / 4095.0 + offset;
  }
  else if (bits == 16)
  {
    result = static_cast<float>(x_int) * span / 65535.0 + offset;
  }

  return result;
}

void Motor::packCommand(CANMessage &msg, float p_des, float v_des, float kp, float kd, float t_ff)
{
  // Limit data to be within bounds
  p_des = constrain(p_des, P_MIN, P_MAX);
  v_des = constrain(v_des, V_MIN, V_MAX);
  kp = constrain(kp, Kp_MIN, Kp_MAX);
  kd = constrain(kd, Kd_MIN, Kd_MAX);
  t_ff = constrain(t_ff, T_MIN, T_MAX);

  unsigned int p_int = floatToUInt(p_des, P_MIN, P_MAX, 16);
  unsigned int v_int = floatToUInt(v_des, V_MIN, V_MAX, 12);
  unsigned int kp_int = floatToUInt(kp, Kp_MIN, Kp_MAX, 12);
  unsigned int kd_int = floatToUInt(kd, Kd_MIN, Kd_MAX, 12);
  unsigned int t_int = floatToUInt(t_ff, T_MIN, T_MAX, 12);

  // Pack ints into the CAN message buffer
  msg.data[0] = p_int >> 8;                           // Position 8 higher
  msg.data[1] = p_int & 0xFF;                         // Position 8 lower
  msg.data[2] = v_int >> 4;                           // Speed 8 higher
  msg.data[3] = ((v_int & 0xF) << 4) | (kp_int >> 8); // Speed 4 bit lower KP 4bit higher
  msg.data[4] = kp_int & 0xFF;                        // KP 8 bit lower
  msg.data[5] = kd_int >> 4;                          // Kd 8 bit higher
  msg.data[6] = ((kd_int & 0xF) << 4) | (t_int >> 8); // KP 4 bit lower torque 4 bit higher
  msg.data[7] = t_int & 0xff;                         // torque 4 bit lower
}

float Motor::getPosition()
{
  return pOut;
}

float Motor::getVelocity()
{
  return vOut;
}

float Motor::getTorque()
{
  return iOut;
}

uint8_t Motor::getTemperature()
{
  return temperature;
}

uint8_t Motor::getErrorCode()
{
  return errorCode;
}

void Motor::unpackCommand(const CANMessage &msg)
{
  // Unpack ints from the CAN buffer
  unsigned int receivedID = msg.data[0];
  unsigned int positionInt = (msg.data[1] << 8) | msg.data[2];
  unsigned int velocityInt = (msg.data[3] << 4) | (msg.data[4] >> 4);
  unsigned int torqueInt = ((msg.data[4] & 0xF) << 8) | msg.data[5];
  uint8_t temperature = msg.data[6];
  uint8_t errorCode = msg.data[7];

  // Convert ints to floats and update member variables
  pOut = uintToFloat(positionInt, P_MIN, P_MAX, 16);
  vOut = uintToFloat(velocityInt, V_MIN, V_MAX, 12);
  iOut = uintToFloat(torqueInt, -T_MAX, T_MAX, 12);

  this->temperature = temperature - 40; // t-40 = celcius according to CubeMars discord
  this->errorCode = errorCode;          // needs to be interpreted?
}

void Motor::update()
{

  if (millis() - lastSendTime > sendInterval)
  {
    lastSendTime = millis();
    if (!ACAN_ESP32::can.tryToSend(latestFrame))
    {
      // debugW("MOTOR: CAN command failed, ID: %d", canID);
      // TODO: Warning message that does not overflow the debug 
    }
  }

  CANMessage receivedFrame;
  if (ACAN_ESP32::can.receive(receivedFrame))
  {
    if (receivedFrame.id == canID)
    {
      unpackCommand(receivedFrame);
    }
  }
}