#include "JoystickControl.h"

JoystickControl::JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, Arm &armL, Arm &armR, ChoreoPlayer &choreoPlayer, Menu &menu, EStop &eStop, MovePlayer &movePlayer, Sequencer &sequencer, BottangoSocket &bottangoSocket)
    : Debug(Debug), joystick(joystick), legL(legL), legR(legR), armL(armL), armR(armR), choreoPlayer(choreoPlayer), menu(menu), eStop(eStop), movePlayer(movePlayer), sequencer(sequencer), bottangoSocket(bottangoSocket) {}

void JoystickControl::update()
{

  bool justConnected = joystick.getConnected();

  // new connection
  if (justConnected && !joystickConnected)
  {
    joystickConnected = true;
    setLedValues();
  }

  // not connected
  if (!justConnected)
  {
    joystickConnected = false;
    // return; // needs to be turned off for mannequin to work
  }

  deltaT = millis() - prevUpdateTime;

  bool defaultOn = true;

  switch (controlMode)
  {
  case MODE_NONE:
    break;
  case MODE_SUMMATIVE_90:
    modeSummative(variableAngle[variableSetting], speedSummativeMode);
    break;
  case MODE_MANNEQUIN:
    modeMannequin();
    break;

  case MODE_TELEPRESENCE:
    modeTelepresence();
    break;

  default:
    break;
  }

  if (defaultOn)
  {
    defaultSubmodes();
  }

  prevUpdateTime = millis();
}

void JoystickControl::setMode(JoystickControlMode mode)
{
  controlMode = mode;
  choreoPlayer.stop();
  movePlayer.stop();
  sequencer.stop();
  bottangoSocket.stop();
}

void JoystickControl::submodeStop()
{
  if (joystick.getMiscPSPressed())
  {
    eStop.set();
    legR.stop();
    legL.stop();
    legLTarget = 180;
    legRTarget = 180;
    armR.stop();
    armL.stop();
    choreoPlayer.stop();
    movePlayer.stop();
    sequencer.stop();
    bottangoSocket.stop();
  }
}

void JoystickControl::submodeStand()
{
  if (joystick.getButtonSquarePressed())
  {
    choreoPlayer.start(CHOREO_STANDING);
    legLTarget = 180;
    legRTarget = 180;
  }
}

void JoystickControl::submodeMenu()
{

  if (!joystick.getButtonL1() && !joystick.getButtonR1() && joystick.getL2() == 0 && joystick.getR2() == 0)
  {
    if (joystick.getDpadUpPressed())
    {
      menu.pressUp();
    }

    if (joystick.getDpadDownPressed())
    {
      menu.pressDown();
    }

    if (joystick.getDpadLeftPressed())
    {
      menu.pressLeft();
    }

    if (joystick.getDpadRightPressed())
    {
      menu.pressRight();
    }
  }
}

void JoystickControl::submodeMenuOption()
{
  if (joystick.getMiscOption())
  {
    submodeMenu();
  }
}


void JoystickControl::submodeArmNeutralJoystick()
{
  if (joystick.getButtonL3Pressed())
  {
    if (joystick.getAxisLYCorrected() > 10)
    {
      if (armNeutral < 430)
      {
        armNeutral += 90;
        joystick.setRumble(64, 10);
      }
    }
    if (joystick.getAxisLYCorrected() < -10)
    {
      if (armNeutral > -90)
      {
        armNeutral -= 90;
        joystick.setRumble(64, 10);
      }
    }
  }
  if (joystick.getButtonR3Pressed())
  {
    if (joystick.getAxisRYCorrected() > 10)
    {
      if (legNeutral < 200)
      {
        legNeutral += 90;
        joystick.setRumble(64, 10);
      }
    }
    if (joystick.getAxisRYCorrected() < -10)
    {
      if (legNeutral > 160)
      {
        legNeutral -= 90;
        joystick.setRumble(64, 10);
      }
    }
  }
}

void JoystickControl::defaultSubmodes()
{
  submodeStop();
  submodeStand();
  submodeMenu();
  submodeArmNeutralJoystick();
  submodeStopChoreo();
  submodeToggleSynch();
  submodeChangeVariableAngle();
  // submodeCurrentRumble();
  submodePoseButtons();
  // submodeAGTButtons();
}

void JoystickControl::submodePoseButtons(){
  if (joystick.getButtonTrianglePressed())
  {
    movePlayer.startMove("/pose_handstand.csv");
  }
  if (joystick.getButtonCirclePressed())
  {
    movePlayer.startMove("/pose_stand.csv");
  }

  if (joystick.getButtonSquarePressed())
  {
    movePlayer.startMove("/pose_sit.csv", false, false, 20, 2, 60);
  }

}

void JoystickControl::submodeAGTButtons(){
  if (joystick.getButtonTrianglePressed())
  {
    movePlayer.startMove("/greetings_wave.csv", false, false, 20, 2, 75);
  }
  if (joystick.getButtonCirclePressed())
  {
    movePlayer.startMove("/walk_normal.csv", false, true);
  }

  if (joystick.getButtonSquarePressed())
  {
    movePlayer.startMove("/pose_sit.csv", false, false, 20, 2, 60);
  }

  if (joystick.getButtonCrossPressed())
  {
    movePlayer.startMove("/pose_stand.csv");
  }

}

void JoystickControl::submodeCurrentRumble()
{
  static long unsigned int lastRumble = 0;
  if (millis() - lastRumble > 50)
  {
    lastRumble = millis();
    int legCurrentThreshold = 5;
    if (joystick.getButtonR1() || joystick.getR2() > 0) // benen
    {
      if (abs(legL.getTorque()) >= legCurrentThreshold || abs(legR.getTorque()) >= legCurrentThreshold)
      {
        joystick.setRumble(255, 150);
      }
    }
    int armCurrentThreshold = 4;
    if (joystick.getButtonL1() || joystick.getL2() > 0) // armen
    {
      if (abs(armL.getTorque()) >= armCurrentThreshold || abs(armR.getTorque()) >= armCurrentThreshold)
      {
        joystick.setRumble(255, 150);
      }
    }
  }
}

void JoystickControl::submodeStopChoreo()
{
  const int STOP_TRESHOLD = 0;
  if (joystick.getButtonCross() ||  joystick.getButtonL1() || joystick.getL2() > STOP_TRESHOLD || joystick.getButtonR1() || joystick.getR2() > STOP_TRESHOLD) 
  {
    choreoPlayer.stop();
    movePlayer.stop();
    sequencer.stop();
    bottangoSocket.stop();
  }
}

void JoystickControl::submodeToggleSynch()
{
  if (joystick.getMiscOptionPressed())
  {
    synchronized = !synchronized;
    setLedValues();
  }
}

void JoystickControl::submodeChangeVariableAngle()
{
  if (joystick.getMiscCreatePressed())
  {
    variableSetting++;
    variableSetting %= 3;
    setLedValues();
  }
}

// void JoystickControl::modeAbsolute(int rotDegrees, float speed)
// {

//   // legs
//   if (joystick.getButtonL1() || joystick.getL2() > 0)
//   {

//     if (joystick.getL2() > 0)
//     {
//       speed = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
//     }

//     float displacement = speed * deltaT;

//     float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, legNeutral - rotDegrees, legNeutral + rotDegrees);
//     float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, legNeutral - rotDegrees, legNeutral + rotDegrees);

//     legLTarget = adjustByDisplacement(legLTarget, legLPosJoystick, displacement);
//     legRTarget = adjustByDisplacement(legRTarget, legRPosJoystick, displacement);

//     legL.setTarget(legLTarget, menu.getP(), menu.getD());
//     legR.setTarget(legRTarget, menu.getP(), menu.getD());
//   }

//   // arms
//   if (joystick.getButtonR1() || joystick.getR2() > 0)
//   {

//     if (joystick.getR2() > 0)
//     {
//       speed = fMap(joystick.getR2(), 0, 1020, 0, speedTriggerMax);
//     }

//     float displacement = speed * deltaT;

//     float armLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, armNeutral - rotDegrees, armNeutral + rotDegrees);
//     float armRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, armNeutral - rotDegrees, armNeutral + rotDegrees);

//     armLTarget = adjustByDisplacement(armLTarget, armLPosJoystick, displacement);
//     armRTarget = adjustByDisplacement(armRTarget, armRPosJoystick, displacement);

//     armL.setTarget(armLTarget, menu.getP(), menu.getD());
//     armR.setTarget(armRTarget, menu.getP(), menu.getD());
//   }
// }

// void JoystickControl::modeLegsRelative()
// {
//   if (joystick.getButtonL1()) // could be replaced with joystick being in center?
//   {
//     float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, -1, 1);

//     float displacementL = legLPosJoystick * speedRelativeMode * deltaT;

//     legLTarget += displacementL;
//     legLTarget = constrain(legLTarget, 40, 320);
//     legL.setTarget(legLTarget, menu.getP(), menu.getD());
//   }

//   if (joystick.getButtonR1())
//   {
//     float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, -1, 1);

//     float displacementR = legRPosJoystick * speedRelativeMode * deltaT;

//     legRTarget += displacementR;
//     legRTarget = constrain(legRTarget, 40, 320);
//     legR.setTarget(legRTarget, menu.getP(), menu.getD());
//   }
// }

void JoystickControl::modeSummative(int rotDegrees, float speed)
{
  // if synch, do that stuff instead
  if (synchronized)
  {
    modeSynch(rotDegrees, speed);
    return;
  }

  // legs
  if (joystick.getButtonR1() || joystick.getR2() > 0)
  {
    float joyRX = fMap(joystick.getAxisRXCorrected(), -128, 128, -rotDegrees, rotDegrees);
    float joyRY = fMap(joystick.getAxisRYCorrected(), -128, 128, -rotDegrees, rotDegrees);

    float joyLLegTarget = constrain(legNeutral + joyRY + joyRX, legNeutral - rotDegrees, legNeutral + rotDegrees);
    float joyRLegTarget = constrain(legNeutral + joyRY - joyRX, legNeutral - rotDegrees, legNeutral + rotDegrees);

    if (joystick.getR2() > 0)
    {
      speed = fMap(joystick.getR2() * joystick.getR2(), 0, 1020 * 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    legLTarget = adjustByDisplacement(legL.getTarget(), joyLLegTarget, displacement);
    legRTarget = adjustByDisplacement(legR.getTarget(), joyRLegTarget, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  // arms
  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    float joyLX = fMap(joystick.getAxisLXCorrected(), -128, 128, -rotDegrees, rotDegrees);
    float joyLY = fMap(joystick.getAxisLYCorrected(), -128, 128, -rotDegrees, rotDegrees);

    float joyLArmTarget = constrain(armNeutral + joyLY + joyLX, armNeutral - rotDegrees, armNeutral + rotDegrees);
    float joyRArmTarget = constrain(armNeutral + joyLY - joyLX, armNeutral - rotDegrees, armNeutral + rotDegrees);

    if (joystick.getL2() > 0)
    {
      speed = fMap(joystick.getL2() * joystick.getL2(), 0, 1020 * 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    armLTarget = adjustByDisplacement(armL.getTarget(), joyLArmTarget, displacement);
    armRTarget = adjustByDisplacement(armR.getTarget(), joyRArmTarget, displacement);

    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }
}

void JoystickControl::modeSynch(int rotDegrees, float speed)
{

  // R moves both legs equally, L moves both arms equally
  // legs
  if (joystick.getButtonR1() || joystick.getR2() > 0)
  {
    float joyR = fMap(joystick.getAxisRYCorrected(), -128, 128, legNeutral - rotDegrees, legNeutral + rotDegrees);

    if (joystick.getR2() > 0)
    {
      speed = fMap(joystick.getR2() * joystick.getR2(), 0, 1020 * 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    legLTarget = adjustByDisplacement(legL.getTarget(), joyR, displacement);
    legRTarget = adjustByDisplacement(legR.getTarget(), joyR, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  // arms
  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    float joyL = fMap(joystick.getAxisLYCorrected(), -128, 128, armNeutral - rotDegrees, armNeutral + rotDegrees);

    if (joystick.getL2() > 0)
    {
      speed = fMap(joystick.getL2() * joystick.getL2(), 0, 1020 * 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    armLTarget = adjustByDisplacement(armL.getTarget(), joyL, displacement);
    armRTarget = adjustByDisplacement(armR.getTarget(), joyL, displacement);

    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }
}



void JoystickControl::modeTelepresence()
{

  // Variables to store joint angles and angular velocities for arms L and R
  float angleL = armL.getPosition();
  float angleR = armR.getPosition();
  float velL = armL.getVelocity();
  float velR = armR.getVelocity();

  // Calculate the control signal (torque) for arm L
  float angleDiffL = angleR - angleL;
  float velDiffL = velR - velL;
  float torqueL = calcTelepresenceTorque(angleDiffL, velDiffL, velL);

  // Calculate the control signal (torque) for arm 2
  float angleDiffR = angleL - angleR;
  float velDiffR = velL - velR;
  float torqueR = calcTelepresenceTorque(angleDiffR, velDiffR, velR);

  armL.setTorqueUnprotected(torqueL);
  armR.setTorqueUnprotected(torqueR);
}

void JoystickControl::modeMannequin(){
  if (!movePlayer.isIdle()){
    return;
  }
  float torqArmL = abs(armL.getTorque());
  float torqArmR = abs(armR.getTorque());
  float torqLegL = abs(legL.getTorque());
  float torqLegR = abs(legR.getTorque());

  if (torqArmL > 2){
    float diff = armL.getPosition() - armL.getTarget() ;
    armL.setTarget(armL.getTarget() + diff * 0.1, menu.getP(), menu.getD());
  }
  if (torqArmR > 2){
    float diff = armR.getPosition() - armR.getTarget() ;
    armR.setTarget(armR.getTarget() + diff * 0.1, menu.getP(), menu.getD());
  }

  if (torqLegL > 3){
    float diff = legL.getPosition() - legL.getTarget() ;
    legL.setTarget(legL.getTarget() + diff * 0.1, menu.getP(), menu.getD());
  }
  if (torqLegR > 3){
    float diff = legR.getPosition() - legR.getTarget() ;
    legR.setTarget(legR.getTarget() + diff * 0.1, menu.getP(), menu.getD());
  }
}

float JoystickControl::adjustByDisplacement(float currentVal, float target, float displacement)
{

  float newVal = currentVal;

  if (abs(target - currentVal) < displacement)
  {
    newVal = target;
  }
  else
  {
    if (currentVal > target)
    {
      newVal -= displacement;
    }
    else if (currentVal < target)
    {
      newVal += displacement;
    }
  }

  return newVal;
}

float JoystickControl::calcTelepresenceTorque(float angleDiff, float velDiff, float localVel)
{
  float torque = teleKp * angleDiff + teleKd * velDiff - teleK * localVel;
  return torque;
}

void JoystickControl::setLedValues()
{
  if (synchronized)
  {
    ledB = 255;
  }
  else
  {
    ledB = 0;
  }

  if (variableSetting == 0)
  {
    ledG = 0;
  }
  else if (variableSetting == 1)
  {
    ledG = 70;
  }
  else
  {
    ledG = 255;
  }

  joystick.setColorLED(ledR, ledG, ledB);
}
