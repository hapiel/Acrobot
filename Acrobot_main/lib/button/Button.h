#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
  Button(uint8_t pin);
  void init();
  void update();
  bool isDown();
  bool isPressed();
  bool isReleased();
  bool isHeldFor(unsigned long time);

private:
  uint8_t pin;
  bool prevState;
  unsigned long lastPressTime;
  bool buttonDown;
  bool pressed;
  bool released;
  int debounceTime = 25;
};

#endif
