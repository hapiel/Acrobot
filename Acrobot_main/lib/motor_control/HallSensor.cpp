#include "HallSensor.h"

HallSensor::HallSensor(TwoWire &Wire, RemoteDebug &Debug) : Wire(Wire), Debug(Debug)
{
  ADS = new ADS1115(address, &Wire);
  motorIDToSensorTable[ARM_L_ID - 1] = 2;
  motorIDToSensorTable[ARM_R_ID - 1] = 0;
  motorIDToSensorTable[LEG_L_ID - 1] = 3;  // temp value, needs to be corrected when legs are moved.
  motorIDToSensorTable[LEG_R_ID - 1] = 1;
}

void HallSensor::init()
{
  ADS->begin();
  ADS->setGain(1);
}

int16_t HallSensor::getValFromID(int motorID)
{
  return sensorValueOfId[motorID -1];
}


bool HallSensor::getReadyFromID(int motorID, int16_t calibrationTreshold)
{
  return sensorValueOfId[motorID -1] < calibrationTreshold;
}

// TODO: Needs mutex
void HallSensor::update()
{
  // save to a local variable
  for (int i = 0; i < 4; i++)
  {
    sensorValueOfId[i] = ADS->readADC(motorIDToSensorTable[i]);
    // TODO: Slow function, could be rewritten to ADS continuous mode?
  }

  // store local variable in sensorValueOfId in mutex
  

}