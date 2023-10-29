#include "BottangoPlayer.h"

BottangoPlayer::BottangoPlayer(RemoteDebug &Debug, Leg &legL, Leg &legR, Arm &armL, Arm &armR) : Debug(Debug), legL(legL), legR(legR), armL(armL), armR(armR)
{
}
void BottangoPlayer::update()
{
  currenttime = millis()-starttime;
  if(currenttime >= 733){
    enabled = 0;
  }
  if(enabled == 1){
  legL.setTarget(mapBeziervalue(legLBezier.getValue(currenttime)), kp, ki);
  legR.setTarget(mapBeziervalue(legRBezier.getValue(currenttime)), kp, ki);
  armL.setTarget(mapBeziervalue(armLBezier.getValue(currenttime)), kp, ki);
  armR.setTarget(mapBeziervalue(armRBezier.getValue(currenttime)), kp, ki);
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

float BottangoPlayer::mapBeziervalue(float value)
{
  return fMap(value, 0.0, 8128.0, -180.0, 540);
}


// POSES