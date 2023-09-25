/*
ACROBOT MAIN ESP
Daniel Simu - 2023
This is the code for the primary ESP32 of the Acrobot. It controls all of the hardware except the RGB led and the bluetooth, which are controlled by the secondary ESP. They communicate over UART.

The project can be debugged over serial, telnet, websocket, thanks to RemoteDebug. Send '?' over any of these for commands.

Hardware list: 
- Motors over CAN bus
- Hall sensors through ADC over I2C
- SD card reader
- Buzzer
- Emergency stop latch (eStop)
- IMU over I2C
- Battery sensor
- LCD display
- Wifi
- 4 buttons
(currently not connected:)
- ledstrip
- IBUS

The project should be built in platformio
*/

// external libraries
#include <Arduino.h>
#include "Wire.h"
#include "ADS1X15.h"
#include "SPI.h"
#include "SdFat.h"
#include "sdios.h"
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug , using fork: https://github.com/karol-brejna-i/RemoteDebug
#include "WiFi.h"
#include "DNSServer.h"
#include "ESPmDNS.h"
#include <LiquidCrystal_I2C.h>

// custom libraries
#include "Joystick.h"
#include "CANHandler.h"
#include "Motor.h"
#include "EStop.h"
#include "Buzzer.h"
#include "Button.h"
#include "BatterySensor.h"

#include "config.h" // needs to be made from config_sample.h, in /include

#define ESTOP_PIN 5
#define SDA_PIN 22 // double check this on board
#define SCL_PIN 33
#define BUZZER_PIN 26
#define BUTTON_UP 34
#define BUTTON_DOWN 35
#define BUTTON_LEFT 39
#define BUTTON_RIGHT 32
#define BATTERY_SENSOR 36

#define HOST_NAME "Acrobot"

// external libraries
TwoWire wire(0);
ADS1115 ADS(0x48, &wire);
RemoteDebug Debug; // Debug levels: Verbose Debug Info Warning Error

// custom libraries
Joystick joystick;
CANHandler canHandler;
Motor motor1(1);
Motor motor2(2);
EStop eStop(ESTOP_PIN, Debug); 
Buzzer buzzer(BUZZER_PIN, Debug);
Button buttonUp(BUTTON_UP, Debug);
Button buttonDown(BUTTON_DOWN, Debug);
Button buttonLeft(BUTTON_LEFT, Debug);
Button buttonRight(BUTTON_RIGHT, Debug);
BatterySensor batterySensor(BATTERY_SENSOR);
LiquidCrystal_I2C lcd(0x27, 20, 4); // 20 wide 4


// wifi
bool wifiConnected = false;

void initDebug()
{
  Debug.begin(HOST_NAME);         // Initialize the WiFi server
  Debug.setResetCmdEnabled(true); // Enable the reset command
  Debug.showProfiler(true);       // Profiler (Good to measure times, to optimize codes)
  Debug.showColors(true);         // Colors
  Debug.setSerialEnabled(true);   // All messages too send to serial too, and can be see in serial monitor
}

void inits()
{
  Serial.begin(115200); // Initialize Serial for USB communication

  Serial.println("Next init: wifi");
  WiFi.begin(wifiSsid, wifiPassword); // from config.h

  Serial.println("Next init: Debug");
  initDebug(); // AFTER WIFI!

  debugI("Next init: Serial2 with joystick");
  Serial2.begin(115200); // Initialize UART2 for receiving data from joystick

  debugI("Next init: Battery sensor");
  batterySensor.init();

  debugI("Next init: canHandler");
  canHandler.setupCAN();

  debugI("Next init: eStop");
  eStop.init();

  debugI("Next init: Wire");
  wire.begin(SDA_PIN, SCL_PIN);

  debugI("Next init: ADS");
  ADS.begin(); // @Esmee, should we set the ADS gain? https://github.com/RobTillaart/ADS1X15#programmable-gain

  debugI("Next init: Buzzer");
  buzzer.init();

  debugI("Next init: Buttons");
  buttonUp.init();
  buttonDown.init();
  buttonLeft.init();
  buttonRight.init();

  debugI("Inits Done.");
}

void wifiConnection(){
  if (!wifiConnected)
  {
    // Wait for connection
    if (WiFi.status() == WL_CONNECTED)
    {
      if (MDNS.begin(HOST_NAME))
      {
        debugI("MDNS responder started. Hostname -> %s or %s.local", HOST_NAME, HOST_NAME);

      }
      MDNS.addService("telnet", "tcp", 23);
      debugI("Connected to IP address: %s ", WiFi.localIP().toString().c_str());

    }
  }
  wifiConnected = WiFi.status() == WL_CONNECTED;
}

void updates()
{
  joystick.update(); // Update joystick and button states
  eStop.update();
  buzzer.update();
  buttonUp.update();
  buttonDown.update();
  buttonLeft.update();
  buttonRight.update();
  batterySensor.update();
}

// for testing & sending periodical messages
bool runEvery(int interval, long &nextExecutionMillis){
  long currentMillis = millis();
  if (nextExecutionMillis - currentMillis >= 0) {
    return false;
  }
  nextExecutionMillis = ((currentMillis / interval) + 1) * interval;
  return true;
}

void setup()
{
  inits();
}

void loop()
{
  updates();
  wifiConnection(); // restore wifi variables

  static long executionTimer2 = 0;
  if (runEvery(1000, executionTimer2)){
    debugI("Battery level: %u", batterySensor.getPercentage());
  }

  // debug messages
  static long executionTimer1 = 0;
  if (runEvery(1000, executionTimer1)){
    debugV("* Time: %u:%.2u:%.2u", (millis() / 3600000) , (millis() / 60000) % 60, (millis() / 1000) % 60);
    
    if(!wifiConnected){
      debugW("Wifi not connected");
    }  
  } 
  Debug.handle(); // needs to be in loop
}
