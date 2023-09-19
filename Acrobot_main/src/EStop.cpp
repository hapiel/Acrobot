#include "EStop.h"


EStop::EStop(int pin) : pin(pin) {
  unSet();
}

void EStop::set() {
      digitalWrite(pin, HIGH);
      pinMode(pin, OUTPUT);
      isSet = true;
      lastTimeSet = millis();
    }

void EStop::unSet(){
      digitalWrite(pin, HIGH);
      pinMode(pin, INPUT);
      isSet = false;
    }

bool EStop::getSet(){
  return isSet;
}


void EStop::update(){
  if (isSet && lastTimeSet + setDuration < millis()){
    unSet();
  }
}