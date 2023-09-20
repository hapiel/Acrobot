#include <Arduino.h>
#include "Joystick.h"
#include "CANHandler.h"
#include "Motor.h"
#include "EStop.h"

#define ESTOP_PIN 5

Joystick joystick;
CANHandler canHandler;
Motor motor1(1);
Motor motor2(2);
EStop eStop(ESTOP_PIN); // global

void updates()
{
  // Update joystick and button states
  joystick.update();
  eStop.update();
}

void setup()
{
  Serial.begin(115200);  // Initialize Serial for USB communication
  Serial2.begin(115200); // Initialize UART2 for receiving data from joystick
  canHandler.setupCAN();
  eStop.init();

}

void loop()
{
  if (millis() % 5000 < 50 && !eStop.getSet()){
    eStop.set();
    Serial.println("set");
  }

  updates();

  // print timer
  static unsigned long previousPrintTime = 0;
  if (millis() - previousPrintTime > 1000)
  {
    previousPrintTime = millis(); // Update the last print time
    Serial.println("test");
  }

}
