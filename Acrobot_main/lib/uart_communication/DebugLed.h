#ifndef DEBUGLED_H
#define DEBUGLED_H

#include <Arduino.h>

class DebugLed {
  public:
    DebugLed();
    void setRGB(uint8_t red, uint8_t green, uint8_t blue);
    void setR(uint8_t red);
    void setG(uint8_t green);
    void setB(uint8_t blue);
    void update();
    void flicker(int time = 100);

  private:
    bool colorsChanged = true; // send first command in case second esp didn't reboot
    bool flickerOn = false;
    uint8_t red = 0;
    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t brightnessDivider = 2; // brightness = brightness / brightnessDivider during flicker
    long flickerTimer = 0;
};

#endif