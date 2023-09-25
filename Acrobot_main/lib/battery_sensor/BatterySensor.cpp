#include <BatterySensor.h>

BatterySensor::BatterySensor(uint8_t pin) : pin(pin){
}

void BatterySensor::init()
{
  pinMode(pin, INPUT);
}

int BatterySensor::getPercentage()
{
  return map(batterySamples.getAverage(), 0, 4096, 0, 100);
}

void BatterySensor::update(){
  batterySamples.add(analogRead(pin));
}

