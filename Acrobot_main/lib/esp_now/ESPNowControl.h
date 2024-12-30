#ifndef ESPNOW_CONTROL_H
#define ESPNOW_CONTROL_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include <esp_now.h>
#include <WiFi.h>

class ESPNowControl
{
public:
  ESPNowControl(RemoteDebug &Debug);
  void init();
  void update();


private:
  RemoteDebug &Debug; 
  // uint8_t receiverAddress[6] = {0xf4, 0x12, 0xfa, 0x7a, 0x4e, 0xf0}; // LilyGo dongle 
  uint8_t receiverAddress[6] = {0x90, 0x15, 0x06, 0xdb, 0x41, 0x7c}; // G dongle
  String message; 
  esp_now_peer_info_t peerInfo;

  // void OnDataRec(const uint8_t *incomingData, int len);
  void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void OnDataRecv(const uint8_t *incomingData, int len); 
};

#endif // ESPNOW_CONTROL_H
