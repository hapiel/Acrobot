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
    void update(void);
    void enableFlicker(void);
    void disableFlicker(void);

  private:
    bool sendMostRecentColors = true;
    bool flicker = false;
    uint8_t red = 0;
    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t brightnessDevider = 2;
    uint16_t flickerDutycycle = 1000;
    uint16_t flickerPeriod = 2000;
};

#endif