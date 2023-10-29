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
  legL.setTarget(legLBezier.getValue(currenttime), kp, ki);
  legR.setTarget(legRBezier.getValue(currenttime), kp, ki);
  armL.setTarget(armLBezier.getValue(currenttime), kp, ki);
  armR.setTarget(armRBezier.getValue(currenttime), kp, ki);
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

// POSES