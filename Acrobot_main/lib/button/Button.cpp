#include "Button.h"

Button::Button(uint8_t _pin, RemoteDebug& Debug) : pin(_pin), Debug(Debug), prevState(LOW), lastActionTime(0), buttonDown(false)  {}

void Button::init() {
  pinMode(pin, INPUT);
}

void Button::update() {
  prevState = buttonDown;
  bool buttonState = digitalRead(pin);
  pressed = false;
  released = false;

  // Check if the button was changed and debounce
  if (buttonState != prevState && millis() - lastActionTime >= debounceTime)
  {
    lastActionTime = millis();
    if (buttonState){
      buttonDown = true;
      pressed = true;
      debugI("BUTTON: pressed, pin: %u", pin);
    } else {
      buttonDown = false;
      released = true;
      debugD("BUTTON: released, pin: %u", pin);
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
  return isDown() && (millis() - lastActionTime >= time);
}