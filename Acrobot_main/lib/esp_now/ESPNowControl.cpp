#include "ESPNowControl.h"

static ESPNowControl *instance = nullptr; 

ESPNowControl::ESPNowControl(RemoteDebug &Debug)
    : Debug(Debug) {
    instance = this; 
}

void ESPNowControl::init(){
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

//   esp_now_register_send_cb(OnDataSent);
//   esp_now_register_recv_cb(OnDataRecv);
  
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
    if (Serial.available() > 0) {
        message = Serial.readStringUntil('\n');
        Serial.printf("You typed: %s\n", message);

        const char *msg = message.c_str();
        esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg, strlen(msg) + 1);

        if (result == ESP_OK) {
            Serial.println("Message sent successfully!");
        } else {
            Serial.println("Error sending the message.");
        }
    }
}


void ESPNowControl::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void ESPNowControl::OnDataRecv(const uint8_t *incomingData, int len) {
  char message[len + 1]; // Create a buffer to hold the received message
  memcpy(message, incomingData, len); // Copy data into the buffer
  message[len] = '\0'; // Null-terminate the string

  Serial.printf("Received message: %s\n", message);
}