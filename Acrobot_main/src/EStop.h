#ifndef ESTOP_H
#define ESTOP_H

#include <Arduino.h>

class EStop {
  public:
    EStop(int pin);
    void set();
    void update();
    bool getSet();

  private:
    void unSet();
    uint32_t lastTimeSet;
    int pin;
    bool isSet = false;
    int setDuration = 300; // time in ms before it can be manually released

};

#endif