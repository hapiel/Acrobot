// external libraries
#include <Arduino.h>
#include "Wire.h"
#include "ADS1X15.h"
#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"

// custom libraries
#include "Joystick.h"
#include "CANHandler.h"
#include "Motor.h"
#include "EStop.h"


#define ESTOP_PIN 5
#define SDA_PIN 22 // double check this on board
#define SCL_PIN 33

// external libraries
TwoWire wire(0);
ADS1115 ADS(0x48, &wire);

// custom libraries
Joystick joystick;
CANHandler canHandler;
Motor motor1(1);
Motor motor2(2);
EStop eStop(ESTOP_PIN); // global


void updates()
{
  joystick.update();   // Update joystick and button states
  eStop.update(); 
}

void setup()
{
  Serial.begin(115200);  // Initialize Serial for USB communication
  Serial2.begin(115200); // Initialize UART2 for receiving data from joystick
  canHandler.setupCAN();
  eStop.init();
  wire.begin(SDA_PIN, SCL_PIN);
  ADS.begin();
  // @Esmee, should we set the ADS gain? https://github.com/RobTillaart/ADS1X15#programmable-gain
  

}

void loop()
{
  updates();


  // print timer
  static unsigned long previousPrintTime = 0;
  if (millis() - previousPrintTime > 200)
  {
    previousPrintTime = millis(); // Update the last print time
    Serial.println("test");
  }
}
