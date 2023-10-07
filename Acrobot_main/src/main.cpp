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
#include "SPI.h"
#include "SdFat.h"
#include "sdios.h"
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug , using fork: https://github.com/karol-brejna-i/RemoteDebug
#include "WiFi.h"
#include "DNSServer.h"
#include "ESPmDNS.h"
#include "LiquidCrystal_I2C.h"
#include <ItemSubMenu.h>
#include "ItemToggle.h"
#include "ItemCommand.h"
#include "LcdMenu.h" // needs to be after item-imports!!!

// custom libraries
#include "utilsAcrobot.h"
#include "Joystick.h"
#include "CANHandler.h"
#include "Motor.h"
#include "EStop.h"
#include "Buzzer.h"
#include "Button.h"
#include "BatterySensor.h"
#include "DebugLed.h"
#include "HallSensor.h"
#include "Leg.h"
#include "StatusChecker.h"
#include "menu.h"

// parameters
#include "wifiConfig.h" // needs to be made from wifiConfig_sample.h, in /include
#include "motorIDs.h"

#define ESTOP_PIN 5
#define SDA_PIN 25
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
RemoteDebug Debug; // Debug levels: Verbose Debug Info Warning Error. Can't be named differently due to library macros?
LiquidCrystal_I2C lcd(0x27, 20, 4); // 20 wide 4 tall


// custom libraries
Joystick joystick;
CANHandler canHandler;
Motor motorLegL(LEG_L_ID, canHandler, Debug);
Motor motorLegR(LEG_R_ID, canHandler, Debug);
HallSensor hallSensor(wire, Debug);
Leg legL(motorLegL, hallSensor, LEG_L_ID, 39.45, true); // offset values
Leg legR(motorLegR, hallSensor, LEG_R_ID, 4.79, false); 
EStop eStop(ESTOP_PIN, Debug);
Buzzer buzzer(BUZZER_PIN, Debug);
Button buttonUp(BUTTON_UP, Debug);
Button buttonDown(BUTTON_DOWN, Debug);
Button buttonLeft(BUTTON_LEFT, Debug);
Button buttonRight(BUTTON_RIGHT, Debug);
BatterySensor batterySensor(BATTERY_SENSOR);
DebugLed debugLed;
StatusChecker statusChecker(Debug, batterySensor, buzzer, debugLed, joystick, eStop);

// wifi
bool wifiConnected = false;

// MENU SECTION START
// ---------

// This section needs to be in the same file that inits the lcdMenu.

extern MenuItem *statusPage[];
extern MenuItem *motorPage[];
extern MenuItem *hardwarePage[];
extern MenuItem *adsPage[];
extern MenuItem *aboutPage[];
extern MenuItem *bootPage[];



LcdMenu lcdMenu(3, 20);
Menu menu(lcdMenu, lcd, joystick, buttonUp, buttonDown, buttonLeft, buttonRight, legL, legR, buzzer, hallSensor, WiFi, eStop, batterySensor, Debug);

MAIN_MENU(
    ITEM_SUBMENU("Boot motors", bootPage),
    ITEM_SUBMENU("Status", statusPage),
    ITEM_SUBMENU("Motors", motorPage),
    ITEM_SUBMENU("Hardware", hardwarePage),
    ITEM_SUBMENU("About", aboutPage));

SUB_MENU(bootPage, mainMenu,
         ITEM_BASIC(menu.bootAdc),   // adc
         ITEM_BASIC(menu.bootState), // or "ready ready ready"
         ITEM_BASIC(menu.bootPos),
         ITEM_BASIC(menu.bootRelais));

SUB_MENU(statusPage, mainMenu,
         ITEM_BASIC(menu.statusTemp), // XX = missing motor
         ITEM_BASIC(menu.statusWifi),
         ITEM_BASIC(menu.statusMem));

SUB_MENU(motorPage, mainMenu,
         ITEM_BASIC("   ArL ArR LeL LeR"),
         ITEM_BASIC(menu.motorTemp),
         ITEM_BASIC(menu.motorAmp),
         ITEM_BASIC(menu.motorPosL), // should later be swapped with arms
         ITEM_BASIC(menu.motorTargL),
         ITEM_BASIC(menu.motorPosA),
         ITEM_BASIC(menu.motorTargA));



SUB_MENU(hardwarePage, mainMenu,
         ITEM_BASIC("Set eStop"),
         ITEM_BASIC("Toggle backlight"), // enable again on button press
         ITEM_COMMAND("Buzzer beep",  []() { menu.callbackBeep(); }), // lambda because non static
         ITEM_BASIC("Flash led"),
         ITEM_SUBMENU("Show ADS", adsPage));



SUB_MENU(adsPage, hardwarePage,
         ITEM_BASIC(" - ADS values - "),
         ITEM_BASIC("AL 00000 AR 00000"),
         ITEM_BASIC("LL 00000 LR 00000"));

SUB_MENU(aboutPage, mainMenu,
         ITEM_BASIC("Acrobot v3 Jona"),
         ITEM_BASIC("By Daniel Simu"),
         ITEM_BASIC("::Supported by::"),
         ITEM_BASIC("V2_ Unstable"),
         ITEM_BASIC("Circusstad Festival"),
         ITEM_BASIC("Amarte"),
         ITEM_BASIC("::Collaborators::"),
         ITEM_BASIC("Basti Kuhz"),
         ITEM_BASIC("Esmee v/d Ster"),
         ITEM_BASIC("Mila Baumann"),
         ITEM_BASIC("Marit Adriaanse"),
         ITEM_BASIC("Ricard Lopez"));

// ---------
// MENU SECTION END

void initDebug()
{
  Debug.begin(HOST_NAME);         // Initialize the WiFi server
  Debug.setResetCmdEnabled(true); // Enable the reset command
  Debug.showProfiler(true);       // Profiler (Good to measure times, to optimize codes)
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

  debugI("Next init: ADS & HallSensor");
  hallSensor.init();

  debugI("Next init: Buzzer");
  buzzer.init();

  debugI("Next init: Buttons");
  buttonUp.init();
  buttonDown.init();
  buttonLeft.init();
  buttonRight.init();

  debugI("Next init: Menu");
  menu.init(mainMenu);

  debugI("Inits Done.");
}

void wifiConnection()
{
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
  canHandler.update();
  eStop.update();
  buzzer.update();
  buttonUp.update();
  buttonDown.update();
  buttonLeft.update();
  buttonRight.update();
  batterySensor.update();
  debugLed.update();
  legL.update();
  legR.update();
  menu.update();
  statusChecker.update();
}

void updatesI2C()
{
  menu.updateI2C();
  hallSensor.update();
}



void taskMain(void *parameter)
{
  for (;;)
  {

    updates();
    wifiConnection(); // restore wifi variables


    // control legs, temp system
    if (joystick.getButtonR1())
    {
      if (joystick.getMiscPSPressed())
      {
        legR.startCalibration();
      }
      float position = fMap(joystick.getAxisRYCorrected(), -128, 128, 90, 270);
      legR.setTarget(position, 8, 1);
    }

    if (joystick.getButtonL1())
    {
      if (joystick.getMiscPSPressed())
      {
        legL.startCalibration();
      }

      float legPos = fMap(joystick.getAxisLYCorrected(), -128, 128, 90, 270);
      legL.setTarget(legPos, 8, 1);
    }

    if (joystick.getButtonCrossPressed())
    {

      legR.stop();
      legL.stop();
    }

    if (joystick.getButtonSquarePressed())
    {

      legR.setTarget(0, 0, 0);
      legL.setTarget(0, 0, 0);
    }

    // debug messages
    static long executionTimer1 = 0;
    if (runEvery(1000, executionTimer1))
    {
      debugV("* Time: %u:%.2u:%.2u", (millis() / 3600000), (millis() / 60000) % 60, (millis() / 1000) % 60);

    }

    Debug.handle(); // needs to be in bottom of loop

    vTaskDelay(10);
  }
}

void taskI2C(void *parameter)
{
  for (;;)
  {

    updatesI2C();

    vTaskDelay(10);
  }
}

void setup()
{
  inits();
  xTaskCreatePinnedToCore(taskMain, "taskMain", 100000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskI2C, "taskI2C", 10000, NULL, 1, NULL, 1);
}

void loop()
{
  vTaskDelete(NULL);
}


