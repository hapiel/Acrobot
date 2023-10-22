#include "Menu.h"


Menu::Menu(LcdMenu &lcdMenu, LiquidCrystal_I2C &lcd, Joystick &joystick, Button &buttonUp, Button &buttonDown, Button &buttonLeft, Button &buttonRight, Leg &legL, Leg &legR, Buzzer &buzzer, HallSensor &hallSensor, WiFiClass &WiFi, EStop &eStop, BatterySensor &batterySensor, RemoteDebug &Debug) : lcdMenu(lcdMenu), lcd(lcd), joystick(joystick), buttonUp(buttonUp), buttonDown(buttonDown), buttonLeft(buttonLeft), buttonRight(buttonRight), legL(legL), legR(legR), buzzer(buzzer), hallSensor(hallSensor), WiFi(WiFi), eStop(eStop), batterySensor(batterySensor), Debug(Debug)
{
}

void Menu::callbackBeep()
{
  buzzer.buzz(500);
  debugD("Beep");
}

void Menu::callbackBacklight(uint16_t  isOff)
{
  // receives inverse value on isOn
  if (!isOff){
    lcd.backlight();
  } else {
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
  if (buttonUp.isPressed() || joystick.getDpadUpPressed())
  {
    upPressed = true;
  }
  if (buttonDown.isPressed() || joystick.getDpadDownPressed())
  {
    downPressed = true;
  }
  if (buttonLeft.isPressed() || joystick.getDpadLeftPressed())
  {
    backPressed = true;
  }
  if (buttonRight.isPressed() || joystick.getDpadRightPressed())
  {
    rightPressed = true;
    enterPressed = true;
  }
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

void Menu::updateText(){
    sprintf(bootAdc, "A: %03d %03d %03d %03d", hallSensor.getValFromID(ARM_L_ID) / 100, hallSensor.getValFromID(ARM_R_ID) / 100, hallSensor.getValFromID(LEG_L_ID) / 100, hallSensor.getValFromID(LEG_R_ID) / 100);

  sprintf(bootState, "S: %3s %3s xxx xxx", 
    legL.getState() == STATE_OFF ? "OFF" : legL.getState() == STATE_CALIBRATION ? "CAL" : "ON",
    legR.getState() == STATE_OFF ? "OFF" : legR.getState() == STATE_CALIBRATION ? "CAL" : "ON");

  sprintf(bootPos, "P: %.2f %.2f", legL.getPosition(), legR.getPosition());

  sprintf(statusTemp, "Temp: 00 00 %0d %0d", legL.getTemperature(), legR.getTemperature());

  sprintf(statusWifi, "Wifi-%s BT-%s J:%d%%", WiFi.status() == WL_CONNECTED ? "Y" : ".", joystick.getConnected() ? "Y" : ".", joystick.getBatteryPercentage());

  sprintf(statusMem, "Mem: %2d%% R-%u:%.2u:%.2u", (int)(100 * (1 - ((float)ESP.getFreeHeap() / (float)ESP.getHeapSize()))), (millis() / 3600000), (millis() / 60000) % 60, (millis() / 1000) % 60);

  sprintf(motorTemp, "Tem %02d %02d %02d %02d", 0, 0, legL.getTemperature(), legR.getTemperature());

  sprintf(motorAmp, "Amp %02d %02d %02d %02d", 0, 0, (int)legL.getTorque(), (int)legR.getTorque());

  sprintf(motorPosL, "PL %06.1f %06.1f", legL.getPosition(), legR.getPosition());

  sprintf(motorTargL, "TL %06.1f %06.1f", legL.getTarget(), legR.getTarget());

  sprintf(PUpText, "P UP, val: %.1f", P);
  sprintf(DUpText, "D UP, val: %.1f", D);
}


void Menu::PUp(){
  P += 1;
}

void Menu::PDown(){
  P -= 1;
}
void Menu::DUp(){
  D += 0.2; 
}

void Menu::DDown(){
  D -= 0.2; 
}

float Menu::getP(){
  return P;
}

float Menu::getD(){
  return D;
}