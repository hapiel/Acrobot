#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include <Arduino.h>
#include "EStop.h"

EStop eStopTest(7);

void eStopTestLoop(){

  if (millis() % 5000 < 50 && !eStopTest.getSet()){
    eStopTest.set();
    Serial.println("set");
  }

  eStopTest.update();
  
}

#endif