#include <Arduino.h>
#include "Joystick.h"
#include "CANHandler.h"
#include "Motor.h"
#include "EStop.h"

// #include "testFunctions.h" // for running tests.

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
  // // TESTS:
  // eStopTestLoop();

  // // END TESTS

  updates();

  static int playerCount = 0;
  // if (joystick.getButtonCrossPressed())
  // {
  //   playerCount++;
  //   joystick.setPlayerLEDs(playerCount);
  //   motor1.start();
  //   delay(1000);
  //   motor1.reZero();
  //   delay(1000);
  // }

  if (joystick.getButtonCirclePressed())
  {
    motor1.start();
    delay(1000);
    motor1.reZero();
    delay(1000);
  }

  if (joystick.getButtonSquarePressed())
  {
    motor2.start();
    delay(1000);
    motor2.reZero();
    delay(1000);
  }

  if (joystick.getButtonTriangle() || joystick.getButtonCross())
  {
    motor1.stop();
    motor2.stop();
  }

  if (joystick.getButtonR1())
  {
    motor1.setPosition(joystick.getAxisRXMapped() / 50.0, 2, 0.7);
  }
  if (joystick.getButtonL1())
  {
    motor2.setVelocity(joystick.getAxisLXMapped() / 20.0, 1);
  }

  // print timer
  static unsigned long previousPrintTime = 0;
  if (millis() - previousPrintTime > 200)
  {
    previousPrintTime = millis(); // Update the last print time
    Serial.println("test");
  }

  // Serial.print(joystick.getButtonCross());
  // Serial.print(" ");
  // Serial.println(joystick.getButtonCrossPressed());

  // Serial.println(playerCount);
}
