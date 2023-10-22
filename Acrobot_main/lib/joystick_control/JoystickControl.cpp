#include "JoystickControl.h"

JoystickControl::JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, ChoreoPlayer &choreoPlayer, Menu &menu)
    : Debug(Debug), joystick(joystick), legL(legL), legR(legR), choreoPlayer(choreoPlayer), menu(menu) {}

void JoystickControl::update()
{

  deltaT = millis() - prevUpdateTime;

  defaultSubmodes();

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
  default:
    break;
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

    if (abs(legRPosJoystick - legRTarget) < displacement)
    {
      legRTarget = legRPosJoystick;
    }
    else
    {
      if (legRPosJoystick > legRTarget)
      {
        legRTarget += displacement;
      }
      else if (legRPosJoystick < legRTarget)
      {
        legRTarget -= displacement;
      }
    }
    legR.setTarget(legRTarget, menu.getP(), menu.getD());
  }

  if (joystick.getButtonL1())
  {
    choreoPlayer.stop();

    float displacement = speed * deltaT;

    float legLPosJoystick = fMap(joystick.getAxisLYCorrected(), -128, 128, limitLow, limitHigh);

    if (abs(legLPosJoystick - legLTarget) < displacement)
    {
      legLTarget = legLPosJoystick;
    }
    else
    {
      if (legLPosJoystick > legLTarget)
      {
        legLTarget += displacement;
      }
      else if (legLPosJoystick < legLTarget)
      {
        legLTarget -= displacement;
      }
    }
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
