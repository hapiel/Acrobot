#ifndef DEBUGLED_H
#define DEBUGLED_H

#include <Arduino.h>

class DebugLed
{
public:
  DebugLed();
  void setRGB(uint8_t red, uint8_t green, uint8_t blue);
  void setRTemp(uint8_t red, uint16_t ms);
  void setG(uint8_t green);
  void setB(uint8_t blue);
  void update();
  void flicker(int time = 100);

private:
  void setR(uint8_t red);
  bool colorsChanged = true; // send first command in case second esp didn't reboot
  bool flickerOn = false;
  uint8_t red = 0;
  uint8_t blue = 0;
  uint8_t green = 0;
  float brightnessMultiplier = 0.75; // brightness = brightness * brightnessMultiplier during flicker
  long flickerTimer = 0;
  uint32_t redTimer = 0;
  bool redTimerActive = false;
};

#endif