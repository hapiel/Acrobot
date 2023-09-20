#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include <Arduino.h>
#include "EStop.h"


void eStopTestLoop(){

  if (millis() % 5000 < 50 && !eStop.getSet()){
    eStop.set();
    Serial.println("set");
  }
  
}

#endif