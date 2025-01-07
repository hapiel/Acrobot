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
  // check if received new data via ESP_NOW
  if (newData){
    receiveDelayTime = millis() - receiveDelayTime;
    getDataFromMsg(receivedMessage);
    const char *msg = receivedMessage.c_str();
    Serial.printf("Received msg: %s, delay: %d\n", msg, receiveDelayTime);
    receiveDelayTime = millis();

    newData = false;
  }

  // Send back ast values (ASAP)
  sendRobotInfoFast();

  // Send back low values (every 1s)
  // if (millis() - prevInfoUpdate > 1000) {
  //   sendRobotInfoSlow();
  // }
}

void ESPNowControl::sendRobotInfoFast(){
  canSend = false;
  legLPos = legL.getPosition();
  legRPos = legR.getPosition();
  armLPos = armL.getPosition();
  armRPos = armR.getPosition();
  legLTorque = legL.getTorque();
  legRTorque = legR.getTorque();
  armLTorque = armL.getTorque();
  armRTorque = armR.getTorque();

  char data[128];  // buffer
  snprintf(data, sizeof(data), "4xMP_4xMT %f %f %f %f %f %f %f %f",
         legLPos, legRPos, armLPos, armRPos, legLTorque, legRTorque, armLTorque, armRTorque);

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)data, strlen(data) + 1);
}

void ESPNowControl::sendRobotInfoSlow(){
  canSend = false;
  batteryPercentage = batterySensor.getPercentage();
  legLTemp = legL.getTemperature();
  legRTemp = legR.getTemperature();
  armLTemp = armL.getTemperature();
  armRTemp = armR.getTemperature();

  //  TODO: deze werken nog niet, hebben type State, hoe naar String? 
  // legLState = legL.getState();
  // legRState = legR.getState();
  // armLState = armL.getState();
  // armRState = armR.getState();


  char data[128];  // buffer
  snprintf(data, sizeof(data), "BP_4xMTMP %d %u %u %u %u",
         batteryPercentage,                  
         legLTemp, legRTemp, armLTemp, armRTemp);
        //  legLState.toString().c_str(),      
        //  legRState.toString().c_str(),
        //  armLState.toString().c_str(),
        //  armRState.toString().c_str());

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)data, strlen(data) + 1);
  prevInfoUpdate = millis();
}


void ESPNowControl::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  canSend = true;
}

void ESPNowControl::OnDataRecv(const uint8_t *incomingData, int len) {
  char incomingMessage[len + 1]; // buffer
  memcpy(incomingMessage, incomingData, len); 
  incomingMessage[len] = '\0'; // null-terminate string
  receivedMessage = incomingMessage;
  newData = true;
}


void ESPNowControl::getDataFromMsg(String infoMessage){

  if (infoMessage.startsWith("AL")) {   
    // Format --> AL:123.0 AR:123.0 LL:123.0 LR:123.0 
    armLTarget = infoMessage.substring(infoMessage.indexOf("AL:") + 3, infoMessage.indexOf(" AR")).toFloat();
    armRTarget = infoMessage.substring(infoMessage.indexOf("AR:") + 3, infoMessage.indexOf(" LL")).toFloat();
    legLTarget = infoMessage.substring(infoMessage.indexOf("LL:") + 3, infoMessage.indexOf(" LR")).toFloat();
    legRTarget = infoMessage.substring(infoMessage.indexOf("LR:") + 3).toFloat();
    // Serial.printf("AL: %.3f, AR: %.3f, LL: %.3f, LR: %.3f\n", armLTarget, armRTarget, legLTarget, legRTarget);

    } else if (infoMessage.startsWith("KP")){
      // Format --> KP:123.0 KD:5.0
      kP = infoMessage.substring(infoMessage.indexOf("KP:") + 3, infoMessage.indexOf(" KD")).toFloat();
      kD = infoMessage.substring(infoMessage.indexOf("KD:") + 3).toFloat();
      // Serial.printf("KP: %.3f, KD: %.3f\n", kP, kD);
    } else {
      Serial.printf("message in incorrect format: %s\n", infoMessage);
    }
  
  updateLimbs();
}


void ESPNowControl::updateLimbs(){
  legL.setTarget(legLTarget, kP, kD);
  legR.setTarget(legRTarget, kP, kD);
  armL.setTarget(armLTarget, kP, kD);
  armR.setTarget(armRTarget, kP, kD);
}