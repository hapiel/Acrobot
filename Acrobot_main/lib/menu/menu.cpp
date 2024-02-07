#include "Menu.h"

Menu::Menu(LcdMenu &lcdMenu, LiquidCrystal_I2C &lcd, Joystick &joystick, Button &buttonUp, Button &buttonDown, Button &buttonLeft, Button &buttonRight, Leg &legL, Leg &legR, Arm &armL, Arm &armR, Buzzer &buzzer, HallSensor &hallSensor, WiFiClass &WiFi, EStop &eStop, BatterySensor &batterySensor, RemoteDebug &Debug) : lcdMenu(lcdMenu), lcd(lcd), joystick(joystick), buttonUp(buttonUp), buttonDown(buttonDown), buttonLeft(buttonLeft), buttonRight(buttonRight), legL(legL), legR(legR), armL(armL), armR(armR), buzzer(buzzer), hallSensor(hallSensor), WiFi(WiFi), eStop(eStop), batterySensor(batterySensor), Debug(Debug)
{
}

void Menu::callbackBeep()
{
  buzzer.buzz(500);
  debugD("Beep");
}

void Menu::callbackBacklight(uint16_t isOff)
{
  // receives inverse value on isOn
  if (!isOff)
  {
    lcd.backlight();
  }
  else
  {
    lcd.noBacklight();
  }

  lcdMenu.setBacklight(!isOff);
}

void Menu::init(MenuItem **mainMenu)
{
  lcd.init();
  lcd.backlight();
  lcdMenu.setupLcdWithMenu(0x27, mainMenu);
}

void Menu::update()
{
  // update all buttons
  if (buttonUp.isPressed())
  {
    upPressed = true;
  }
  if (buttonDown.isPressed())
  {
    downPressed = true;
  }
  if (buttonLeft.isPressed())
  {
    leftPressed = true;
    backPressed = true;
  }
  if (buttonRight.isPressed())
  {
    rightPressed = true;
    enterPressed = true;
  }
}

void Menu::pressUp()
{
  upPressed = true;
}

void Menu::pressDown()
{
  downPressed = true;
}

void Menu::pressLeft()
{
  leftPressed = true;
  backPressed = true;
}

void Menu::pressRight()
{
  rightPressed = true;
  enterPressed = true;
}

void Menu::applyInput()
{
  if (upPressed)
  {
    upPressed = false;
    lcdMenu.up();
    printLastMessage();
  }
  if (downPressed)
  {
    downPressed = false;
    lcdMenu.down();
    printLastMessage();
  }
  if (leftPressed)
  {
    leftPressed = false;
    lcdMenu.left();
    printLastMessage();
  }
  if (rightPressed)
  {
    rightPressed = false;
    lcdMenu.right();
    printLastMessage();
  }
  if (backPressed)
  {
    backPressed = false;
    lcdMenu.back();
    printLastMessage();
  }
  if (enterPressed)
  {
    enterPressed = false;
    lcdMenu.enter();
    printLastMessage();
  }
}

void Menu::lcdMessage(const String &message)
{
  lastMessage = message;
  printLastMessage();
  buzzer.buzz(300);
}

void Menu::lcdBatteryValue()
{
  lcd.setCursor(18, 3);

  char twoDigits[3]; // amount of characters in string + 1
  sprintf(twoDigits, "%02d", batterySensor.getPercentage());
  lcd.print(twoDigits);
}

void Menu::lcdBatteryAll()
{
  lcd.setCursor(0, 3);
  lcd.print("A:");
  lcd.print(batterySensor.getAdc());
  lcd.print(" V:");
  lcd.print(batterySensor.getVoltage());
  lcd.print(" P:");
  lcd.print(batterySensor.getPercentage());
}

void Menu::printLastMessage()
{
  lcd.setCursor(0, 3);
  lcd.print(lastMessage);
}

void Menu::updateI2C()
{
  applyInput();

  static long executionTimer = 0;
  if (runEvery(200, executionTimer))
  {
    updateText();
    lcdBatteryValue();
    lcdMenu.drawMenuNoClear();
  }
}

void Menu::updateText()
{
  sprintf(bootAdc, "A: %03d %03d %03d %03d", hallSensor.getValFromID(ARM_L_ID) / 100, hallSensor.getValFromID(ARM_R_ID) / 100, hallSensor.getValFromID(LEG_L_ID) / 100, hallSensor.getValFromID(LEG_R_ID) / 100);

  sprintf(bootState, "S: %3s %3s %3s %3s",
          armL.getState() == STATE_OFF ? "OFF" : armL.getState() == STATE_CALIBRATION ? "CAL"
                                                                                      : "ON",
          armR.getState() == STATE_OFF ? "OFF" : armR.getState() == STATE_CALIBRATION ? "CAL"
                                                                                      : "ON",
          legL.getState() == STATE_OFF ? "OFF" : legL.getState() == STATE_CALIBRATION ? "CAL"
                                                                                      : "ON",
          legR.getState() == STATE_OFF ? "OFF" : legR.getState() == STATE_CALIBRATION ? "CAL"
                                                                                      : "ON");

  sprintf(bootPosA, "PA: %.2f %.2f", armL.getPosition(), armR.getPosition());

  sprintf(bootPosL, "PL: %.2f %.2f", legL.getPosition(), legR.getPosition());

  sprintf(statusTemp, "Temp: 00 00 %0d %0d", legL.getTemperature(), legR.getTemperature());

  sprintf(statusWifi, "Wifi-%s BT-%s J:%d%%", WiFi.status() == WL_CONNECTED ? "Y" : ".", joystick.getConnected() ? "Y" : ".", joystick.getBatteryPercentage());

  sprintf(statusMem, "Mem: %2d%% R-%u:%.2u:%.2u", (int)(100 * (1 - ((float)ESP.getFreeHeap() / (float)ESP.getHeapSize()))), (millis() / 3600000), (millis() / 60000) % 60, (millis() / 1000) % 60);

  sprintf(motorTemp, "Tem %02d %02d %02d %02d", armL.getTemperature(), armR.getTemperature(), legL.getTemperature(), legR.getTemperature());

  sprintf(motorAmp, "Amp %02d %02d %02d %02d", (int)armL.getTorque(), (int)armR.getTorque(), (int)legL.getTorque(), (int)legR.getTorque());

  sprintf(motorPosA, "PA %06.1f %06.1f", armL.getPosition(), armR.getPosition());

  sprintf(motorTargA, "TA %06.1f %06.1f", armL.getTarget(), armR.getTarget());

  sprintf(motorPosL, "PL %06.1f %06.1f", legL.getPosition(), legR.getPosition());

  sprintf(motorTargL, "TL %06.1f %06.1f", legL.getTarget(), legR.getTarget());

  sprintf(adsA, "AL %05d AR %05d", hallSensor.getValFromID(ARM_L_ID), hallSensor.getValFromID(ARM_R_ID));
  sprintf(adsL, "LL %05d LR %05d", hallSensor.getValFromID(LEG_L_ID), hallSensor.getValFromID(LEG_R_ID));

  sprintf(PUpText, "P UP, val: %.1f", P);
  sprintf(IUpText, "D UP, val: %.1f", D);
}

void Menu::PAdjust(float val)
{
  P += val;
}

void Menu::DAdjust(float val)
{
  D += val;
}

float Menu::getP()
{
  return P;
}

float Menu::getD()
{
  return D;
}