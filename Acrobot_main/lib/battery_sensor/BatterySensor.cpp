#include <BatterySensor.h>

BatterySensor::BatterySensor(uint8_t pin) : pin(pin){
}

void BatterySensor::init()
{
  pinMode(pin, INPUT);
}

int BatterySensor::getPercentage()
{
  int avg = batterySamples.getAverage();

  if (avg < chart[0][1]){
    return 0;
  }

  for (int i = 1; i < chartSize; i++){
    if (avg < chart[i][1]){
      return map(avg, chart[i - 1][1] + 1, chart[i][1], chart[i - 1][2], chart[i][2]);
    }
  }
  return 99;
}

int BatterySensor::getAdc()
{
  return batterySamples.getAverage();
}

float BatterySensor::getVoltage()
{
  int avg = batterySamples.getAverage();

  if (avg < chart[0][1]){
    return 0;
  }

  for (int i = 1; i < chartSize; i++){
    if (avg < chart[i][1]){
      return fMap(avg, chart[i - 1][1] + 1, chart[i][1], chart[i - 1][0], chart[i][0]);
    }
  }
  return 99;
}



void BatterySensor::update(){
  batterySamples.add(analogRead(pin));
}

