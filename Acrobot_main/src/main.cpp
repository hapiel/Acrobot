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
#include "SD.h"
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug , using fork: https://github.com/karol-brejna-i/RemoteDebug
#include "WiFi.h"
#include "DNSServer.h"
#include "ESPmDNS.h"
#include "LiquidCrystal_I2C.h"
#include <ItemSubMenu.h>
#include "ItemToggle.h"
#include "ItemCommand.h"
#include "LcdMenu.h" // needs to be after item-imports!!!
#include "WebServer.h"
#define CSV_PARSER_DONT_IMPORT_SD
#include "CSV_Parser.h"

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
#include "Arm.h"
#include "StatusChecker.h"
#include "menu.h"
#include "ChoreoPlayer.h"
#include "JoystickControl.h"
#include "webserverFunctions.h"
#include "MovePlayer.h"
#include "Sequencer.h"

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

#define HOST_NAME "acrobot"
#define SCK 22
#define MISO 19
#define MOSI 23
#define CS 18

// external libraries
RemoteDebug Debug;                  // Debug levels: Verbose Debug Info Warning Error. Can't be named differently due to library macros?
LiquidCrystal_I2C lcd(0x27, 20, 4); // 20 wide 4 tall
LcdMenu lcdMenu(3, 20);

File file;
// no header so that each run is predictable. Otherwise the first line is skipped but only the first time we use this object.
CSV_Parser cp("ssffffffff", false);

// queue handling for tasks
QueueHandle_t functionQueue;

using TaskFunction = void (*)();

void executeTasksFromQueue()
{
  TaskFunction taskFunction;

  while (xQueueReceive(functionQueue, &taskFunction, 0))
  {
    taskFunction(); // Execute the task function
  }
}

// needed for csv_parser library
char feedRowParser()
{
  return file.read();
}
bool rowParserFinished()
{
  return ((file.available() > 0) ? false : true);
}
// need custom SPI class because of incorrect wiring
SPIClass spi = SPIClass(VSPI);

// custom libraries
Joystick joystick(Debug);
CANHandler canHandler;
Motor motorLegL(LEG_L_ID, canHandler, Debug);
Motor motorLegR(LEG_R_ID, canHandler, Debug);
Motor motorArmL(ARM_L_ID, canHandler, Debug);
Motor motorArmR(ARM_R_ID, canHandler, Debug);
HallSensor hallSensor(Wire, Debug);
DebugLed debugLed;
Leg legL(motorLegL, hallSensor, Debug, debugLed, LEG_L_ID, 32.75, true); // offset values
Leg legR(motorLegR, hallSensor, Debug, debugLed, LEG_R_ID, 0.99, false);
Arm armL(motorArmL, hallSensor, Debug, debugLed, ARM_L_ID, 28.64, true);
Arm armR(motorArmR, hallSensor, Debug, debugLed, ARM_R_ID, -3.25, false);

EStop eStop(ESTOP_PIN, Debug);
Buzzer buzzer(BUZZER_PIN, Debug);
Button buttonUp(BUTTON_UP, Debug);
Button buttonDown(BUTTON_DOWN, Debug);
Button buttonLeft(BUTTON_LEFT, Debug);
Button buttonRight(BUTTON_RIGHT, Debug);
BatterySensor batterySensor(BATTERY_SENSOR);
ChoreoPlayer choreoPlayer(Debug, legL, legR, armL, armR);
StatusChecker statusChecker(Debug, batterySensor, buzzer, debugLed, joystick, eStop);
Menu menu(lcdMenu, lcd, joystick, buttonUp, buttonDown, buttonLeft, buttonRight, legL, legR, armL, armR, buzzer, hallSensor, WiFi, eStop, batterySensor, Debug);
MovePlayer movePlayer(Debug, legL, legR, armL, armR, file, cp);
JoystickControl joystickControl(Debug, joystick, legL, legR, armL, armR, choreoPlayer, menu, eStop, movePlayer);
Sequencer sequencer(movePlayer, Debug);

// wifi
bool wifiConnected = false;

// MENU SECTION START
// ---------

// This section needs to be in the same file that inits the lcdMenu.

extern MenuItem *bootPage[];
extern MenuItem *movesPage[];
extern MenuItem *sequencerPage[];
extern MenuItem *moveTestPage[];
extern MenuItem *moveAcroPage[];
extern MenuItem *movePosePage[];
extern MenuItem *moveWarmupPage[];
extern MenuItem *moveGreetingPage[];
extern MenuItem *moveTravelPage[];
extern MenuItem *moveQuickPage[];
extern MenuItem *statusPage[];
extern MenuItem *motorPage[];
extern MenuItem *PIPage[];
extern MenuItem *controlPage[];
extern MenuItem *sequencePage[];
extern MenuItem *hardwarePage[];
extern MenuItem *adsPage[]; // in hardwarpage
extern MenuItem *aboutPage[];

MAIN_MENU(
    ITEM_SUBMENU("Boot motors", bootPage),
    ITEM_SUBMENU("Moves", movesPage),
    ITEM_SUBMENU("Sequencer", sequencerPage),
    ITEM_SUBMENU("Status", statusPage),
    ITEM_SUBMENU("Motors", motorPage),
    ITEM_SUBMENU("Change PI value", PIPage),
    ITEM_SUBMENU("Control mode", controlPage),
    ITEM_SUBMENU("Sequences old", sequencePage),
    ITEM_SUBMENU("Hardware", hardwarePage),
    ITEM_SUBMENU("About", aboutPage));

SUB_MENU(sequencerPage, mainMenu,
         ITEM_COMMAND("walk_test", []()
                      {
        TaskFunction lambdaFunction = []()
        { sequencer.startSequence("/routine_walk_test.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(movesPage, mainMenu,
         ITEM_SUBMENU("Poses", movePosePage),
         ITEM_SUBMENU("Acro", moveAcroPage),
         ITEM_SUBMENU("Travel", moveTravelPage),
         ITEM_SUBMENU("Warmup", moveWarmupPage),
         ITEM_SUBMENU("Greeting", moveGreetingPage),
         ITEM_SUBMENU("Quick numbers", moveQuickPage),
         ITEM_SUBMENU("MovePlayer TESTS", moveTestPage));

SUB_MENU(moveGreetingPage, movesPage,
         ITEM_COMMAND("wave_double", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/greeting_wave_double.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("wave_left", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/greeting_wave_left.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("nod", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/greeting_nod.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("bow_small", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/greeting_bow_small.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("bow_deep", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/greeting_bow_deep.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("shake_hand", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/greeting_shake_hand.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));



SUB_MENU(moveWarmupPage, movesPage,
         ITEM_COMMAND("arm_swing", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/warmup_arm_swing.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("leg_stretch", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/warmup_leg_stretch.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("shoulder_stretch", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/warmup_shoulder_stretch.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("situp", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/warmup_situp.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(moveAcroPage, movesPage,
         ITEM_COMMAND("acro_podcheska", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/acro_podcheska.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(moveTravelPage, movesPage,
         ITEM_COMMAND("walk_normal", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_normal.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("walk_large", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_large.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("walk_backwards", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_backwards.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("walk_forwards", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_forwards.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("walk_zombie", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_zombie.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("crawl_sit", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/crawl_sit.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("crawl_scorpion", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/crawl_scorpion.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(moveQuickPage, movesPage,
         ITEM_COMMAND("quick_01", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_01.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("quick_02", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_02.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("quick_03", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_03.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("quick_04", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_04.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("quick_05", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_05.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("quick_06", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_06.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("quick_07", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_07.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("quick_08", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_08.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("quick_09", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_09.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("quick_10", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/quick_10.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(movePosePage, movesPage,
         ITEM_COMMAND("stand", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_stand.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("stand_legs_only", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_stand_legs_only.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("180", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_180.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("sit", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_sit.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("handstand", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_handstand.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("seven", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_seven.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("handstand_split", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_handstand_split.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("handstd oversplit", []()
                      {
          TaskFunction lambdaFunction = []()
          { movePlayer.startMove("/pose_handstand_oversplit.csv"); };
          xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("planche", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_planche.csv", true, false, 30, 3, 30); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("scorpion", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_scorpion.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("yoga_dog", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_yoga_dog.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("table", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_table.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("split_l", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/pose_split_l.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(moveTestPage, movesPage,
         ITEM_COMMAND("walk repeat", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_normal.csv", false, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("walk begin only", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_normal.csv", true, true); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("w begin only slow", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_normal.csv", true, true, 5, 1, 10); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("walk non-repeat", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/walk_normal.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("test startallfront", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/test-start_front.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),

         ITEM_COMMAND("rightarm_only", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/TEST_rightarm_only.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("righta_posemixer", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/TEST_rightarm_posemixer.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("ra non zero start", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/TEST_rightarm_nonzerostart.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("ra late start", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/TEST_rightarm_latestart.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_COMMAND("ra weird curve", []()
                      {
        TaskFunction lambdaFunction = []()
        { movePlayer.startMove("/TEST_rightarm_weirdcurve.csv"); };
        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }));

SUB_MENU(bootPage, mainMenu,
         ITEM_COMMAND("CALLIBRATE", []()
                      {
                        TaskFunction lambdaFunction = []()
                        {
                          legR.startCalibration();
                          legL.startCalibration();
                          armL.startCalibration();
                          armR.startCalibration(); 
                          }; 
                        xQueueSend(functionQueue, &lambdaFunction, portMAX_DELAY); }),
         ITEM_BASIC(menu.bootAdc),   // adc
         ITEM_BASIC(menu.bootState), // or "ready ready ready"
         ITEM_BASIC(menu.bootPosA), ITEM_BASIC(menu.bootPosL), ITEM_BASIC(menu.bootRelais));

SUB_MENU(statusPage, mainMenu,
         ITEM_BASIC(menu.statusTemp),
         ITEM_BASIC(menu.statusWifi),
         ITEM_BASIC(menu.statusMem));

SUB_MENU(motorPage, mainMenu,
         ITEM_BASIC("ArL ArR LeL LeR"),
         ITEM_BASIC(menu.motorTemp),
         ITEM_BASIC(menu.motorAmp),
         ITEM_BASIC(menu.motorPosA),
         ITEM_BASIC(menu.motorTargA),
         ITEM_BASIC(menu.motorPosL),
         ITEM_BASIC(menu.motorTargL));

SUB_MENU(PIPage, mainMenu,
         ITEM_COMMAND(menu.PUpText, []()
                      { menu.PAdjust(2); }),
         ITEM_COMMAND("P DOWN step: (2)", []()
                      { menu.PAdjust(-2); }),
         ITEM_COMMAND(menu.IUpText, []()
                      { menu.DAdjust(0.5); }),
         ITEM_COMMAND("I DOWN step: (0.5)", []()
                      { menu.DAdjust(-0.5); }));

SUB_MENU(controlPage, mainMenu,
         ITEM_COMMAND("Summative", []()
                      { joystickControl.setMode(MODE_SUMMATIVE_90); }),
         ITEM_COMMAND("Telepresence Arm", []()
                      { joystickControl.setMode(MODE_TELEPRESENCE); }),
         ITEM_COMMAND("Pose", []()
                      { joystickControl.setMode(MODE_POSE); }));

SUB_MENU(sequencePage, mainMenu,
         ITEM_COMMAND("awakening", []()
                      { choreoPlayer.start(CHOREO_AWAKENING); }),
         ITEM_COMMAND("lets DELAY", []()
                      { choreoPlayer.start(CHOREO_LETS_DANCE_DELAY); }),
         ITEM_COMMAND("Stand", []()
                      { choreoPlayer.start(CHOREO_STANDING); }),
         ITEM_COMMAND("lets dance", []()
                      { choreoPlayer.start(CHOREO_LETS_DANCE0); }),
         ITEM_COMMAND("lets dance1", []()
                      { choreoPlayer.start(CHOREO_LETS_DANCE1); }),
         ITEM_COMMAND("lets dance2", []()
                      { choreoPlayer.start(CHOREO_LETS_DANCE2); }),
         ITEM_COMMAND("Arm_test", []()
                      { choreoPlayer.start(CHOREO_ARM_TEST); }),
         ITEM_COMMAND("walk cont", []()
                      { choreoPlayer.start(CHOREO_WALK_CONT); }),
         ITEM_COMMAND("Mila start", []()
                      { choreoPlayer.start(ACT_MILA); }),
         ITEM_COMMAND("Mila_music 0", []()
                      { choreoPlayer.start(MUSIC_SEQUENCE_0); }));

SUB_MENU(hardwarePage, mainMenu,
         ITEM_COMMAND("Set eStop", []()
                      { eStop.set(); }),
         ITEM_TOGGLE("LCD backlight", "off", "on", [](uint16_t isOff)
                     { menu.callbackBacklight(isOff); }), // enable again on button press
         ITEM_COMMAND("Buzzer beep", []()
                      { menu.callbackBeep(); }), // lambda because non static
         ITEM_SUBMENU("Show ADS", adsPage));

SUB_MENU(adsPage, hardwarePage,
         ITEM_BASIC(" - ADS values - "),
         ITEM_BASIC(menu.adsA),
         ITEM_BASIC(menu.adsL));

SUB_MENU(aboutPage, mainMenu,
         ITEM_BASIC("Acrobot v3 Jona"),
         ITEM_BASIC("By Daniel Simu"),
         ITEM_BASIC("::Supported by::"),
         ITEM_BASIC("V2_ Unstable"),
         ITEM_BASIC("Circusstad Festival"),
         ITEM_BASIC("Amarte"),
         ITEM_BASIC("Gemeente Rotterdam"),
         ITEM_BASIC("::Collaborators::"),
         ITEM_BASIC("Basti Kuhz"),
         ITEM_BASIC("Esmee v/d Ster"),
         ITEM_BASIC("Luc van Esch"),
         ITEM_BASIC("Mila Baumann"),
         ITEM_BASIC("Marit Adriaanse"),
         ITEM_BASIC("Edwin Dertien"),
         ITEM_BASIC("Ricard Lopez"),
         ITEM_BASIC("Bram Graafland"),
         ITEM_BASIC("::Sponsors::"),
         ITEM_BASIC("CubeMars"),
         ITEM_BASIC("Aluxprofile"),
         ITEM_BASIC("Fillamentum"),
         ITEM_BASIC("PCBWay"),
         ITEM_BASIC("MakerBeam"));

// ---------
// MENU SECTION END

// webserver

// webserver end

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

  debugI("Next init: SPI");
  SPI.begin(SCK, MISO, MOSI, CS);

  debugI("Next init: SD card");
  if (!SD.begin(CS))
  {
    debugE("SD card init FAILED!");
  }
  else
  {
    hasSD = true; // webserver
    debugI("SD card init done.");
  }

  debugI("Next init: canHandler");
  canHandler.setupCAN();

  debugI("Next init: eStop");
  eStop.init();

  debugI("Next init: Wire");
  Wire.begin(SDA_PIN, SCL_PIN);

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

  debugI("Next init: Webserver");
  // webserver
  server.on("/list", HTTP_GET, printDirectory);
  server.on("/edit", HTTP_DELETE, handleDelete);
  server.on("/edit", HTTP_PUT, handleCreate);
  server.on(
      "/edit", HTTP_POST, []()
      { returnOK(); },
      handleFileUpload);
  server.onNotFound(handleNotFound);

  server.begin();
  // webserver end

  debugI("Inits Done.");
  buzzer.buzz(50); // short buzz to indicate boot
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
      MDNS.addService("http", "tcp", 80);
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
  joystickControl.update();
  legL.update();
  legR.update();
  armL.update();
  armR.update();
  menu.update();
  statusChecker.update();
  choreoPlayer.update();
  sequencer.update();

  // webserver
  if (wifiConnected)
  {
    server.handleClient();
  }

  movePlayer.update();
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
    executeTasksFromQueue();
    Debug.handle(); // needs to be in bottom of loop
  }
}

void taskI2C(void *parameter)
{
  for (;;)
  {
    updatesI2C();
    vTaskDelay(5);
  }
}

void setup()
{
  inits();
  functionQueue = xQueueCreate(5, sizeof(TaskFunction));
  xTaskCreatePinnedToCore(taskMain, "taskMain", 100000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskI2C, "taskI2C", 20000, NULL, 1, NULL, 1);
}

void loop()
{
  vTaskDelete(NULL);
  taskMain(NULL);
  taskI2C(NULL);
}
