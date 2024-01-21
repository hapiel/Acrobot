#include "BottangoPlayerNew.h"

BottangoPlayer::BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, File &file, CSV_Parser &cp) : Debug(Debug), legL(legL), legR(legR), armL(armL), armR(armR), file(file), cp(cp)
{
}

int BottangoPlayer::getLargestError()
{
    int armLError = abs(armLBeginPosition - armL.getTarget());
    int armRError = abs(armRBeginPosition - armR.getTarget());
    int legLError = abs(legLBeginPosition - legL.getTarget());
    int legRError = abs(legRBeginPosition - legR.getTarget());
    return(armLError, armRError, legLError, legRError);
}

void BottangoPlayer::outputSetpoints()
{
    if (armLBezier.isInProgress(currenttime))
    {
        armR.setTarget(armRBezier.getValue(currenttime), kp, ki);
    }
    if (legLEnabled == 1 & armLBezier.isInProgress(currenttime))
    {
        legL.setTarget(legLBezier.getValue(currenttime), kp, ki);
    }
    if (legREnabled == 1 & armLBezier.isInProgress(currenttime))
    {
        legR.setTarget(legRBezier.getValue(currenttime), kp, ki);
    }
}
void BottangoPlayer::update()
{
    currentTime = millis();
    static unsigned long int goToBeginTimeStart;
    static unsigned long int goToBeginTimeEnd;
    switch(bottangoPlayerState){
        case STOP_MOVE:
        {
            break;
        }
        case INIT_MOVE:// maak hier een apparte functie van
        {
            file = SD.open(FileDir, FILE_READ);
            readAndParseCSVRow();//responsible for end target
        }
        case INIT_BEGIN: // maak hier een apparte functie van
        {
            int largestError = getLargestError();
            if(largestError > errorThreshold)
            {
                int goToBeginTimeduration = int(largestError/goToBeginMaxSpeed);
                goToBeginTimeStart = currentTime;
                goToBeginTimeEnd = currentTime+goToBeginTimeduration;
                armLStartPos = armL.getTarget();
                armRStartPos = armR.getTarget();
                legLStartPos = legL.getTarget();
                legRStartPos = legR.getTarget();
            }
            //else
            //{
            //    bottangoPlayerState = EXECUTE_MOVE;
            //}
        }
        case GO_TO_BEGIN://safestart
        
        {
            int largestError = getLargestError();
            if(largestError > errorThreshold)
            {
                bottangoPlayerState = EXECUTE_MOVE;
            }
            else
            {

            }
            
        }
        case EXECUTE_MOVE://hele csv bestand afspelen
        {
            while (sumAllReadWritePointers() < (bezierBufferLenght - 4) && fileReady == true)// && i < 10)
            {
                readAndParseCSVRow();
                Serial.printf("curves in buffers: %d.\n",sumAllReadWritePointers());
                //delay(20);
                if (armLBezier.isInProgress(currenttime))
                {
                    armL.setTarget(armLBezier.getValue(currenttime), kp, ki);
                }

            }

        }

    }


  if (armLEnabled || armREnabled || legLEnabled || legREnabled)
  {
    while (sumAllReadWritePointers() < (bezierBufferLenght - 4) && fileReady == true)// && i < 10)
    {
      readAndParseCSVRow();
      Serial.printf("curves in buffers: %d.\n",sumAllReadWritePointers());
      //delay(20);
    }
  }
  else{
    starttime = millis();
  }
  currenttime = millis() - starttime;
  checkEndOfCurve();
  Serial.printf("curves in  %d, file ready: %d\n",sumAllReadWritePointers(), fileReady);
  //Serial.printf("armL enabled: %d\n",armLEnabled);
  Serial.printf("current time: %d, start time: %d\n", currenttime, starttime);
  Serial.printf("armL enabled: %d, start Y: %f, start time: %d, end time: %d\n",armLEnabled,armLBezier.getStartMovement(),armLBezier.getStartTimeMs(),armLBezier.getEndTimeMs());
  //Serial.printf("armR enabled: %d, start Y: %f, start time: %d, end time: %d\n",armREnabled,armRBezier.getStartMovement(),armRBezier.getStartTimeMs(),armRBezier.getEndTimeMs());
  //Serial.printf("legL enabled: %d, start Y: %f, start time: %d, end time: %d\n",legLEnabled,legLBezier.getStartMovement(),legLBezier.getStartTimeMs(),legLBezier.getEndTimeMs());
  //Serial.printf("legR enabled: %d, start Y: %f, start time: %d, end time: %d\n",legREnabled,legRBezier.getStartMovement(),legRBezier.getStartTimeMs(),legRBezier.getEndTimeMs());
  

}

void BottangoPlayer::start()
{
  starttime = millis();
  armLEnabled = true;
  armREnabled = true;
  legLEnabled = true;
  legREnabled = true;
  ForceRead == true;
  Serial.printf("Forceread = %d\n", ForceRead);
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
  Serial.printf("checking forceread\n");
  if (ForceRead==true){
    Serial.printf("ForceRead = %d\n", ForceRead);
  }
  
  if (armLBezier.isFinished(currenttime)||ForceRead == true)
  {
    Serial.printf("ForceRead = %d\n", ForceRead);
    if (armLCurveRead != armLCurveWrite)
    {
      Serial.printf("armL setControllPoints, read: %d, write: %d, forceread: %d\n", armLCurveRead, armLCurveWrite, ForceRead);
      unsigned int lastArmLCurveRead;
      if (armLCurveRead == 0){
        unsigned int lastArmLCurveRead = 10;
      }
      else{
        unsigned int lastArmLCurveRead = armLCurveRead -1;
      }
      if(false){// (armLCurveArray[armLCurveRead][0]<armLCurveArray[lastArmLCurveRead][0]&&resetTime==false){
        //hier meot gewachten worden tot de alle curves klaar zijn, dan tijd nul, dan next file
        armLWaitingOnNextFile = true;
      }
      else{
        armLBezier.setControllPoints(armLCurveArray[armLCurveRead]);
        armLCurveRead++;
        if (armLCurveRead >= bezierBufferLenght)
        {
          armLCurveRead = 0;
        }
      }
    }
    else if (csvDisabledFlag)
    {
      armLEnabled = false;//hier is ergens een probleem met 2e bestand inladen
    }
  }

  if (armRBezier.isFinished(currenttime)||ForceRead == true)
  {
    if (armRCurveRead != armRCurveWrite)
    {
      Serial.printf("armR setControllPoints, read: %d, write: %d, forceread: %d\n", armRCurveRead, armRCurveWrite, ForceRead);
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
  if (legLBezier.isFinished(currenttime)||ForceRead == true)
  {
    if (legLCurveRead != legLCurveWrite)
    {
      Serial.printf("legL setControllPoints, read: %d, write: %d, forceread: %d\n", legLCurveRead, legLCurveWrite, ForceRead);
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
  if (legRBezier.isFinished(currenttime)||ForceRead == true)
  {
    if (legRCurveRead != legRCurveWrite)
    {
      Serial.printf("legR setControllPoints, read: %d, write: %d, forceread: %d\n", legRCurveRead, legRCurveWrite, ForceRead);
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
  ForceRead = false;
}

void BottangoPlayer::openCSV()
{
  file = SD.open(currentFileDir,FILE_READ);
  if (!file) {
    debugE("File (%s) open failed",currentFileDir);
    fileReady = false;
  }
  else{
    Serial.println("openCSV else");
    csvDisabledFlag = false;
    fileReady = true;
    ForceRead = true;
    starttime = millis();
  }
}

void BottangoPlayer::closeCSV()
{
  file.close();
}

void BottangoPlayer::readAndParseCSVRow()
{
  Serial.printf("time: %d,", millis());
  unsigned long int timebefore = millis();
  if (cp.parseRow())
  {
  unsigned long int timeafter = millis();
  Serial.printf("read duration: %d\n", timeafter);
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

    //Serial.printf("cmd: %s, id: %s, startMS: %f, durationMS: %f, startPosition: %f, startControlPointX: %f, startControlPointY: %f, endPosition: %f, endControlPointX: %f, endControlPointY: %f\n", commandType[0], motorID[0], startMs[0], durationMs[0], startPosition[0], startControlPointX[0], startControlPointY[0], endPosition[0], endControlPointX[0], endControlPointY[0]);
    Serial.printf("reading csv row, data:\n");
    Serial.printf("0: %f, 1: %f, 2: %f, 3: %f, 4: %f, 5: %f, 6: %f, 7: %f\n", controllArray[0], controllArray[1], controllArray[2], controllArray[3], controllArray[4], controllArray[5], controllArray[6], controllArray[7]);
    
    if (strcmp(commandType[0], "sC") == 0)
    Serial.println("sC");
    {
      if (strcmp(motorID[0], "m_arm_l") == 0)
      {
        Serial.println("Added: m_arm_l\n");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(armLCurveArray[armLCurveWrite]));
        armLCurveWrite++;
        if (armLCurveWrite == bezierBufferLenght)
        {
          armLCurveWrite = 0;
        }
      }
      else if (strcmp(motorID[0], "m_arm_r") == 0)
      {
        Serial.println("Added: m_arm_r");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(armRCurveArray[armRCurveWrite]));
        armRCurveWrite++;
        if (armRCurveWrite == bezierBufferLenght)
        {
          armRCurveWrite = 0;
        }
      }
      else if (strcmp(motorID[0], "m_leg_l") == 0)
      {
        Serial.println("Added: m_leg_l");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(legLCurveArray[legLCurveWrite]));
        legLCurveWrite++;
        if (legLCurveWrite == bezierBufferLenght)
        {
          legLCurveWrite = 0;
        }
      }
      else if (strcmp(motorID[0], "m_leg_r") == 0)
      {
        Serial.println("Added: m_leg_r");
        std::copy(std::begin(controllArray), std::end(controllArray), std::begin(legRCurveArray[legRCurveWrite]));
        legRCurveWrite++;
        if (legRCurveWrite == bezierBufferLenght)
        {
          legRCurveWrite = 0;
        }
      }
      else{
        Serial.printf("motor id not found: ");
        Serial.println(motorID[0]);
      }
    }
  }
  else
  {
    Serial.println("end of file");
    closeCSV();
    fileReady = false;
    enabled = false;
    csvDisabledFlag = true;
  }
}

int BottangoPlayer::sumAllReadWritePointers()
{
  int deltaPointersArmL = armLCurveWrite - armLCurveRead;
  if (deltaPointersArmL < 0)
    deltaPointersArmL += bezierBufferLenght;
  int deltaPointersArmR = armRCurveWrite - armRCurveRead;
  if (deltaPointersArmR < 0)
    deltaPointersArmR += bezierBufferLenght;
  int deltaPointersLegL = legLCurveWrite - legLCurveRead;
  if (deltaPointersLegL < 0)
    deltaPointersLegL += bezierBufferLenght;
  int deltaPointersLegR = legRCurveWrite - legRCurveRead;
  if (deltaPointersLegR < 0)
    deltaPointersLegR += bezierBufferLenght;
  return (deltaPointersArmL + deltaPointersArmR + deltaPointersLegL + deltaPointersLegR);
}

void BottangoPlayer::increaseReadByOne()
{
  ForceRead == true;
}

