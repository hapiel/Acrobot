#include "ESPNowControl.h"

static ESPNowControl *instance = nullptr; 

ESPNowControl::ESPNowControl(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, BatterySensor &batterySensor)
    : Debug(Debug), legL(legL), legR(legR), armL(armL), armR(armR), batterySensor(batterySensor){
    instance = this; 
}

void ESPNowControl::init(){
  WiFi.mode(WIFI_AP_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status) {
      if (instance) {
          instance->OnDataSent(mac_addr, status);
      }
  });

  esp_now_register_recv_cb([](const uint8_t *mac, const uint8_t *incomingData, int len) {
      if (instance) {
          instance->OnDataRecv(incomingData, len);
      }
  });

  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW R/S Initialized.");
}

void ESPNowControl::update(){
  // if you want to send information, include it here and call send function, like so:
    // if (Serial.available() > 0) {
    //     message = Serial.readStringUntil('\n');
    //     Serial.printf("You typed: %s\n", message);

    //     const char *msg = message.c_str();
    //     esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg, strlen(msg) + 1);

    //     if (result == ESP_OK) {
    //         Serial.println("Message sent successfully!");
    //     } else {
    //         Serial.println("Error sending the message.");
    //     }
    // }


    // Fast values (ASAP)

    // Slow values (every 1s)
    // if (millis() - prevInfoUpdate > 1000) {
    //   sendRobotInfoSlow();
    // }
}


void ESPNowControl::sendRobotInfoSlow(){
  // BP = batterySensor.getpercentage();
}


void ESPNowControl::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  canSend = true;
}

void ESPNowControl::OnDataRecv(const uint8_t *incomingData, int len) {
  char message[len + 1]; // Create a buffer to hold the received message
  memcpy(message, incomingData, len); // Copy data into the buffer
  message[len] = '\0'; // Null-terminate the string

  getDataFromMsg(message);

  delayedTime = millis() - delayedTime;
  Serial.printf("Received message: %s, delay: %d\n", message, delayedTime);
  delayedTime = millis();
}


void ESPNowControl::getDataFromMsg(String message){

  if (message.startsWith("AL")) {   
    // Format --> AL:123.0 AR:123.0 LL:123.0 LR:123.0 
    armLTarget = message.substring(message.indexOf("AL:") + 3, message.indexOf(" AR")).toFloat();
    armRTarget = message.substring(message.indexOf("AR:") + 3, message.indexOf(" LL")).toFloat();
    legLTarget = message.substring(message.indexOf("LL:") + 3, message.indexOf(" LR")).toFloat();
    legRTarget = message.substring(message.indexOf("LR:") + 3).toFloat();
    // Serial.printf("AL: %.3f, AR: %.3f, LL: %.3f, LR: %.3f\n", armLTarget, armRTarget, legLTarget, legRTarget);

    } else if (message.startsWith("KP")){
      // Format --> KP:123.0 KD:5.0
      kP = message.substring(message.indexOf("KP:") + 3, message.indexOf(" KD")).toFloat();
      kD = message.substring(message.indexOf("KD:") + 3).toFloat();
      // Serial.printf("KP: %.3f, KD: %.3f\n", kP, kD);
    }
  
  updateLimbs();
}


void ESPNowControl::updateLimbs(){
  legL.setTarget(legLTarget, kP, kD);
  legR.setTarget(legRTarget, kP, kD);
  armL.setTarget(armLTarget, kP, kD);
  armR.setTarget(armRTarget, kP, kD);
}