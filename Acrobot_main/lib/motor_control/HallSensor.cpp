#include "HallSensor.h"

HallSensor::HallSensor(TwoWire &wire, RemoteDebug &Debug) : wire(wire), Debug(Debug)
{
  ADS = new ADS1115(address, &wire);
}

void HallSensor::init()
{
  ADS->begin();
  ADS->setGain(1);
}

int16_t HallSensor::getArmL()
{
  return ADS->readADC(2);
}

int16_t HallSensor::getArmR()
{
  return ADS->readADC(0);
}

int16_t HallSensor::getLegL()
{
  return ADS->readADC(1);
}

int16_t HallSensor::getLegR()
{
  return ADS->readADC(3);
}
