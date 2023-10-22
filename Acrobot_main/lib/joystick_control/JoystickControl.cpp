#include "JoystickControl.h"

JoystickControl::JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, ChoreoPlayer &choreoPlayer, Menu &menu)
    : Debug(Debug), joystick(joystick), legL(legL), legR(legR), choreoPlayer(choreoPlayer), menu(menu) {}

void JoystickControl::update()
{

  deltaT = millis() - prevUpdateTime;

  bool defaultOn = true;

  switch (controlMode)
  {
  case MODE_NONE:

    break;
  case MODE_LEGS_ABSOLUTE_90_LIMITED:
    modeLegsAbsolute(90, 270, speedAbsoluteMode);
    break;
  case MODE_LEGS_ABSOLUTE_90_UNLIMITED:
    modeLegsAbsolute(90, 270, 5);
    break;
  case MODE_LEGS_ABSOLUTE_40_LIMITED:
    modeLegsAbsolute(40, 320, speedAbsoluteMode);
    break;
  case MODE_LEGS_RELATIVE:
    modeLegsRelative();
    break;
  case MODE_SUMMATIVE:
    modeSummative();
    break;
  case MODE_POSE:
    defaultOn = false;
    submodeCalibrate();
    submodeStop();
    submodeMenuOption();
    modePose();
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
      legR.startCalibration();
    }
  }

  if (joystick.getButtonL1())
  {
    if (joystick.getMiscPSPressed())
    {
      legL.startCalibration();
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

void JoystickControl::submodeMenuOption()
{
  if (joystick.getMiscOption())
  {
    submodeMenu();
  }
}

void JoystickControl::defaultSubmodes()
{
  submodeCalibrate();
  submodeStop();
  submodeStand();
  submodeMenu();
}

void JoystickControl::modeLegsAbsolute(int limitLow, int limitHigh, float speed)
{

  if (joystick.getButtonR1())
  {
    choreoPlayer.stop();

    float displacement = speed * deltaT;

    float legRPosJoystick = fMap(joystick.getAxisRYCorrected(), -128, 128, limitLow, limitHigh);

    legRTarget = adjustByDisplacement(legRTarget, legRPosJoystick, displacement);

    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  if (joystick.getButtonL1())
  {
    choreoPlayer.stop();

    float displacement = speed * deltaT;

    float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, limitLow, limitHigh);

    legLTarget = adjustByDisplacement(legLTarget, legLPosJoystick, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
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

void JoystickControl::modeSummative()
{

  if (joystick.getButtonL1() || joystick.getL2() > 0)
  {
    choreoPlayer.stop();
    float joyX = fMap(joystick.getAxisLXCorrected(), -128, 128, -90, 90);
    float joyY = fMap(joystick.getAxisLYCorrected(), -128, 128, -90, 90);

    float joyRTarget = constrain(180 + joyY - joyX, 90, 270);
    float joyLTarget = constrain(180 + joyY + joyX, 90, 270);

    float speed = speedSummativeMode;

    if (joystick.getL2() > 0)
    {
      speed = fMap(joystick.getL2(), 0, 1020, 0, speedTriggerMax);
    }

    float displacement = speed * deltaT;

    legLTarget = adjustByDisplacement(legLTarget, joyLTarget, displacement);
    legRTarget = adjustByDisplacement(legRTarget, joyRTarget, displacement);

    legL.setTarget(legLTarget, menu.getP(), menu.getD());
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
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

float JoystickControl::adjustByDisplacement(float currentVal, float target, float displacement)
{

  float newVal = currentVal;

  if (abs(target - currentVal) < displacement)
  {
    newVal = currentVal;
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