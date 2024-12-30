/*
ACROBOT MAIN ESP
Daniel Simu - 2023
This is the code for the primary ESP32 of the Acrobot. It controls all of the
hardware except the RGB led and the bluetooth, which are controlled by the
secondary ESP. They communicate over UART.

The project can be debugged over serial, telnet, websocket, thanks to
RemoteDebug. Send '?' over any of these for commands.

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
#include "DNSServer.h"
#include "ESPmDNS.h"
#include "ItemCommand.h"
#include "ItemToggle.h"
#include "LcdMenu.h" // needs to be after item-imports!!!
#include "LiquidCrystal_I2C.h"
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug , using fork: https://github.com/karol-brejna-i/RemoteDebug
#include "SD.h"
#include "SPI.h"
#include "WebServer.h"
#include "WiFi.h"
#include "Wire.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ItemSubMenu.h>
#define CSV_PARSER_DONT_IMPORT_SD
#include "CSV_Parser.h"

// custom libraries
#include "Arm.h"
#include "BatterySensor.h"
#include "BottangoSocket.h"
#include "Button.h"
#include "Buzzer.h"
#include "CANHandler.h"
#include "ChoreoPlayer.h"
#include "Dashboard.h"
#include "DebugLed.h"
#include "EStop.h"
#include "HallSensor.h"
#include "Joystick.h"
#include "JoystickControl.h"
#include "Leg.h"
#include "Motor.h"
#include "MovePlayer.h"
#include "Sequencer.h"
#include "StatusChecker.h"
#include "menu.h"
#include "utilsAcrobot.h"
#include "webserverFunctions.h"
#include <functional>

// parameters
#include "motorIDs.h"
#include "wifiConfig.h" // needs to be made from wifiConfig_sample.h, in /include

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
RemoteDebug Debug;                  // Debug levels: Verbose Debug Info Warning Error. Can't be
                                    // named differently due to library macros?
LiquidCrystal_I2C lcd(0x27, 20, 4); // 20 wide 4 tall
LcdMenu lcdMenu(3, 20);

File file;
// no header so that each run is predictable. Otherwise the first line is
// skipped but only the first time we use this object.
CSV_Parser cp("ssffffffff", false);

// queue handling for tasks
QueueHandle_t functionQueue;
QueueHandle_t secondaryTaskQueue;

using Task = std::function<void()>;

void executeTasksFromQueue()
{
  Task task;

  while (xQueueReceive(functionQueue, &task, 0))
  {
    task(); // Execute the task function
  }
}

void executeTasksFromSecondaryQueue()
{
  Task task;

  while (xQueueReceive(secondaryTaskQueue, &task, 0))
  {
    task(); // Execute the task function
  }
}

void sendToSecondaryTaskQueue(Task task)
{
  xQueueSend(secondaryTaskQueue, &task, portMAX_DELAY);
};

// needed for csv_parser library
char feedRowParser() { return file.read(); }
bool rowParserFinished() { return ((file.available() > 0) ? false : true); }
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
Leg legL(motorLegL, hallSensor, Debug, debugLed, LEG_L_ID, 32.75,
         true); // offset values
Leg legR(motorLegR, hallSensor, Debug, debugLed, LEG_R_ID, 0.99, false);
Arm armL(motorArmL, hallSensor, Debug, debugLed, ARM_L_ID, 28.64, true, 11100);
Arm armR(motorArmR, hallSensor, Debug, debugLed, ARM_R_ID, -3.25, false, 10750);
Dashboard dashboard;
EStop eStop(ESTOP_PIN, Debug);
Buzzer buzzer(BUZZER_PIN, Debug);
Button buttonUp(BUTTON_UP, Debug);
Button buttonDown(BUTTON_DOWN, Debug);
Button buttonLeft(BUTTON_LEFT, Debug);
Button buttonRight(BUTTON_RIGHT, Debug);
BatterySensor batterySensor(BATTERY_SENSOR);
ChoreoPlayer choreoPlayer(Debug, legL, legR, armL, armR);
StatusChecker statusChecker(Debug, batterySensor, buzzer, debugLed, joystick,
                            eStop);
Menu menu(lcdMenu, lcd, joystick, buttonUp, buttonDown, buttonLeft, buttonRight,
          legL, legR, armL, armR, buzzer, hallSensor, WiFi, eStop,
          batterySensor, Debug);
MovePlayer movePlayer(Debug, legL, legR, armL, armR, file, cp, choreoPlayer);
Sequencer sequencer(movePlayer, Debug);
BottangoSocket bottangoSocket(Debug, menu, armL, armR, legL, legR);
JoystickControl joystickControl(Debug, joystick, legL, legR, armL, armR,
                                choreoPlayer, menu, eStop, movePlayer,
                                sequencer, bottangoSocket);

// wifi
bool wifiConnected = false;

// MENU SECTION START
// ---------

/* #region Menu */

// This section needs to be in the same file that inits the lcdMenu.

extern MenuItem *bootPage[];
extern MenuItem *arsPage[];
extern MenuItem *circusstadPage[];
extern MenuItem *kelderfestPage[];
extern MenuItem *agtPage[];
extern MenuItem *fgtPage[];
extern MenuItem *movesPage[];
extern MenuItem *sequencerPage[];
extern MenuItem *bottangoPage[];
extern MenuItem *moveTestPage[];
extern MenuItem *moveAcroPage[];
extern MenuItem *movePosePage[];
extern MenuItem *moveWarmupPage[];
extern MenuItem *moveGreetingPage[];
extern MenuItem *moveTravelPage[];
extern MenuItem *moveQuickPage[];
extern MenuItem *moveQuick50Page[];
extern MenuItem *moveQuickPowPage[];
extern MenuItem *statusPage[];
extern MenuItem *motorPage[];
extern MenuItem *PIPage[];
extern MenuItem *controlPage[];
extern MenuItem *sequencePage[];
extern MenuItem *hardwarePage[];
extern MenuItem *adsPage[]; // in hardwarepage
extern MenuItem *aboutPage[];
extern MenuItem *moveQuickRepeatPage[];
extern MenuItem *moveQuick50RepeatPage[];

MAIN_MENU(ITEM_SUBMENU("Boot motors", bootPage), 
          ITEM_SUBMENU("AGT", agtPage), 
          ITEM_SUBMENU("DST", fgtPage),
          ITEM_SUBMENU("ARS", arsPage),
          ITEM_SUBMENU("Circusstad", circusstadPage),
          ITEM_SUBMENU("Antwerpen", kelderfestPage),
          ITEM_SUBMENU("Moves", movesPage),
          ITEM_SUBMENU("Sequencer", sequencerPage),
          ITEM_SUBMENU("Bottango Socket", bottangoPage),
          ITEM_SUBMENU("Status", statusPage), 
          ITEM_SUBMENU("Motors", motorPage),
          ITEM_SUBMENU("Change PI value", PIPage),
          ITEM_SUBMENU("Control mode", controlPage),
          ITEM_SUBMENU("Sequences old", sequencePage),
          ITEM_SUBMENU("Hardware", hardwarePage),
          ITEM_SUBMENU("About", aboutPage));

SUB_MENU(
    arsPage, mainMenu,
       ITEM_COMMAND("seq 2",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/ars/ars_sequence_2.csv", false,
                                          true, 40);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("seq 1",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/ars/ars_sequence_1.csv", false,
                                          true, 40);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 })

);


SUB_MENU(
    circusstadPage, mainMenu,
    ITEM_COMMAND("sit",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/circusstad_spiegel_acroyoga.csv",
                                          true, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    // act_circusstad.csv
    ITEM_COMMAND("spiegel-yoga",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/circusstad_spiegel_acroyoga.csv",
                                          false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("mannequin",
                 []()
                 { joystickControl.setMode(MODE_MANNEQUIN); }),

    ITEM_COMMAND("microphone",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/circusstad_microphone.csv", false,
                                          false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("mannequin 2",
                 []()
                 { joystickControl.setMode(MODE_MANNEQUIN); }),

    ITEM_COMMAND("stand",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/circusstad_acro.csv", true, false,
                                          50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("lets dance",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/circusstad_acro.csv", false, false,
                                          50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("I feel empty",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/circusstad_ending_alive.csv", false,
                                          false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 })

);

SUB_MENU(kelderfestPage, mainMenu,
         ITEM_COMMAND("stand",
                      []()
                      {
                        Task task = []()
                        {
                          movePlayer.startMove("/pose_stand.csv");
                        };
                        xQueueSend(functionQueue, &task,
                                   portMAX_DELAY);
                      }),
         // act_jamileh.csv
         ITEM_COMMAND("act Jamileh", []()
                      {
           Task task = []() {
             movePlayer.startMove("/act_jamileh.csv", false, false, 50);
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(agtPage, mainMenu,
         ITEM_COMMAND("stand",
                      []()
                      {
                        Task task = []()
                        {
                          movePlayer.startMove("/pose_stand.csv");
                        };
                        xQueueSend(functionQueue, &task,
                                   portMAX_DELAY);
                      }),
         // act_moveyourfeet.csv
         ITEM_COMMAND("act move feet", []()
                      {
           Task task = []() {
             movePlayer.startMove("/act_moveyourfeet.csv", false, false, 50);
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(fgtPage, mainMenu,
         ITEM_COMMAND("stand",
                      []()
                      {
                        Task task = []()
                        {
                          movePlayer.startMove("/pose_stand.csv");
                        };
                        xQueueSend(functionQueue, &task,
                                   portMAX_DELAY);
                      }),
         // act_moveyourfeet.csv
         ITEM_COMMAND("act Cabeza",
                      []()
                      {
                        Task task = []()
                        {
                          movePlayer.startMove("/tango-sequence-dst.csv", false,
                                               false, 50);
                        };
                        xQueueSend(functionQueue, &task,
                                   portMAX_DELAY);
                      }),
         ITEM_COMMAND("walk_normal", []()
                      {
           Task task = []() {
             movePlayer.startMove("/walk_normal.csv", false, true);
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(sequencerPage, mainMenu, ITEM_COMMAND("walk_test", []()
                                               {
           Task task = []() {
             sequencer.startSequence("/routine_walk_test.csv");
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(movesPage, mainMenu, ITEM_SUBMENU("Poses", movePosePage),
         ITEM_SUBMENU("Acro", moveAcroPage),
         ITEM_SUBMENU("Travel", moveTravelPage),
         ITEM_SUBMENU("Warmup", moveWarmupPage),
         ITEM_SUBMENU("Greeting", moveGreetingPage),
         ITEM_SUBMENU("Quick P20 nums", moveQuickPage),
         ITEM_SUBMENU("Quick P50 nums", moveQuick50Page),
         ITEM_SUBMENU("Quick P20 rep", moveQuickRepeatPage),
         ITEM_SUBMENU("Quick P50 rep", moveQuick50RepeatPage),
         ITEM_SUBMENU("Quick POW", moveQuickPowPage),
         ITEM_SUBMENU("MovePlayer TESTS", moveTestPage));

SUB_MENU(
    moveGreetingPage, movesPage,
    ITEM_COMMAND("wave_double",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/greeting_wave_double.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("wave_left",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/greeting_wave_left.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("nod",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/greeting_nod.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("bow_small",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/greeting_bow_small.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("bow_deep",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/greeting_bow_deep.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("shake_hand", []()
                 {
      Task task = []() {
        movePlayer.startMove("/greeting_shake_hand.csv");
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    moveWarmupPage, movesPage,
    ITEM_COMMAND("arm_swing",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/warmup_arm_swing.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("leg_stretch",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/warmup_leg_stretch.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("shoulder_stretch",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/warmup_shoulder_stretch.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("situp", []()
                 {
      Task task = []() {
        movePlayer.startMove("/warmup_situp.csv");
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(moveAcroPage, movesPage, 
        
        ITEM_COMMAND("forehead balance", []()
                                               {
           Task task = []() {
             movePlayer.startMove("/iso-forhead-balance.csv", false, false, 50);
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }),
           ITEM_COMMAND("f2h catch rockroll", []()
                                               {
           Task task = []() {
             movePlayer.startMove("/iso-f2h-catch-rockroll-seq.csv", false, false, 50);
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }),
           ITEM_COMMAND("forehead balance", []()
                                               {
           Task task = []() {
             movePlayer.startMove("/iso-lowh2h-standonshoulder.csv", false, false, 50);
           };
           xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    moveTravelPage, movesPage,
    ITEM_COMMAND("walk_normal",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_normal.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("walk_large",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_large.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("walk_backwards",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_backwards.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("walk_forwards",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_forwards.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("walk_zombie",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_zombie.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("crawl_sit",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/crawl_sit.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("crawl_scorpion", []()
                 {
      Task task = []() {
        movePlayer.startMove("/crawl_scorpion.csv", false, true);
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    moveQuick50Page, movesPage,
    ITEM_COMMAND("quick P50 01",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_01.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 02",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_02.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 03",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_03.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 04",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_04.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 05",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_05.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 06",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_06.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 07",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_07.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 08",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_08.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 09",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_09.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 10",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_10.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 11",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_11.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 12",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_12.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 13",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_13.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 14",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_14.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 15",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_15.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 16",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_16.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 17",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_17.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 18",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_18.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 19",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_19.csv", false, false, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 20", []()
                 {
      Task task = []() {
        movePlayer.startMove("/quick_20.csv", false, false, 50);
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    moveQuickPage, movesPage,
    ITEM_COMMAND("quick_01",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_01.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_02",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_02.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_03",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_03.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_04",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_04.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_05",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_05.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_06",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_06.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_07",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_07.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_08",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_08.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_09",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_09.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_10",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_10.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_11",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_11.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_12",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_12.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_13",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_13.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_14",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_14.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_15",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_15.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_16",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_16.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_17",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_17.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_18",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_18.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_19",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_19.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_20",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_20.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 })

);

SUB_MENU(
    moveQuick50RepeatPage, movesPage,
    ITEM_COMMAND("quick P50 01",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_01.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 02",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_02.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 03",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_03.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 04",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_04.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 05",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_05.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 06",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_06.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P50 07",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_07.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 08",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_08.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 09",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_09.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 10",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_10.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 11",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_11.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 12",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_12.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 13",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_13.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 14",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_14.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 15",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_15.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 16",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_16.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 17",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_17.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 18",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_18.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 19",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_19.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick P50 20",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_20.csv", false, true, 50);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 })

);

SUB_MENU(
    moveQuickRepeatPage, movesPage,
    ITEM_COMMAND("quick_01",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_01.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_02",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_02.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_03",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_03.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_04",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_04.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_05",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_05.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_06",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_06.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick_07",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_07.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_08",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_08.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_09",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_09.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_10",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_10.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_11",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_11.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_12",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_12.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_13",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_13.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_14",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_14.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_15",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_15.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_16",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_16.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_17",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_17.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_18",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_18.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_19",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_19.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("quick_20", []()
                 {
      Task task = []() {
        movePlayer.startMove("/quick_20.csv", false, true);
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    moveQuickPowPage, movesPage,
    ITEM_COMMAND("quick P 10",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 10);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 20",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 20);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 40",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 40);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 60",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 60);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 80",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 80);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 100",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 100);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 120",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/quick_pow.csv", false, false, 120);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("quick P 150", []()
                 {
      Task task = []() {
        movePlayer.startMove("/quick_pow.csv", false, false, 150);
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    movePosePage, movesPage,
    ITEM_COMMAND("stand",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_stand.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("stand_legs_only",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_stand_legs_only.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("180",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_180.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("sit",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_sit.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("handstand",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_handstand.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("seven",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_seven.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("handstand_split",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_handstand_split.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("handstd oversplit",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_handstand_oversplit.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("planche",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_planche.csv", true, false, 30,
                                          3, 30);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("scorpion",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_scorpion.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("yoga_dog",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_yoga_dog.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("table",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/pose_table.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("split_l", []()
                 {
      Task task = []() {
        movePlayer.startMove("/pose_split_l.csv");
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(
    moveTestPage, movesPage,
    ITEM_COMMAND("walk repeat",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_normal.csv", false, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("walk begin only",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_normal.csv", true, true);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("w begin only slow",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_normal.csv", true, true, 5, 1,
                                          10);
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("walk non-repeat",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/walk_normal.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("test startallfront",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/test-start_front.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),

    ITEM_COMMAND("rightarm_only",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/TEST_rightarm_only.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("righta_posemixer",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/TEST_rightarm_posemixer.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("ra non zero start",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/TEST_rightarm_nonzerostart.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("ra late start",
                 []()
                 {
                   Task task = []()
                   {
                     movePlayer.startMove("/TEST_rightarm_latestart.csv");
                   };
                   xQueueSend(functionQueue, &task, portMAX_DELAY);
                 }),
    ITEM_COMMAND("ra weird curve", []()
                 {
      Task task = []() {
        movePlayer.startMove("/TEST_rightarm_weirdcurve.csv");
      };
      xQueueSend(functionQueue, &task, portMAX_DELAY); }));

SUB_MENU(bootPage, mainMenu,
         ITEM_COMMAND("CALLIBRATE",
                      []()
                      {
                        Task task = []()
                        {
                          legR.startCalibration();
                          legL.startCalibration();
                          armL.startCalibration();
                          armR.startCalibration();
                        };
                        xQueueSend(functionQueue, &task,
                                   portMAX_DELAY);
                      }),
         ITEM_BASIC(menu.bootAdc),   // adc
         ITEM_BASIC(menu.bootState), // or "ready ready ready"
         ITEM_BASIC(menu.bootPosA), ITEM_BASIC(menu.bootPosL),
         ITEM_BASIC(menu.bootRelais));

SUB_MENU(bottangoPage, mainMenu,
         ITEM_COMMAND("start", []()
                      { bottangoSocket.start(); }),
         ITEM_COMMAND("stop", []()
                      { bottangoSocket.stop(); }));

SUB_MENU(statusPage, mainMenu, ITEM_BASIC(menu.statusTemp),
         ITEM_BASIC(menu.statusWifi), ITEM_BASIC(menu.statusMem));

SUB_MENU(motorPage, mainMenu, ITEM_BASIC("ArL ArR LeL LeR"),
         ITEM_BASIC(menu.motorTemp), ITEM_BASIC(menu.motorAmp),
         ITEM_BASIC(menu.motorPosA), ITEM_BASIC(menu.motorTargA),
         ITEM_BASIC(menu.motorPosL), ITEM_BASIC(menu.motorTargL));

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
         ITEM_COMMAND("Summative",
                      []()
                      { joystickControl.setMode(MODE_SUMMATIVE_90); }),
         ITEM_COMMAND("Manequin",
                      []()
                      { joystickControl.setMode(MODE_MANNEQUIN); }),
         ITEM_COMMAND("Telepresence Arm",
                      []()
                      { joystickControl.setMode(MODE_TELEPRESENCE); }));

SUB_MENU(
    sequencePage, mainMenu,
    ITEM_COMMAND("awakening", []()
                 { choreoPlayer.start(CHOREO_AWAKENING); }),
    ITEM_COMMAND("lets DELAY",
                 []()
                 { choreoPlayer.start(CHOREO_LETS_DANCE_DELAY); }),
    ITEM_COMMAND("Stand", []()
                 { choreoPlayer.start(CHOREO_STANDING); }),
    ITEM_COMMAND("lets dance",
                 []()
                 { choreoPlayer.start(CHOREO_LETS_DANCE0); }),
    ITEM_COMMAND("lets dance1",
                 []()
                 { choreoPlayer.start(CHOREO_LETS_DANCE1); }),
    ITEM_COMMAND("lets dance2",
                 []()
                 { choreoPlayer.start(CHOREO_LETS_DANCE2); }),
    ITEM_COMMAND("Arm_test", []()
                 { choreoPlayer.start(CHOREO_ARM_TEST); }),
    ITEM_COMMAND("walk cont", []()
                 { choreoPlayer.start(CHOREO_WALK_CONT); }),
    ITEM_COMMAND("Mila start", []()
                 { choreoPlayer.start(ACT_MILA); }),
    ITEM_COMMAND("Mila_music 0",
                 []()
                 { choreoPlayer.start(MUSIC_SEQUENCE_0); }));

SUB_MENU(hardwarePage, mainMenu,
         ITEM_COMMAND("Set eStop", []()
                      { eStop.set(); }),
         ITEM_TOGGLE("LCD backlight", "off", "on",
                     [](uint16_t isOff)
                     {
                       menu.callbackBacklight(isOff);
                     }), // enable again on button press
         ITEM_COMMAND("Buzzer beep",
                      []()
                      {
                        menu.callbackBeep();
                      }), // lambda because non static
         ITEM_COMMAND("Rumble long", []()
                      { joystick.setRumble(255, 255); }),
         ITEM_COMMAND("Rumble soft", []()
                      { joystick.setRumble(5, 255); }),
         ITEM_COMMAND("Rumble short", []()
                      { joystick.setRumble(255, 5); }),
         ITEM_SUBMENU("Show ADS", adsPage));

SUB_MENU(adsPage, hardwarePage, ITEM_BASIC(" - ADS values - "),
         ITEM_BASIC(menu.adsA), ITEM_BASIC(menu.adsL));

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
         ITEM_BASIC("Ricard Mallafre"),
         ITEM_BASIC("Bram Graafland"),
         ITEM_BASIC("::Sponsors::"),
         ITEM_BASIC("CubeMars"),
         ITEM_BASIC("Aluxprofile"),
         ITEM_BASIC("Fillamentum"),
         ITEM_BASIC("PCBWay"),
         ITEM_BASIC("MakerBeam"));

/* #endregion */

// ---------
// MENU SECTION END

void initDebug()
{
  Debug.begin(HOST_NAME);         // Initialize the WiFi server
  Debug.setResetCmdEnabled(true); // Enable the reset command
  Debug.showProfiler(
      true);                    // Profiler (Good to measure times, to optimize codes)
  Debug.setSerialEnabled(true); // All messages too send to serial too, and can
                                // be see in serial monitor
}

void inits()
{

  Serial.println("Next init: wifi");
  // WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

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

  debugI("Next init: Dashboard instance");
  dashboard.setLeftArm(armL)
      .setRightArm(armR)
      .setLeftLeg(legL)
      .setRightLeg(legR)
      .setBatterySensor(batterySensor);

  // webserver
  debugI("Next init: Webserver");
  ws.onEvent(onEvent);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
  async_server.addHandler(&ws);
  async_server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/plain", "pong"); });
  async_server.on("/sd", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                    Task task = [request]() {
    onSdRequest(request);
  };
  xQueueSend(secondaryTaskQueue, &task, portMAX_DELAY); });

  async_server.onNotFound(notFound);

  async_server.begin();

  server.enableCORS(true);

  server.on("/play", HTTP_POST, []()
            {
    DynamicJsonDocument options(1024);
    DeserializationError error = deserializeJson(options, server.arg("plain"));

    if (error) {
      Serial.println("Failed to parse JSON");
      server.send(400, "text/plain", "Invalid JSON");
      return;
    }

    if (!options.containsKey("mode")) {
      server.send(400, "text/plain", "missing mode\r\n");
      return;
    }

    if (!options.containsKey("file")) {
      server.send(400, "text/plain", "missing file\r\n");
      return;
    }

    if (!options.containsKey("power")) {
      server.send(400, "text/plain", "missing power\r\n");
      return;
    }

    struct TaskParams {
      String file;
      float power;
      bool beginPosOnly;
      bool repeat;
    };

    TaskParams *params = new TaskParams{options["file"], options["power"],
                                        options["mode"] == "beginPosOnly",
                                        options["mode"] == "repeat"};

    Task task = [params]() {
      movePlayer.startMove(params->file.c_str(), params->beginPosOnly,
                           params->repeat, params->power);
      delete params;
    };

    xQueueSend(functionQueue, &task, portMAX_DELAY);
    server.send(204); });

  server.on("/file", HTTP_GET, []()
            {
    if (!server.hasArg("path")) {
      server.send(400, "text/plain", "missing path\r\n");
      return;
    }
    String path = server.arg("path");
    bool exists = loadFromSdCard(path);
    if (!exists) {
      server.send(404, "text/plain", "file not found\r\n");
    } });

  server.on(
      "/stop", HTTP_POST, []()
      { returnOK(); },
      []()
      {
        Task task = []()
        { movePlayer.stop(); };
        xQueueSend(functionQueue, &task, portMAX_DELAY);
      });

  server.on("/list", HTTP_GET, printDirectory);
  server.on("/edit", HTTP_DELETE, handleDelete);
  server.on("/edit", HTTP_PUT, handleCreate);
  server.on("/edit", HTTP_POST, []()
            { returnOK(); }, handleFileUpload);
  server.on("/ping", HTTP_GET, []()
            { server.send(200, "text/plain", "pong"); });

  server.onNotFound(handleNotFound);

  server.begin();

  // webserver end

  debugI("inits done!");
  buzzer.buzz(50); // short buzz to indicate boot
}

void wifiConnection()
{

  if (WiFi.status() == WL_CONNECTED && !wifiConnected)
  {
    wifiConnected = true;
    if (MDNS.begin(HOST_NAME))
    {
      debugI("MDNS responder started. Hostname -> %s or %s.local", HOST_NAME, HOST_NAME);
      MDNS.addService("telnet", "tcp", 23);
      MDNS.addService("http", "tcp", 80);
      MDNS.addService("ws", "tcp", 3000);
      debugI("Connected to IP address: %s ", WiFi.localIP().toString().c_str());
    }
    else
    {
      debugE("Error setting up MDNS responder!");
    }
  }
  else if (WiFi.status() != WL_CONNECTED && wifiConnected)
  {
    wifiConnected = false;
    debugI("WiFi connection lost");
  }
}

unsigned long lastWsSend = 0;
int ws_message_count = 0;

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
  bottangoSocket.update();
  movePlayer.update();

  // webserver
  if (wifiConnected)
  {
    server.handleClient();
  }
  // webserver end
}

void updatesI2C()
{
  menu.updateI2C();
  hallSensor.update();
}

void updatesWebsocket()
{
  // Send a message through the WebSocket every second
  if (millis() - lastWsSend > 1000)
  {
    lastWsSend = millis();
    StaticJsonDocument<600> doc = dashboard.getRobotStatusJson();
    String status;
    serializeJson(doc, status);
    ws.textAll(status);
  }
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

void taskSecondary(void *parameter)
{
  for (;;)
  {
    updatesI2C();
    updatesWebsocket();
    executeTasksFromSecondaryQueue();
    vTaskDelay(5);
  }
}

void setup()
{
  Serial.begin(115200); // Initialize Serial for USB communication
  Serial.println("setting up");
  inits();
  Serial.println("inits done");
  functionQueue = xQueueCreate(5, sizeof(Task));
  secondaryTaskQueue = xQueueCreate(5, sizeof(Task));

  BaseType_t taskMainCreated = xTaskCreatePinnedToCore(taskMain, "taskMain", 8192, NULL, 1, NULL, 1);
  BaseType_t taskI2CCreated = xTaskCreatePinnedToCore(taskSecondary, "taskI2C", 20000, NULL, 1, NULL, 1);

  if (taskMainCreated == pdPASS)
  {
    Serial.println("taskMain created successfully");
  }
  else
  {
    Serial.printf("Failed to create taskMain. Error code: %d\n", taskMainCreated);
  }

  if (taskI2CCreated == pdPASS)
  {
    Serial.println("taskI2C created successfully");
  }
  else
  {
    Serial.printf("Failed to create taskI2C. Error code: %d\n", taskI2CCreated);
  }

  Serial.println("setup done");
}

void loop()
{
  vTaskDelete(NULL);
}
