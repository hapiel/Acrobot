#ifndef MENU_H
#define MENU_H

#include <ItemSubMenu.h>
#include "ItemCommand.h"
#include "ItemToggle.h"
#include <LcdMenu.h> // needs to be after item-imports!!!
#include <LiquidCrystal_I2C.h>

// access to all other classes
#include "Joystick.h"
#include "Button.h"
#include "Buzzer.h"
#include "Leg.h"
#include "Arm.h"
#include "Hallsensor.h"
#include "WiFi.h"
#include "EStop.h"
#include "BatterySensor.h"
#include "utils.h"

class Menu
{
public:
  Menu(LcdMenu &lcdMenu, LiquidCrystal_I2C &lcd, Joystick &joystick, Button &buttonUp, Button &buttonDown, Button &buttonLeft, Button &buttonRight, Leg &legL, Leg &legR, Arm &armL, Arm &armR, Buzzer &buzzer, HallSensor &hallSensor, WiFiClass &WiFi, EStop &eStop, BatterySensor &batterySensor, RemoteDebug &Debug);
  void init(MenuItem **mainMenu);
  void lcdMessage(const String &message);

  void callbackBeep();
  void callbackBacklight(uint16_t isOff);
  void PAdjust(float val);
  void DAdjust(float val);

  void pressUp();
  void pressDown();
  void pressLeft();
  void pressRight();

  float getP(); // for testing different P/D values that are settable
  float getD();

  void update();
  void updateI2C();

  // variable menu texts
  char bootAdc[19] = "A: 234 234 234 234";
  char bootState[19] = "S: off on cal xxx";   // or "ready ready ready
  char bootPosA[19] = "PA: 123 123 ";         // position arms
  char bootPosL[19] = "PL: 123 123 ";         // position legs
  char bootRelais[19] = "Relais: Set EXAMPL"; // or relais unset

  char statusTemp[19] = "Temp: 00 00 XX 00 "; // XX = missing motor
  char statusWifi[19] = "Wifi-. BT-.    J";
  char statusMem[19] = "Mem %20 R: 0:00:00"; // runtime

  char motorTemp[19] = "Tem 00 00 00 00";
  char motorAmp[19] = "Amp 00 00 00 00 ";
  char motorPosL[19] = "PL -0000.0 -0000.0";
  char motorTargL[19] = "TL -0000.0 -0000.0";
  char motorPosA[19] = "PA -0000.0 -0000.0";
  char motorTargA[19] = "TA -0000.0 -0000.0";

  char adsA[19] = "AL 00000 AR 00000";
  char adsL[19] = "LL 00000 LR 00000";

  char PUpText[19] = "P UP, val: X";
  char IUpText[19] = "I UP, val: X";

private:
  LiquidCrystal_I2C &lcd;
  LcdMenu &lcdMenu;
  Joystick &joystick;
  Button &buttonUp;
  Button &buttonDown;
  Button &buttonLeft;
  Button &buttonRight;
  Leg &legL;
  Leg &legR;
  Arm &armL;
  Arm &armR;
  Buzzer &buzzer;
  HallSensor &hallSensor;
  WiFiClass &WiFi;
  EStop &eStop;
  BatterySensor &batterySensor;
  RemoteDebug &Debug;

  float P = 16;
  float D = 2;

  bool upPressed = false;
  bool downPressed = false;
  bool leftPressed = false;
  bool rightPressed = false;
  bool backPressed = false;
  bool enterPressed = false;

  String lastMessage = "";

  void applyInput();
  void printLastMessage();
  void updateText();
  void lcdBatteryValue();
  void lcdBatteryAll();
};

#endif
