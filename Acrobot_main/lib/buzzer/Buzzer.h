#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer
{
public:
  Buzzer(uint8_t pin);
  void init();
  void update();
  void buzz(uint16_t time = 100);

private:
  uint8_t pin;
  uint32_t timer = 0;
};

#endif
