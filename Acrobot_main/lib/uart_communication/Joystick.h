#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick
{
public:
  Joystick();

  // Getter functions
  bool getConnected();
  int getIdx();
  int getAxisLX(); // -508 - 512
  int getAxisLY(); // -508 - 512
  int getAxisRX(); // -508 - 512
  int getAxisRY(); // -508 - 512
  int getL2();     // 0 - 1020
  int getR2();     // 0 - 1020
  int getMisc();
  unsigned int getDpad();    // probably using getDpadUp etc. is preferred
  unsigned int getButtons(); // probably using getButtonCoss etc. is preferred
  long getGyroX();           // at least 6 digits, signed
  long getGyroY();           // at least 6 digits, signed
  long getGyroZ();           // at least 6 digits, signed
  long getAccelX();          // at least 5 digits, signed
  long getAccelY();          // at least 5 digits, signed
  long getAccelZ();          // at least 5 digits, signed

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
  bool getMiscOption();

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

  int getAxisLXCorrected(); // -127 - 128
  int getAxisLYCorrected(); // -127 - 128
  int getAxisRXCorrected(); // -127 - 128
  int getAxisRYCorrected(); // -127 - 128

  bool getMiscPSPressed();
  bool getMiscCreatePressed();
  bool getMiscOptionPressed();

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

  bool connected;
  uint32_t lastSeen = 0;
  int timeOut = 300;
  unsigned int prevButtons;
  unsigned int pressedButtons;
  unsigned int prevPressedButtons;
  unsigned int prevMisc;
  unsigned int pressedMisc;
  unsigned int prevPressedMisc;
  unsigned int prevDpad;
  unsigned int pressedDpad;
  unsigned int prevPressedDpad;
  int deadzoneSize = 20; // default size, can be altered

  int remapAxisValue(int value);

  String receivedData;
};

#endif // JOYSTICK_H
