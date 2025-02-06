#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include <credentials.h>

#define NUM_LEDS 1
#define DATA_PIN 40 // Connected to IO40
#define CLOCK_PIN 39 // Connected to IO39

// Pick the correct receiver: 
// uint8_t receiverAddress[] = {0x90, 0x15, 0x06, 0xdb, 0x41, 0x7c}; // G dongle
uint8_t receiverAddress[] = {0x08, 0xB6, 0x1F, 0x3B, 0x65, 0x90}; // Robot
// uint8_t receiverAddress[] = {0xf4, 0x12, 0xfa, 0x7a, 0x4e, 0xf0}; // LilyGo dongle 
// uint8_t receiverAddress[] = {0x70, 0x04, 0x1d, 0xb5, 0x7a, 0xb4}; // LilyGo dongle 2



String message; 
String newMessage;
bool canSend = true;
bool newData = false;
int counter = 0;
int sendStatus = 1;
unsigned long lastSendTime = 0;
unsigned long lastWifiCheck = 0; 
esp_now_peer_info_t peerInfo;

CRGB leds[NUM_LEDS]; 

void OnSentCallBack(const uint8_t *mac_addr, esp_now_send_status_t status) {
  canSend = true;
  if (status != ESP_NOW_SEND_SUCCESS){
    sendStatus = 0;
  } else {
    sendStatus = 1;
  }
}

void OnRecvCallBack(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  // Create a buffer for the message
  char incomingMessage[len + 1];
  memcpy(incomingMessage, incomingData, len);
  incomingMessage[len] = '\0';

  newMessage = incomingMessage;
  newData = true;
}


void setup() {
  Serial.begin(115200); 
  Serial.println("ESP booting");
  delay(100);

  Serial.println("Initialising LED");
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  leds[0] = CRGB::Blue;
  FastLED.show();
  Serial.println("LED OK!");

  // WiFi stuff 
  // --> Needs to be connected to same network as receiver, otherwise cannot send data!!
  WiFi.mode(WIFI_AP_STA);   // access point AND station (can connect and others can connect too)
  WiFi.begin(wifiSsid, wifiPassword);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.printf("WIFI %i @init\n", 0);
    leds[0] = CRGB::Red;
    FastLED.show();
  }

  leds[0] = CRGB::Green;
  FastLED.show();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register SEND and RECEIVE callback functions
  esp_now_register_send_cb(OnSentCallBack);
  esp_now_register_recv_cb(OnRecvCallBack);

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

  // Receiving data via ESP-NOW 
  if (newData) {
    Serial.println(newMessage);
    newData = false;
  }

  // Sending data over ESP-Now
  // which has been read from the Serial input (= how input from Chataigne comes in)
  if (Serial.available() > 0) { 
    message = Serial.readStringUntil('\n');

    if (canSend) {
      canSend = false;
      const char *msg = message.c_str();
      esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg, strlen(msg) + 1);

      lastSendTime = millis() - lastSendTime;
      if (sendStatus == 0){
        Serial.printf("E %s, d %d\n", msg, lastSendTime);
      } else {
        Serial.printf("S %s, d %d\n", msg, lastSendTime);
      }

      lastSendTime = millis();
    }
  }

  // Reset canSend after 2 sec timeout
  if (millis() - lastSendTime > 2000 && !canSend) {
    Serial.println("Not receiving send callback for 2 sec.: check WiFi connection!");
    Serial.println("Resetting canSend..."); 
    canSend = true;
  }

  if (millis() - lastWifiCheck > 1000){
    if (WiFi.status() != WL_CONNECTED){
      while (WiFi.status() != WL_CONNECTED) {
        leds[0] = CRGB::Red;
        FastLED.show();
        Serial.printf("WIFI %i @init\n", 0);
        delay(1000);
      }
    }
    leds[0] = CRGB::Green;
    FastLED.show();
    lastWifiCheck = millis();
    Serial.printf("WIFI %i @init\n", 1);
  }

  // // EXAMPLE: If you want to test sending any data (e.g. a count every loop)
  // if (canSend) {
  //   canSend = false;
  //   String msg = String(counter); // make message into string
  //   esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msg.c_str(), msg.length() + 1);

  //   lastSendTime = millis() - lastSendTime;
  //   if (sendStatus == 0){
  //     Serial.printf("Error send msg %s, delay %d\n", msg, lastSendTime);
  //   } else {
  //     Serial.printf("Succes send msg %s, delay %d\n", msg, lastSendTime);
  //   }

  //   counter++;
  //   lastSendTime = millis();
  // }

  // delay(1);
}

