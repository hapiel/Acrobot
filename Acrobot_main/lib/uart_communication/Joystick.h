#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick
{
public:
  Joystick();

  // Getter functions
  int getIdx();
  int getAxisLX();
  int getAxisLY();
  int getAxisRX();
  int getAxisRY();
  int getL2();
  int getR2();
  int getMisc();
  unsigned int getDpad();    // probably using getDpadUp etc. is preferred
  unsigned int getButtons(); // probably using getButtonCoss etc. is preferred
  long getGyroX();
  long getGyroY();
  long getGyroZ();
  long getAccelX();
  long getAccelY();
  long getAccelZ();

  bool getDpadUp();
  bool getDpadDown();
  bool getDpadLeft();
  bool getDpadRight();

  bool getButtonCross();
  bool getButtonCircle();
  bool getButtonSquare();
  bool getButtonTriangle();

  bool getButtonL1();
  bool getButtonR1();
  bool getButtonL3();
  bool getButtonR3();

  bool getMiscPS();
  bool getMiscCreate();
  bool getMiscOptions();

  bool getDpadUpPressed();
  bool getDpadDownPressed();
  bool getDpadLeftPressed();
  bool getDpadRightPressed();

  bool getButtonCrossPressed();
  bool getButtonCirclePressed();
  bool getButtonSquarePressed();
  bool getButtonTrianglePressed();

  bool getButtonL1Pressed();
  bool getButtonR1Pressed();
  bool getButtonL3Pressed();
  bool getButtonR3Pressed();

  int getAxisLXCorrected();
  int getAxisLYCorrected();
  int getAxisRXCorrected();
  int getAxisRYCorrected();

  bool getMiscPSPressed();
  bool getMiscCreatePressed();
  bool getMiscOptionsPressed();

  int getBatteryPercentage();

  // setters
  void setRumble(int force, int duration); // not working on other esp currently
  void setColorLED(int red, int green, int blue);
  void setPlayerLEDs(int ledMask);

  void setDeadzoneSize(unsigned int size);

  void update();

private:
  int idx, axisLX, axisLY, axisRX, axisRY, brake, throttle, misc, battery;
  unsigned int dpad, buttons;
  long gyroX, gyroY, gyroZ, accelX, accelY, accelZ;

  unsigned int prevButtons;
  unsigned int pressedButtons;
  unsigned int prevMisc;
  unsigned int pressedMisc;
  unsigned int prevDpad;
  unsigned int pressedDpad;
  int deadzoneSize = 20; // default size, can be altered

  int remapAxisValue(int value);

  String receivedData;
};

#endif // JOYSTICK_H
