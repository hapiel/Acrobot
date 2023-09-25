#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "RemoteDebug.h"

class Button {
public:
  Button(uint8_t pin, RemoteDebug& Debug);
  void init();
  void update();
  bool isDown();
  bool isPressed();
  bool isReleased();
  bool isHeldFor(unsigned long time);

private:
  uint8_t pin;
  bool prevState;
  unsigned long lastActionTime;
  bool buttonDown;
  bool pressed;
  bool released;
  int debounceTime = 40;
  RemoteDebug& Debug;
};

#endif
