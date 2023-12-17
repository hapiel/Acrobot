#include "HallSensor.h"

HallSensor::HallSensor(TwoWire &Wire, RemoteDebug &Debug) : Wire(Wire), Debug(Debug)
{
  ADS = new ADS1115(address, &Wire);
  motorIDToSensorTable[ARM_L_ID - 1] = 2;
  motorIDToSensorTable[ARM_R_ID - 1] = 0;
  motorIDToSensorTable[LEG_L_ID - 1] = 3;
  motorIDToSensorTable[LEG_R_ID - 1] = 1;
  mutex = xSemaphoreCreateMutex();
}

void HallSensor::init()
{
  ADS->begin();
  ADS->setGain(1);
}

int16_t HallSensor::getValFromID(int motorID)
{
  int16_t val;
  if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
  {
    
    val = sensorValueOfId[motorID - 1];
    xSemaphoreGive(mutex);
  }

  return val;

}

bool HallSensor::getReadyFromID(int motorID, int16_t calibrationThreshold)
{
  bool ready;
  if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
  {
    ready =  sensorValueOfId[motorID - 1] < calibrationThreshold;
    xSemaphoreGive(mutex);
  }

  return ready;
}

void HallSensor::update()
{
  // save to a local variable
  int16_t tempSensorValueOfId[SENSOR_COUNT];

  for (int i = 0; i < SENSOR_COUNT; i++)
  {
    tempSensorValueOfId[i] = ADS->readADC(motorIDToSensorTable[i]);
    // TODO: Slow function, could be rewritten to ADS continuous mode?
  }


  if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE)
  {
    for (int i = 0; i < 4; i++)
    {
      sensorValueOfId[i] = tempSensorValueOfId[i];
    }
    xSemaphoreGive(mutex);
  }
  
}