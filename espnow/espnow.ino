#include <esp_now.h>
#include <WiFi.h>

// uint8_t receiverAddress[] = {0x90, 0x15, 0x06, 0xdb, 0x41, 0x7c}; // G dongle
uint8_t receiverAddress[] = {0x08, 0xB6, 0x1F, 0x3B, 0x65, 0x90}; // Robot
// uint8_t receiverAddress[] = {0xf4, 0x12, 0xfa, 0x7a, 0x4e, 0xf0}; // LilyGo dongle 

const char* wifiSsid = "Dmob";
const char* wifiPassword = "dataleech";

int counter = 0;
String message; 
bool canSend = true;
unsigned long lastSendTime = 0;
esp_now_peer_info_t peerInfo;

void OnSentCallBack(const uint8_t *mac_addr, esp_now_send_status_t status) {
  canSend = true;
}

void OnRecvCallBack(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  char message[len + 1]; // Create a buffer to hold the received message
  memcpy(message, incomingData, len); // Copy data into the buffer
  message[len] = '\0'; // Null-terminate the string

  Serial.print("Received message: ");
  Serial.println(message); 
}

void setup() {
  Serial.begin(115200); 
  WiFi.mode(WIFI_AP_STA);   // access point AND station (can connect and others can connect too)
  WiFi.begin(wifiSsid, wifiPassword);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Trying to connect...");
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnSentCallBack);
  // esp_now_register_recv_cb(OnRecvCallBack);

  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW R/S Initialized.");
}


void loop() {
  if (Serial.available() > 0) {
    message = Serial.readStringUntil('\n');

    if (canSend) {
      canSend = false;
      const char *msg = message.c_str();
      esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg, strlen(msg) + 1);

      lastSendTime = millis() - lastSendTime;
      if (result == ESP_OK) {
        Serial.printf("Succes, msg %s, delay %d\n", msg, lastSendTime);
      } else {
        Serial.printf("Error sending the msg %s, delay %d\n", msg, lastSendTime);
      }
      lastSendTime = millis();

    }
  }
}

