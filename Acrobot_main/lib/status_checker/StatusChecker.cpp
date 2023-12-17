// StatusChecker.cpp, based on StatusChecker.h

// if battery is below 20%, debugled red half on
// if battery is below 15%, repeatedly beep
// if battery is below 13%, trigger eStop

// in update, check wifi connection. set debugled green 20 if connected.
// in update, check if joystick is connected. set debugled blue 20 if connected.
// flicker the led every second to show that the program is running using the flicker function

#include "StatusChecker.h"

StatusChecker::StatusChecker(RemoteDebug &Debug, BatterySensor &batterySensor, Buzzer &buzzer, DebugLed &debugLed, Joystick &joystick, EStop &eStop) : Debug(Debug), batterySensor(batterySensor), buzzer(buzzer), debugLed(debugLed), joystick(joystick), eStop(eStop)
{
}

void StatusChecker::checkBattery()
{
  if (batterySensor.getPercentage() <= 20)
  {
    if (lastInfoTime + 1000 < millis())
    {
      debugI("battery low, pecentage: %d", batterySensor.getPercentage());
      lastInfoTime = millis();
    }

    debugLed.setR(128); // doesn't reset, so other functions can trigger red too.
    if (batterySensor.getPercentage() <= 12)
    {

      if (lastWarningTime + 1000 < millis())
      {
        debugW("battery VERY low, pecentage: %d", batterySensor.getPercentage());
        lastWarningTime = millis();
      }

      if (millis() % 500 < 50)
      {
        buzzer.buzz(50);
      }
    }
  }
}

void StatusChecker::update()
{

  checkBattery();

  if (WiFi.status() == WL_CONNECTED)
  {
    debugLed.setG(10);
  }
  else
  {
    debugLed.setG(0);
  }

  if (joystick.getConnected())
  {
    debugLed.setB(10);

    // display joystick battery level on joystick
    static long executionTimer = 0;
    if (runEvery(1000, executionTimer))
    {
      if (joystick.getBatteryPercentage() <= 20)
      {
        joystick.setPlayerLEDs(0);
      }
      if(joystick.getBatteryPercentage() > 20 && joystick.getBatteryPercentage() <= 40)
      {
        joystick.setPlayerLEDs(1);
      }
      if(joystick.getBatteryPercentage() > 40 && joystick.getBatteryPercentage() <= 60)
      {
        joystick.setPlayerLEDs(2);
      }
      if(joystick.getBatteryPercentage() > 60 && joystick.getBatteryPercentage() <= 80)
      {
        joystick.setPlayerLEDs(3);
      }
      if(joystick.getBatteryPercentage() > 80 )
      {
        joystick.setPlayerLEDs(4);
      }
    }
  }
  else
  {
    debugLed.setB(0);
  }

  if (lastFlickerTime + flickerRate < millis())
  {
    debugLed.flicker(50);
    lastFlickerTime = millis();
  }
}