#include "JoystickControl.h"

JoystickControl::JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, ChoreoPlayer &choreoPlayer, Menu &menu)
    : Debug(Debug), joystick(joystick), legL(legL), legR(legR), choreoPlayer(choreoPlayer), menu(menu) {}

void JoystickControl::update()
{

  deltaT = millis() - prevUpdateTime;

  submodeCalibrate();
  submodeStop();

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
    choreoPlayer.stop();
  }
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
  // Implement the relative legs control logic here.
  // You can use the joystick input to calculate the relative movement of the legs.
  // Use legRTarget and legLTarget to set the target positions.
  // Ensure that the movement does not exceed a certain speed (use the 'speed' variable).
}
