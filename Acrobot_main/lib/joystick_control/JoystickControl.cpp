#include "JoystickControl.h"

JoystickControl::JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, Arm &armL, Arm &armR, ChoreoPlayer &choreoPlayer, Menu &menu)
    : Debug(Debug), joystick(joystick), legL(legL), legR(legR), armL(armL), armR(armR), choreoPlayer(choreoPlayer), menu(menu) {}

void JoystickControl::update()
{

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
    modeSummative(95, speedSummativeMode);
    break;
  case MODE_SUMMATIVE_90_FAST:
    modeSummative(95, speedSummativeFastMode);
    break;
  case MODE_SUMMATIVE_140:
    modeSummative(140, speedSummativeMode);
    break;
  case MODE_POSE:
    defaultOn = false;
    submodeCalibrate();
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

void JoystickControl::submodeCalibrate()
{
  if (joystick.getButtonR1())
  {
    if (joystick.getMiscPSPressed())
    {
      armL.startCalibration();
      armR.startCalibration();
    }
  }

  if (joystick.getButtonL1())
  {
    if (joystick.getMiscPSPressed())
    {
      legL.startCalibration();
      legR.startCalibration();
    }
  }
}

void JoystickControl::submodeStop()
{
  if (joystick.getButtonCrossPressed())
  {
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

  if (!joystick.getButtonL1() && !joystick.getButtonR1() && joystick.getL2() == 0 && joystick.getR2() == 0 )
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

void JoystickControl::submodeArmNeutral()
{

  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    if (joystick.getDpadUpPressed())
    {
      armRNeutral = 0;
      armLNeutral = 0;
    }

    if (joystick.getDpadRightPressed())
    {
      armRNeutral = 90;
      armLNeutral = 90;
    }

    if (joystick.getDpadDownPressed())
    {
      armRNeutral = 180;
      armLNeutral = 180;
    }

    if (joystick.getDpadLeftPressed())
    {
      armRNeutral = 270;
      armLNeutral = 270;
    }
  }
}

void JoystickControl::defaultSubmodes()
{
  submodeCalibrate();
  submodeStop();
  submodeStand();
  submodeMenu();
  submodeArmNeutral();
}

void JoystickControl::modeAbsolute(int rotDegrees, float speed)
{

  //legs
  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    choreoPlayer.stop();

    if (joystick.getL2() > 0)
    {
      speed = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, legLNeutral - rotDegrees, legLNeutral + rotDegrees);
    float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, legRNeutral - rotDegrees, legRNeutral + rotDegrees);

    legLTarget = adjustByDisplacement(legLTarget, legLPosJoystick, displacement);
    legRTarget = adjustByDisplacement(legRTarget, legRPosJoystick, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  //arms
  if (joystick.getButtonR1() || joystick.getR2() > 0)
  {
    choreoPlayer.stop();

    if (joystick.getR2() > 0)
    {
      speed = fMap(joystick.getR2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    float armLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, armLNeutral - rotDegrees, armLNeutral + rotDegrees);
    float armRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, armRNeutral - rotDegrees, armRNeutral + rotDegrees);

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
    choreoPlayer.stop();
    float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, -1, 1);

    float displacementL = legLPosJoystick * speedRelativeMode * deltaT;

    legLTarget += displacementL;
    legLTarget = constrain(legLTarget, 40, 320);
    legL.setTarget(legLTarget, menu.getP(), menu.getD());
  }

  if (joystick.getButtonR1())
  {
    choreoPlayer.stop();
    float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, -1, 1);

    float displacementR = legRPosJoystick * speedRelativeMode * deltaT;

    legRTarget += displacementR;
    legRTarget = constrain(legRTarget, 40, 320);
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }
}

void JoystickControl::modeSummative(int rotDegrees, float speed)
{

  // legs
  if (joystick.getButtonR1() || joystick.getR2() > 0)
  {
    choreoPlayer.stop();
    float joyRX = fMap(joystick.getAxisRXCorrected(), -128, 128, -rotDegrees, rotDegrees);
    float joyRY = fMap(joystick.getAxisRYCorrected(), -128, 128, -rotDegrees, rotDegrees);

    float joyLLegTarget = constrain(legLNeutral + joyRY + joyRX, legLNeutral - rotDegrees, legLNeutral + rotDegrees);
    float joyRLegTarget = constrain(legRNeutral + joyRY - joyRX, legRNeutral - rotDegrees, legRNeutral + rotDegrees);

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
    choreoPlayer.stop();
    float joyLX = fMap(joystick.getAxisLXCorrected(), -128, 128, -rotDegrees, rotDegrees);
    float joyLY = fMap(joystick.getAxisLYCorrected(), -128, 128, -rotDegrees, rotDegrees);

    float joyLArmTarget = constrain(armLNeutral + joyLY + joyLX, armLNeutral - rotDegrees, armLNeutral + rotDegrees);
    float joyRArmTarget = constrain(armLNeutral + joyLY - joyLX, armLNeutral - rotDegrees, armLNeutral + rotDegrees);

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

void JoystickControl::modeSynch(int rotDegrees, float speed){

  // R moves both legs equally, L moves both arms equally
    // legs
  if (joystick.getButtonR1() || joystick.getR2() > 0)
  {
    choreoPlayer.stop();
    float joyR = fMap(joystick.getAxisRYCorrected(), -128, 128, legLNeutral - rotDegrees, legLNeutral + rotDegrees);


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
    choreoPlayer.stop();
    float joyL = fMap(joystick.getAxisLYCorrected(), -128, 128, armLNeutral - rotDegrees, armLNeutral + rotDegrees);

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

void JoystickControl::modeTelepresence(){

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

float JoystickControl::calcTelepresenceTorque(float angleDiff, float velDiff, float localVel) {
  float torque = teleKp * angleDiff + teleKd * velDiff - teleK * localVel;
  return torque;
}