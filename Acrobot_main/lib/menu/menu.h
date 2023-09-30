#ifndef MENUCLASS_H
#define MENUCLASS_H

#include <LcdMenu.h>
#include <ItemSubMenu.h>
#include "Joystick.h"
#include "Button.h"
#include <LiquidCrystal_I2C.h>
#include "Buzzer.h"

// this is not a class because it crashes the lcdMenu library.

extern MenuItem* statusPage[];
extern MenuItem* motorPage[];
extern MenuItem* hardwarePage[];
extern MenuItem* adsPage[];
extern MenuItem* aboutPage[];

MAIN_MENU(
  ITEM_SUBMENU("Status", statusPage),
  ITEM_SUBMENU("Motors", motorPage),
  ITEM_SUBMENU("Hardware", hardwarePage),
  ITEM_SUBMENU("About", aboutPage)
);

SUB_MENU(statusPage, mainMenu,
  ITEM_BASIC("Temp: 00 00 XX 00 "), // XX = missing motor
  ITEM_BASIC("Wifi-Y BT-.       "),
  ITEM_BASIC("Mem %20 R: 0:00:00") // runtime
);

SUB_MENU(motorPage, mainMenu,
  ITEM_BASIC("Temp 00 00 00 00"),
  ITEM_BASIC("Amp  00 00 00 00"),
  ITEM_BASIC("Pos  00 00 00 00"),
  ITEM_BASIC("Targ 00 00 00 00"),
  ITEM_BASIC("   ArL ArR LeL LeR")
);

SUB_MENU(hardwarePage, mainMenu,
  ITEM_BASIC("Set eStop"),
  ITEM_BASIC("Toggle backlight"), // enable again on button press
  ITEM_BASIC("Buzzer beep"),
  ITEM_BASIC("Flash led"),
  ITEM_SUBMENU("Show ADS", adsPage)
);

SUB_MENU(adsPage, hardwarePage,
  ITEM_BASIC(" - ADS values - "),
  ITEM_BASIC("AL 00000 AR 00000"),
  ITEM_BASIC("LL 00000 LR 00000")
);

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
  ITEM_BASIC("Ricard Lopez")
);

LiquidCrystal_I2C lcd(0x27, 20, 4); // 20 wide 4 tall
LcdMenu menu(3, 20);



void menuInit(){
  lcd.init();
  lcd.backlight();
  menu.setupLcdWithMenu(0x27, mainMenu);
}

String lastMessage = "";
void printLastMessage(){
  lcd.setCursor(0, 3);
  lcd.print(lastMessage);
}

void menuInput(Button buttonUp, Button buttonDown, Button buttonLeft, Button buttonRight,Joystick &joystick){

  if (buttonUp.isPressed() || joystick.getDpadUpPressed()){
    menu.up();
    printLastMessage();
  }
  if (buttonDown.isPressed() || joystick.getDpadDownPressed()){
    menu.down();
    printLastMessage();
  }
  if (buttonLeft.isPressed() || joystick.getButtonCrossPressed()){
    menu.back();
    printLastMessage();
  }
  if (buttonRight.isPressed() || joystick.getButtonCirclePressed()){
    menu.enter();
    printLastMessage();
  }

  if (joystick.getDpadRightPressed()){
    menu.right();
    printLastMessage();
  }
  if (joystick.getDpadLeftPressed()){
    menu.left();
    printLastMessage();
  }

}


void lcdMessage(const String &message, Buzzer& buzzer){
  lastMessage = message;
  printLastMessage();
  buzzer.buzz(300);
}

void lcdBatteryValue(int value){
  lcd.setCursor(16, 3);

  char twoDigits[5]; // amount of characters in string + 1
  sprintf(twoDigits, "%04d", value);
  lcd.print(twoDigits);
}

void lcdBatteryAll(int _adc, float _voltage, int _percentage){
  

  lcd.setCursor(0, 3);
  lcd.print("A:");
  lcd.print(_adc);
  lcd.print(" V:");
  lcd.print(_voltage);
  lcd.print(" P:");
  lcd.print(_percentage);
}

#endif