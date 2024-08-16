#ifndef BOTTANGO_SOCKET_H
#define BOTTANGO_SOCKET_H

#include <Arduino.h>
#include <WiFi.h>
#include "RemoteDebug.h"
#include "utilsAcrobot.h"
#include "Arm.h"
#include "Leg.h"
#include "menu.h"

struct BottangoMotorNames {
  const char* keyword;
  int index;
};

class BottangoSocket
{
public:
  BottangoSocket(RemoteDebug& Debug, Menu &menu, Arm &armL, Arm &armR, Leg &legL, Leg &legR);
  void update();
  void start();
  void stop();

private:
  WiFiClient client;
  RemoteDebug& Debug;
  Menu &menu;

  BottangoMotorNames nameToIndex[4] = {
  {"m_arm_l", 0},
  {"m_arm_r", 1},
  {"m_leg_l", 2},
  {"m_leg_r", 3}
};

  

  static const uint8_t NUM_LIMBS = 4;
  Limb *limbs[NUM_LIMBS]; // order: 0: armL, 1: armR, 2: legL, 3: legR

  const int BOTTANGO_MAX_VALUE = 8192;

  int maxDegrees[NUM_LIMBS];
  int minDegrees[NUM_LIMBS];

  bool targetsSet[NUM_LIMBS] = {false, false, false, false};

  float targets[NUM_LIMBS] = {0, 0, 0, 0};

  const char *host = "LAPTOP-VSACRSB1.local";
  const uint16_t port = 59225;

  bool connected = false;

};

#endif
