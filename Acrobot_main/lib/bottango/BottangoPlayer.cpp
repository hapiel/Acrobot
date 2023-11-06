#include "BottangoPlayer.h"

BottangoPlayer::BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp) : Debug(Debug), legL(legL), legR(legR), armL(armL), armR(armR), file(file), cp(cp)
{
}

void BottangoPlayer::update()
{
  if (armLEnabled || armREnabled || legLEnabled || legREnabled)
  {
    // temp loop exit condition
    Serial.println("start while");
    // int i = 0;
    while (sumAllReadWritePointers() < (bezierBufferLenght - 2) ) //&& i < 10)
    {
      // i++;
      readAndParseCSVRow();
      Serial.println(sumAllReadWritePointers());
      delay(20);
    }
    Serial.println("end while");
  }
  currenttime = millis() - starttime;
  checkEndOfCurve();
  if (armLEnabled == 1)
  {
    Serial.println("armL");
    Serial.println(currenttime);
    armL.setTarget(armLBezier.getValue(currenttime), kp, ki);
    Serial.println("armL set");
  }
  if (armREnabled == 1)
  {
    armR.setTarget(armRBezier.getValue(currenttime), kp, ki);
  }
  if (legLEnabled == 1)
  {
    legL.setTarget(legLBezier.getValue(currenttime), kp, ki);
  }
  if (legREnabled == 1)
  {
    legR.setTarget(legRBezier.getValue(currenttime), kp, ki);
  }
}

void BottangoPlayer::start()
{
  starttime = millis();
  armLEnabled = true;
  armREnabled = true;
  legLEnabled = true;
  legREnabled = true;
}

void BottangoPlayer::stop()
{
  armLEnabled = 0;
  armREnabled = 0;
  legLEnabled = 0;
  legREnabled = 0;
}

void BottangoPlayer::loadFile(const char *csvDir)
{
  strcpy(currentFileDir, csvDir);
  Serial.println(currentFileDir);
  openCSV();
}
void BottangoPlayer::checkEndOfCurve()
{
  if (!armLBezier.isInProgress(currenttime))
  {
    // Serial.println("armL not in progress");
    if (armLCurveRead != armLCurveWrite)
    {
      Serial.println("armL read != write");
      armLBezier.setControllPoints(armLCurveArray[armLCurveRead]);
      armLCurveRead++;
      if (armLCurveRead >= bezierBufferLenght)
      {
        armLCurveRead = 0;
      }
    }
    else if (csvDisabledFlag)
    {
      armLEnabled = false;
    }
  }

  if (!armRBezier.isInProgress(currenttime))
  {
    if (armRCurveRead != armRCurveWrite)
    {
      armRBezier.setControllPoints(armRCurveArray[armRCurveRead]);
      armRCurveRead++;
      if (armRCurveRead >= bezierBufferLenght)
      {
        armRCurveRead = 0;
      }
    }
    else if (csvDisabledFlag)
    {
      armREnabled = false;
    }
  }

  if (!legLBezier.isInProgress(currenttime))
  {
    if (legLCurveRead != legLCurveWrite)
    {
      legLBezier.setControllPoints(legLCurveArray[legLCurveRead]);
      legLCurveRead++;
      if (legLCurveRead >= bezierBufferLenght)
      {
        legLCurveRead = 0;
      }
    }
    else if (csvDisabledFlag)
    {
      legLEnabled = false;
    }
  }

  if (!legRBezier.isInProgress(currenttime))
  {
    if (legRCurveRead != legRCurveWrite)
    {
      legRBezier.setControllPoints(legRCurveArray[legRCurveRead]);
      legRCurveRead++;
      if (legRCurveRead >= bezierBufferLenght)
      {
        legRCurveRead = 0;
      }
    }
    else if (csvDisabledFlag)
    {
      legREnabled = false;
    }
  }
}

void BottangoPlayer::openCSV()
{
  file = SD.open(currentFileDir,FILE_READ);
  if (!file) {
    debugE("File (%s) open failed",currentFileDir);
    fileReady = 0;
  }
  else{
    Serial.println("openCSV else");
    csvDisabledFlag = false;
    fileReady = 1;
  }
}


void BottangoPlayer::closeCSV()
{
  file.close();
}

void BottangoPlayer::readAndParseCSVRow()
{
  if (cp.parseRow())
  {
    // original types of Ms and durationMs ints, can be optimized by using a struct.
    char **commandType = (char **)cp[0];
    char **motorID = (char **)cp[1];
    float *startMs = (float *)cp[2];
    float *durationMs = (float *)cp[3];
    float *startPosition = (float *)cp[4];
    float *startControlPointX = (float *)cp[5];
    float *startControlPointY = (float *)cp[6];
    float *endPosition = (float *)cp[7];
    float *endControlPointX = (float *)cp[8];
    float *endControlPointY = (float *)cp[9];

    float controllArray[8] = {startMs[0], durationMs[0], startPosition[0], startControlPointX[0], startControlPointY[0], endPosition[0], endControlPointX[0], endControlPointY[0]};

    Serial.printf("cmd: %s, id: %s, startMS: %f, durationMS: %f, startPosition: %f, startControlPointX: %f, startControlPointY: %f, endPosition: %f, endControlPointX: %f, endControlPointY: %f\n", commandType[0], motorID[0], startMs[0], durationMs[0], startPosition[0], startControlPointX[0], startControlPointY[0], endPosition[0], endControlPointX[0], endControlPointY[0]);

    Serial.printf("0: %f, 1: %f, 2: %f, 3: %f, 4: %f, 5: %f, 6: %f, 7: %f\n", controllArray[0], controllArray[1], controllArray[2], controllArray[3], controllArray[4], controllArray[5], controllArray[6], controllArray[7]);

    if (strcmp(commandType[0], "sC") == 0)
    Serial.println("sC");
    {
      if (strcmp(motorID[0], "m_arm_l") == 0)
      {
        Serial.println("m_arm_l");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(armLCurveArray[armLCurveWrite]));
        armLCurveWrite++;
        if (armLCurveWrite == bezierBufferLenght)
        {
          armLCurveWrite = 0;
        }
      }
      else if (strcmp(motorID[0], "m_arm_r") == 0)
      {
        Serial.println("m_arm_r");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(armRCurveArray[armRCurveWrite]));
        armRCurveWrite++;
        Serial.printf("armRCurveWrite: %i, armRCurveRead: %i\n", armRCurveWrite, armRCurveRead);
        if (armRCurveWrite == bezierBufferLenght)
        {
          armRCurveWrite = 0;
        }
        Serial.printf("armRCurveWrite: %i, armRCurveRead: %i\n", armRCurveWrite, armRCurveRead);
      }
      else if (strcmp(motorID[0], "m_leg_l") == 0)
      {
        Serial.println("m_leg_l");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(legLCurveArray[legLCurveWrite]));
        legLCurveWrite++;
        if (legLCurveWrite == bezierBufferLenght)
        {
          legLCurveWrite = 0;
        }
      }
      else if (strcmp(motorID[0], "m_leg_r") == 0)
      {
        Serial.println("m_leg_r");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(legRCurveArray[legRCurveWrite]));
        legRCurveWrite++;
        if (legRCurveWrite == bezierBufferLenght)
        {
          legRCurveWrite = 0;
        }
      }
    }
  }
  else
  {
    closeCSV();
    fileReady = true;
    enabled = false;
    csvDisabledFlag = true;
  }
}

int BottangoPlayer::sumAllReadWritePointers()
{
  int deltaPointersArmL = armLCurveWrite - armLCurveRead;
  if (deltaPointersArmL < 0)
    deltaPointersArmL + bezierBufferLenght;
  int deltaPointersArmR = armRCurveWrite - armRCurveRead;
  if (deltaPointersArmR < 0)
    deltaPointersArmR + bezierBufferLenght;
  int deltaPointersLegL = legLCurveWrite - legLCurveRead;
  if (deltaPointersLegL < 0)
    deltaPointersLegL + bezierBufferLenght;
  int deltaPointersLegR = legRCurveWrite - legRCurveRead;
  if (deltaPointersLegR < 0)
    deltaPointersLegR + bezierBufferLenght;
  return (deltaPointersArmL + deltaPointersArmR + deltaPointersLegL + deltaPointersLegR);
}

