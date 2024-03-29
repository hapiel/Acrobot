#include "Joystick.h"

Joystick::Joystick(RemoteDebug &Debug) : Debug(Debug)
{
  // Initialize your variables here if needed
}

bool Joystick::getConnected()
{
  return connected;
}

// Implement the getter functions
int Joystick::getIdx()
{
  return idx;
}

int Joystick::getAxisLX()
{
  return axisLX;
}

int Joystick::getAxisLY()
{
  return axisLY;
}

int Joystick::getAxisRX()
{
  return axisRX;
}

int Joystick::getAxisRY()
{
  return axisRY;
}

int Joystick::getL2()
{
  return brake;
}

int Joystick::getR2()
{
  return throttle;
}

int Joystick::getMisc()
{
  return misc;
}

unsigned int Joystick::getDpad()
{
  return dpad;
}

unsigned int Joystick::getButtons()
{
  return buttons;
}

long Joystick::getGyroX()
{
  return gyroX;
}

long Joystick::getGyroY()
{
  return gyroY;
}

long Joystick::getGyroZ()
{
  return gyroZ;
}

long Joystick::getAccelX()
{
  return accelX;
}

long Joystick::getAccelY()
{
  return accelY;
}

long Joystick::getAccelZ()
{
  return accelZ;
}

bool Joystick::getDpadUp()
{
  return dpad & 0x01;
}

bool Joystick::getDpadDown()
{
  return dpad & 0x02;
}

bool Joystick::getDpadRight()
{
  return dpad & 0x04;
}

bool Joystick::getDpadLeft()
{
  return dpad & 0x08;
}

bool Joystick::getButtonCross()
{
  return buttons & 0x0001;
}

bool Joystick::getButtonCircle()
{
  return buttons & 0x0002;
}

bool Joystick::getButtonSquare()
{
  return buttons & 0x0004;
}

bool Joystick::getButtonTriangle()
{
  return buttons & 0x0008;
}

bool Joystick::getButtonL1()
{
  return buttons & 0x0010;
}

bool Joystick::getButtonR1()
{
  return buttons & 0x0020;
}

bool Joystick::getButtonL3()
{
  return buttons & 0x0100;
}

bool Joystick::getButtonR3()
{
  return buttons & 0x0200;
}

bool Joystick::getMiscPS()
{
  return misc & 0x0001;
}

bool Joystick::getMiscCreate()
{
  return misc & 0x0002;
}

bool Joystick::getMiscOption()
{
  return misc & 0x0004;
}
bool Joystick::getDpadUpPressed()
{
  return pressedDpad & 0x0001;
}

bool Joystick::getDpadDownPressed()
{
  return pressedDpad & 0x0002;
}

bool Joystick::getDpadRightPressed()
{
  return pressedDpad & 0x0004;
}

bool Joystick::getDpadLeftPressed()
{
  return pressedDpad & 0x0008;
}

bool Joystick::getButtonCrossPressed()
{
  return pressedButtons & 0x0001;
}

bool Joystick::getButtonCirclePressed()
{
  return pressedButtons & 0x0002;
}

bool Joystick::getButtonSquarePressed()
{
  return pressedButtons & 0x0004;
}

bool Joystick::getButtonTrianglePressed()
{
  return pressedButtons & 0x0008;
}

bool Joystick::getButtonL1Pressed()
{
  return pressedButtons & 0x0010;
}

bool Joystick::getButtonR1Pressed()
{
  return pressedButtons & 0x0020;
}

bool Joystick::getButtonL3Pressed()
{
  return pressedButtons & 0x0100;
}

bool Joystick::getButtonR3Pressed()
{
  return pressedButtons & 0x0200;
}

bool Joystick::getMiscPSPressed()
{
  return pressedMisc & 0x0001;
}

bool Joystick::getMiscCreatePressed()
{
  return pressedMisc & 0x0002;
}

bool Joystick::getMiscOptionPressed()
{
  return pressedMisc & 0x0004;
}

void Joystick::setDeadzoneSize(unsigned int size)
{
  deadzoneSize = size;
}

int Joystick::remapAxisValue(int value)
{
  if (value >= -deadzoneSize && value <= deadzoneSize)
  {
    return 0;
  }

  int remappedValue;
  if (value > deadzoneSize)
  {
    remappedValue = map(value, deadzoneSize + 1, 512, 0, 128);
  }
  else
  {
    remappedValue = map(value, -508, -deadzoneSize - 1, -127, 0);
  }

  return remappedValue;
}

int Joystick::getAxisLXCorrected()
{
  return remapAxisValue(axisLX);
}

int Joystick::getAxisLYCorrected()
{
  return remapAxisValue(axisLY);
}

int Joystick::getAxisRXCorrected()
{
  return remapAxisValue(axisRX);
}

int Joystick::getAxisRYCorrected()
{
  return remapAxisValue(axisRY);
}

int Joystick::getBatteryPercentage()
{
  // Remap battery value from 0-255 to 0-99
  return map(battery, 0, 255, 0, 99);
}

void Joystick::setRumble(int force, int duration)
{
  // currently not implemented on joystick esp
  Serial2.printf("r=%d,%d\n", force, duration);
}

void Joystick::setColorLED(int red, int green, int blue)
{
  Serial2.printf("jC=%d,%d,%d\n", red, green, blue);
}

void Joystick::setPlayerLEDs(int ledMask)
{
  // 0 is off, 1 - 4 sets leds.
  Serial2.printf("jP=%d\n", ledMask);
}

void Joystick::update()
{
  // Update previous button states
  prevButtons = buttons;
  prevMisc = misc;
  prevDpad = dpad;
;

  // read the new button states
  if (Serial2.available())
  {
    receivedData = Serial2.readStringUntil('\n'); // Read data until newline character
    if (receivedData.length() >= 107 && receivedData.length() <= 110)             // check for corruption
    {
      // Serial.println(receivedData.c_str());
      // Interpret the received data
      sscanf(receivedData.c_str(),
             "i%d d%x b%x aL%d %d aR%d %d b%d t%d m%x gx%ld y%ld z%ld ax%ld y%ld z%ld b%ld",
             &idx, &dpad, &buttons, &axisLX, &axisLY, &axisRX, &axisRY, &brake, &throttle, &misc,
             &gyroX, &gyroY, &gyroZ, &accelX, &accelY, &accelZ, &battery);

      lastSeen = millis();

      if (!connected){
        // new connection
        debugI("Joystick connected");
        setColorLED(0, 0, 0);
      }
      connected = true;
    } else {
      debugE("Received non-joystick data. Len: %d, data: %s", receivedData.length(), receivedData.c_str());
    }
  }

  // current state matches previous state?
  pressedButtons = prevPressedButtons & buttons;
  pressedMisc = prevPressedMisc & misc;
  pressedDpad = prevPressedDpad & dpad;

  // Update for next round
  prevPressedButtons = buttons & ~prevButtons;
  prevPressedMisc = misc & ~prevMisc;
  prevPressedDpad = dpad & ~prevDpad;

  if (lastSeen + timeOut < millis())
  {
    connected = false;
  }


}