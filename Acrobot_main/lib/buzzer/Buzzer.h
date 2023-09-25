#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "RemoteDebug.h"

class Buzzer
{
public:
  Buzzer(uint8_t pin, RemoteDebug& Debug);
  void init();
  void update();
  void buzz(uint16_t ms = 100);

private:
  uint8_t pin;
  uint32_t timer = 0;
  RemoteDebug& Debug;
};

#endif
