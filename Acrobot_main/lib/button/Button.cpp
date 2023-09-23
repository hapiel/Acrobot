#include "Button.h"

Button::Button(uint8_t _pin) : pin(_pin), prevState(LOW), lastPressTime(0), buttonDown(false) {}

void Button::init() {
  pinMode(pin, INPUT);
}

void Button::update() {
  prevState = buttonDown;
  bool buttonState = digitalRead(pin);
  pressed = false;
  released = false;

  // Check if the button was changed and debounce
  if (buttonState != prevState && millis() - lastPressTime >= debounceTime)
  {
    if (buttonState){
      lastPressTime = millis();
      buttonDown = true;
      pressed = true;
    } else {
      buttonDown = false;
      released = true;
    }
  }

}

bool Button::isDown() {
  return buttonDown;
}

bool Button::isPressed() {
  return pressed;
}

bool Button::isReleased() {
  return released;
}

bool Button::isHeldFor(unsigned long time) {
  return isDown() && (millis() - lastPressTime >= time);
}