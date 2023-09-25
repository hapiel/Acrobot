#include "EStop.h"

EStop::EStop(int pin, RemoteDebug& Debug) : pin(pin), Debug(Debug) {
}

void EStop::set() {
      digitalWrite(pin, HIGH);
      pinMode(pin, OUTPUT);
      isSet = true;
      lastTimeSet = millis();
      debugW("ESTOP: set");
    }

void EStop::unSet(){
      digitalWrite(pin, HIGH);
      pinMode(pin, INPUT);
      isSet = false;
      debugD("ESTOP: unset");
    }

void EStop::init(){
  unSet(); // this can't go in constructor because needs to be done after setup()
}

bool EStop::getSet(){
  return isSet;
}


void EStop::update(){

  if (isSet && lastTimeSet + setDuration < millis()){
    unSet();
  }
}