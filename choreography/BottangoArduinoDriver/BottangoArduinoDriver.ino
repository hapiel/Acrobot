// !!! DRIVER VERSION: 0.7.0a1 !!!
// !!! Api Version: 8 !!!

// Modified by Daniel Simu for the Acrobot Show, http://acrobot.nl


#include "src/BottangoCore.h"
#include "src/BasicCommands.h"

const int ledPin = 21;
int ledState = LOW;  // Variable to store the state of the LED
unsigned long previousMillis = 0;

void blinkWithoutDelay(unsigned long onTime, unsigned long offTime) {
  // Get the current time
  unsigned long currentMillis = millis();
  
  // Check if it's time to change the state of the LED
  if ((ledState == HIGH) && (currentMillis - previousMillis >= onTime)) {
    ledState = LOW;  // Turn LED off
    previousMillis = currentMillis;  // Save the time
    digitalWrite(ledPin, ledState);  // Update the LED
  } 
  else if ((ledState == LOW) && (currentMillis - previousMillis >= offTime)) {
    ledState = HIGH;  // Turn LED on
    previousMillis = currentMillis;  // Save the time
    digitalWrite(ledPin, ledState);  // Update the LED
  }
}

void setup()
{

  // pinMode(22, INPUT_PULLUP);
  pinMode(34, INPUT);
  pinMode(36, INPUT); // middle button
  pinMode(39, INPUT);
  pinMode(ledPin, OUTPUT);
  BottangoCore::bottangoSetup();
}

uint32_t printTimer = 0;

void loop()
{
  int battRaw = analogRead(35);
  float battVolt = battRaw * (3.3 / 4096) * 2;
  int battPercent = map(battVolt*100, 250, 420, 0, 100);

  // print status every 3 seconds
  if (printTimer + 3000 < millis() )
  {

    printTimer = millis();
    Serial.print("bat: ");
    Serial.print(battVolt);
    Serial.print("v ");
    Serial.print(battPercent);
    Serial.print("% - ");
    Serial.println(battRaw);
  }

  // check battery status with button
  if (!digitalRead(36)){
    if (battPercent > 75){
      digitalWrite(ledPin, HIGH);
    }
    else if (battPercent > 50){
      blinkWithoutDelay(300, 50);
    }
    else if (battPercent > 25){
      blinkWithoutDelay(200, 200);
    }
    else {
      blinkWithoutDelay(100, 400);
    }

  } else {
    digitalWrite(ledPin, LOW);
  }

  BottangoCore::bottangoLoop();
}
