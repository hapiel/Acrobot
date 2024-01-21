#include "JoystickControl.h"

JoystickControl::JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, Arm &armL, Arm &armR, ChoreoPlayer &choreoPlayer, Menu &menu, EStop &eStop)
    : Debug(Debug), joystick(joystick), legL(legL), legR(legR), armL(armL), armR(armR), choreoPlayer(choreoPlayer), menu(menu), eStop(eStop) {}



void JoystickControl::update()
{

  bool justConnected = joystick.getConnected();

  // new connection
  if (justConnected && !joystickConnected)
  {
    joystickConnected = true;
    debugI("Joystick connected");
    setLedValues();
  }

  // not connected
  if (!justConnected)
  {
    joystickConnected = false;
    return;
  }

  setLedValues();

  deltaT = millis() - prevUpdateTime;

  bool defaultOn = true;

  switch (controlMode)
  {
  case MODE_NONE:

    break;
  case MODE_ABSOLUTE_90_LIMITED:
    modeAbsolute(90, speedAbsoluteMode);
    break;
  case MODE_ABSOLUTE_90_UNLIMITED:
    modeAbsolute(90, speedUnlimitedMode);
    break;
  case MODE_ABSOLUTE_140_LIMITED:
    modeAbsolute(140, speedAbsoluteMode);
    break;
  case MODE_ABSOLUTE_20_LIMITED:
    modeAbsolute(20, speedAbsoluteMode);
    break;
  case MODE_LEGS_RELATIVE:
    modeLegsRelative();
    break;
  case MODE_SUMMATIVE_90:
    modeSummative(variableAngle[variableSetting], speedSummativeMode);
    break;
  case MODE_SUMMATIVE_90_FAST:
    modeSummative(95, speedSummativeFastMode);
    break;
  case MODE_SUMMATIVE_140:
    modeSummative(140, speedSummativeMode);
    break;
  case MODE_POSE:
    defaultOn = false;
    submodeStop();
    submodeMenuOption();
    modePose();
    break;
  case MODE_TELEPRESENCE:
    modeTelepresence();
    break;
  case MODE_SYNCH_140:
    modeSynch(140, speedSummativeMode);
    break;
  case MODE_SYNCH_90:
    modeSynch(95, speedSummativeMode);
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

void JoystickControl::submodeArmNeutralDpad()
{

  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    if (joystick.getDpadUpPressed())
    {
      armNeutral = 0;
    }

    if (joystick.getDpadRightPressed())
    {
      armNeutral = 90;
    }

    if (joystick.getDpadDownPressed())
    {
      armNeutral = 180;
    }

    if (joystick.getDpadLeftPressed())
    {
      armNeutral = 270;
    }
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
      }
    }
    if (joystick.getAxisLYCorrected() < -10)
    {
      if (armNeutral > -90)
      {
        armNeutral -= 90;
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
      }
    }
    if (joystick.getAxisRYCorrected() < -10)
    {
      if (legNeutral > 160)
      {
        legNeutral -= 90;
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
  // submodeTestPositions();
  submodeToggleSynch();
  submodeChangeVariableAngle();
}

void JoystickControl::submodeTestPositions()
{
  if (joystick.getButtonTriangle())
  {
    armLTarget = 180;
    armRTarget = 180;
    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }

  if (joystick.getButtonCircle())
  {
    armLTarget = 90;
    armRTarget = 90;
    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }

  if (joystick.getButtonSquare())
  {
    armLTarget = 199;
    armRTarget = 199;
    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }
}

void JoystickControl::submodeStopChoreo()
{
  const int STOP_TRESHOLD = 0;
  if (joystick.getButtonCross() || joystick.getButtonL1() || joystick.getL2() > STOP_TRESHOLD || joystick.getButtonR1() || joystick.getR2() > STOP_TRESHOLD)
  {
    choreoPlayer.stop();
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

void JoystickControl::modeAbsolute(int rotDegrees, float speed)
{

  // legs
  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {

    if (joystick.getL2() > 0)
    {
      speed = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, legNeutral - rotDegrees, legNeutral + rotDegrees);
    float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, legNeutral - rotDegrees, legNeutral + rotDegrees);

    legLTarget = adjustByDisplacement(legLTarget, legLPosJoystick, displacement);
    legRTarget = adjustByDisplacement(legRTarget, legRPosJoystick, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  // arms
  if (joystick.getButtonR1() || joystick.getR2() > 0)
  {

    if (joystick.getR2() > 0)
    {
      speed = fMap(joystick.getR2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    float armLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, armNeutral - rotDegrees, armNeutral + rotDegrees);
    float armRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, armNeutral - rotDegrees, armNeutral + rotDegrees);

    armLTarget = adjustByDisplacement(armLTarget, armLPosJoystick, displacement);
    armRTarget = adjustByDisplacement(armRTarget, armRPosJoystick, displacement);

    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }
}

void JoystickControl::modeLegsRelative()
{
  if (joystick.getButtonL1()) // could be replaced with joystick being in center?
  {
    float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, -1, 1);

    float displacementL = legLPosJoystick * speedRelativeMode * deltaT;

    legLTarget += displacementL;
    legLTarget = constrain(legLTarget, 40, 320);
    legL.setTarget(legLTarget, menu.getP(), menu.getD());
  }

  if (joystick.getButtonR1())
  {
    float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, -1, 1);

    float displacementR = legRPosJoystick * speedRelativeMode * deltaT;

    legRTarget += displacementR;
    legRTarget = constrain(legRTarget, 40, 320);
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }
}

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
      speed = fMap(joystick.getR2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    legLTarget = adjustByDisplacement(legLTarget, joyLLegTarget, displacement);
    legRTarget = adjustByDisplacement(legRTarget, joyRLegTarget, displacement);

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
      speed = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    armLTarget = adjustByDisplacement(armLTarget, joyLArmTarget, displacement);
    armRTarget = adjustByDisplacement(armRTarget, joyRArmTarget, displacement);

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
      speed = fMap(joystick.getR2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    legLTarget = adjustByDisplacement(legLTarget, joyR, displacement);
    legRTarget = adjustByDisplacement(legRTarget, joyR, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  // arms
  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    float joyL = fMap(joystick.getAxisLYCorrected(), -128, 128, armNeutral - rotDegrees, armNeutral + rotDegrees);

    if (joystick.getL2() > 0)
    {
      speed = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    armLTarget = adjustByDisplacement(armLTarget, joyL, displacement);
    armRTarget = adjustByDisplacement(armRTarget, joyL, displacement);

    armL.setTarget(armLTarget, menu.getP(), menu.getD());
    armR.setTarget(armRTarget, menu.getP(), menu.getD());
  }
}

void JoystickControl::modePose()
{
  static int poseLTarget = 180;
  static int poseRTarget = 180;

  float speedL = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
  float speedR = fMap(joystick.getR2(), 0, 1020, 0, speedTriggerMax);

  // stand
  if (joystick.getButtonSquarePressed())
  {
    poseLTarget = 187;
    poseRTarget = 187;
  }

  // 180
  if (joystick.getButtonTrianglePressed())
  {
    poseLTarget = 180;
    poseRTarget = 180;
  }

  // sit
  if (joystick.getButtonCirclePressed())
  {
    poseLTarget = 90;
    poseRTarget = 90;
  }

  // stepR
  if (joystick.getDpadRightPressed())
  {
    poseRTarget = 90;
    poseLTarget = 270;
  }

  // stepL
  if (joystick.getDpadLeftPressed())
  {
    poseRTarget = 270;
    poseLTarget = 90;
  }

  // kickR
  if (joystick.getDpadUpPressed())
  {
    poseRTarget = 90;
    poseLTarget = 190;
  }

  // kickL
  if (joystick.getDpadDownPressed())
  {
    poseRTarget = 190;
    poseLTarget = 90;
  }

  if (joystick.getL2() > 0)
  {
    legLTarget = adjustByDisplacement(legLTarget, poseLTarget, speedL * deltaT);
  }

  if (joystick.getR2() > 0)
  {
    legRTarget = adjustByDisplacement(legRTarget, poseRTarget, speedR * deltaT);
  }

  legL.setTarget(legLTarget, menu.getP(), menu.getD());
  legR.setTarget(legRTarget, menu.getP(), menu.getD());
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
