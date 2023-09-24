// external libraries
#include <Arduino.h>
#include "Wire.h"
#include "ADS1X15.h"
#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug , using fork: https://github.com/karol-brejna-i/RemoteDebug
#include <WiFi.h>
#include <DNSServer.h>
#include "ESPmDNS.h"

// custom libraries
#include "Joystick.h"
#include "CANHandler.h"
#include "Motor.h"
#include "EStop.h"
#include "Buzzer.h"
#include "Button.h"

#define ESTOP_PIN 5
#define SDA_PIN 22 // double check this on board
#define SCL_PIN 33
#define BUZZER_PIN 26
#define BUTTON_UP 34
#define BUTTON_DOWN 35
#define BUTTON_LEFT 39
#define BUTTON_RIGHT 32

#define HOST_NAME "Acrobot"

// external libraries
TwoWire wire(0);
ADS1115 ADS(0x48, &wire);
RemoteDebug Debug;

// custom libraries
Joystick joystick;
CANHandler canHandler;
Motor motor1(1);
Motor motor2(2);
EStop eStop(ESTOP_PIN); // global
Buzzer buzzer(BUZZER_PIN);

// wifi
const char *ssid = "ssid";
const char *password = "password";
bool wifiConnected = false;

void inits()
{
  Serial2.begin(115200); // Initialize UART2 for receiving data from joystick
  canHandler.setupCAN();
  eStop.init();
  wire.begin(SDA_PIN, SCL_PIN);
  ADS.begin(); // @Esmee, should we set the ADS gain? https://github.com/RobTillaart/ADS1X15#programmable-gain
  buzzer.init();
}

void initWifi()
{

  Serial.println("Initializing wifi");

  WiFi.begin(ssid, password);
  Serial.println("");

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

}

void initDebug()
{
  Debug.begin(HOST_NAME);         // Initialize the WiFi server
  Debug.setResetCmdEnabled(true); // Enable the reset command
  Debug.showProfiler(true);       // Profiler (Good to measure times, to optimize codes)
  Debug.showColors(true);         // Colors
  Debug.setSerialEnabled(true);   // All messages too send to serial too, and can be see in serial monitor
}

uint32_t mLastTime = 0;
uint32_t mTimeSeconds = 0;

void debugExample()
{
  if ((millis() - mLastTime) >= 1000)
  {

    // Time

    mLastTime = millis();

    mTimeSeconds++;

    // Blink the led

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    // Debug the time (verbose level)

    debugV("* Time: %u seconds (VERBOSE)", mTimeSeconds);

    if (mTimeSeconds % 5 == 0)
    { // Each 5 seconds

      // Debug levels

      debugV("* This is a message of debug level VERBOSE");
      debugD("* This is a message of debug level DEBUG");
      debugI("* This is a message of debug level INFO");
      debugW("* This is a message of debug level WARNING");
      debugE("* This is a message of debug level ERROR");

      // RemoteDebug handle


    }
  }
}

void updates()
{
  joystick.update(); // Update joystick and button states
  eStop.update();
  buzzer.update();
}

void setup()
{
  Serial.begin(115200); // Initialize Serial for USB communication
  initWifi();
  inits();
}

void loop()
{
  updates();

  if (!wifiConnected)
  {
    // Wait for connection
    if (WiFi.status() == WL_CONNECTED)
    {
      wifiConnected = true;

      if (MDNS.begin(HOST_NAME))
      {
        Serial.print("* MDNS responder started. Hostname -> ");
        Serial.println(HOST_NAME);
      }

      MDNS.addService("telnet", "tcp", 23);

      initDebug();
      Serial.println("");
      Serial.print("Connected to IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
  
  
  debugExample();





  // print timer
  static unsigned long previousPrintTime = 0;
  if (millis() - previousPrintTime > 2000)
  {
    previousPrintTime = millis(); // Update the last print time
    Serial.println("test");
  }

  Debug.handle(); // needs to be in loop
  // yield(); // may be required for the debug library??? 
}
