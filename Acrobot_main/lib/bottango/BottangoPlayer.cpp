#include "BottangoPlayer.h"



BottangoPlayer::BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR, SdExFat &sd) : Debug(Debug), legL(legL), legR(legR), armL(armL), armR(armR), sd(sd)
{
}

void BottangoPlayer::update()
{
  currenttime = millis()-starttime;
  checkEndOfCurve();
  if(armLEnabled == 1)
  {
    armL.setTarget(armLBezier.getValue(currenttime), kp, ki);
  }
  if(armREnabled == 1)
  {
    armR.setTarget(armRBezier.getValue(currenttime), kp, ki);
  }
  if(legLEnabled == 1)
  {
    legL.setTarget(legLBezier.getValue(currenttime), kp, ki);
  }
  if(legREnabled == 1)
  {
    legR.setTarget(legRBezier.getValue(currenttime), kp, ki);
  }
}

void BottangoPlayer::start()
{
  starttime = millis();
  enabled = 1;
}

void BottangoPlayer::stop()
{
  enabled = 0;
}
void BottangoPlayer::checkEndOfCurve()
{
  if (!armLBezier.isInProgress(currenttime))
  {
    armLCurveRead++;
    if(armLCurveRead >= armRCurveLastMove)
    {
      armLEnabled = 0;
    }
    if(armLCurveRead >= bezierBufferLenght){
      armLCurveRead = 0;
    }
    armLBezier.setControllPoints(armLCurveArray[armLCurveRead]);
  }

  if (!armRBezier.isInProgress(currenttime))
  {
    armRCurveRead++;
    if(armRCurveRead >= armRCurveLastMove)
    {
      armREnabled = 0;
    }
    if(armRCurveRead >= bezierBufferLenght){
      armRCurveRead = 0;
    }
    armRBezier.setControllPoints(armRCurveArray[armRCurveRead]);
  }

  if (!legLBezier.isInProgress(currenttime))
  {
    legLCurveRead++;
    if(legLCurveRead >= legLCurveLastMove)
    {
      legLEnabled = 0;
    }
    if(legLCurveRead >= bezierBufferLenght){
      legLCurveRead = 0;
    }
    legLBezier.setControllPoints(legLCurveArray[legLCurveRead]);
  }

  if (!legRBezier.isInProgress(currenttime))
  {
    legRCurveRead++;
    if(legRCurveRead >= legRCurveLastMove)
    {
      legREnabled = 0;
    }
    if(legRCurveRead >= bezierBufferLenght){
      legRCurveRead = 0;
    }
    legRBezier.setControllPoints(legRCurveArray[legRCurveRead]);
  }
}
void BottangoPlayer::loadFileAndPlay(char csvDir [256])
{
  armLCurveRead = 0;
  armRCurveRead = 0;
  legLCurveRead = 0;
  legRCurveRead = 0;
  armLCurveWrite = 0;
  armRCurveWrite = 0;
  legLCurveWrite = 0;
  legRCurveWrite = 0;
  
  char csvLine[128];
  ExFile csv;
  csv = sd.open(csvDir,FILE_READ);
  while(csv.available())
  {
    int n = csv.fgets(csvLine,sizeof(csvLine));
    if (n <= 0) {
      //error("fgets failed");
    }
    if (csvLine[n-1] != '\n' && n == (sizeof(csvLine) - 1)) {
      //error("line too long");
    }
    //if (!parseLine(line)) {
      //error("parseLine failed");
    const char s[2] = ",";
    char *token;

    token = strtok(csvLine, s);
    int i = 0;
    char splitLine[10][16];
    while(token !=NULL)
    {
        splitLine[i] = token;  
    }
      token
  } 
}



// POSESwhile