#ifndef ESPNOW_CONTROL_H
#define ESPNOW_CONTROL_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "RemoteDebug.h"
#include "Leg.h"
#include "Arm.h"
#include "ChoreoPlayer.h"
#include "menu.h"
#include "BatterySensor.h"

class ESPNowControl
{
public:
  ESPNowControl(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, BatterySensor &batterySensor);
  void init();
  void update();


private:
  RemoteDebug &Debug; 
  Leg &legL;
  Leg &legR;
  Arm &armL;
  Arm &armR;
  BatterySensor &batterySensor;
  
  uint8_t receiverAddress[6] = {0xf4, 0x12, 0xfa, 0x7a, 0x4e, 0xf0}; // LilyGo dongle 
  // uint8_t receiverAddress[6] = {0x90, 0x15, 0x06, 0xdb, 0x41, 0x7c}; // G dongle
  String message; 
  esp_now_peer_info_t peerInfo;
  int32_t delayedTime = 0;
  int32_t prevInfoUpdate = 0;
  bool canSend = true;

  float kP = 50;
  float kD = 2;
  float legLTarget = 180;
  float legRTarget = 180;
  float armLTarget = 180;
  float armRTarget = 180;

  // void OnDataRec(const uint8_t *incomingData, int len);
  void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void OnDataRecv(const uint8_t *incomingData, int len); 
  void getDataFromMsg(String message);
  // void updateLimbs(float legLTarget, float legRTarget, float armLTarget, float armRTarget, float kP, float kD);
  void updateLimbs();
  void sendRobotInfoSlow();
  void sendRobotInfoFast();
};

#endif // ESPNOW_CONTROL_H
