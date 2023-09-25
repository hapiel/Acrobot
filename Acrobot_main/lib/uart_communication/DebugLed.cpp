#include "DebugLed.h"

DebugLed::DebugLed(){}

void DebugLed::setRGB(uint8_t _red, uint8_t _green, uint8_t _blue) {
  red = _red;
  green = _green;
  blue = _blue;
  sendMostRecentColors = false;
}

void DebugLed::setR(uint8_t _red){
  red = _red;
  sendMostRecentColors = false;
}

void DebugLed::setG(uint8_t _green){
  green = _green;
  sendMostRecentColors = false;
}

void DebugLed::setB(uint8_t _blue){
  blue = _blue;
  sendMostRecentColors = false;
}

void DebugLed::enableFlicker(){
    flicker = true;
    sendMostRecentColors = false;
}

void DebugLed::disableFlicker(){
    flicker = false;
    sendMostRecentColors = false;
}

void DebugLed::update(){
    if(flicker == false){
        if(sendMostRecentColors == false){
            Serial2.printf("set_debug_led=%u,%u,%u\n", red, green, blue);
            sendMostRecentColors = true;
        }
    }
    else{
        if((millis()%flickerPeriod)==0){
            sendMostRecentColors = false;
        }
        if(sendMostRecentColors == false){
            if((millis()%flickerPeriod)<=flickerDutycycle){
                Serial2.printf("set_debug_led=%u,%u,%u\n", red, green, blue);
                sendMostRecentColors = true;
            }
            else{
                Serial2.printf("set_debug_led=%u,%u,%u\n", red/brightnessDevider, green/brightnessDevider, blue/brightnessDevider);
                sendMostRecentColors = true;
            }
        }
    }

}