#ifndef JOYSTICK_CONTROL_H
#define JOYSTICK_CONTROL_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include "Joystick.h"
#include "Leg.h"
#include "ChoreoPlayer.h"
#include "menu.h"

enum JoystickControlMode
{
  MODE_NONE,
  MODE_LEGS_ABSOLUTE_90_LIMITED,
  MODE_LEGS_ABSOLUTE_90_UNLIMITED,
  MODE_LEGS_ABSOLUTE_40_LIMITED,
  MODE_LEGS_RELATIVE
};

// TODO:
// Update runs the relevant sub-function selected in the joystick control mode. The absolute method has a limited speed. The moment control is taken over by the joystick, the current position is registered and from there on it's only allowed to be changed by a certain amount per update delta time.

class JoystickControl
{
public:
  JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, ChoreoPlayer &choreoPlayer, Menu &menu);
  void update();

  void setMode(JoystickControlMode mode);

private:
  RemoteDebug &Debug;
  Joystick &joystick;
  Leg &legL;
  Leg &legR;
  ChoreoPlayer &choreoPlayer;
  JoystickControlMode controlMode = MODE_LEGS_ABSOLUTE_90_LIMITED;
  Menu &menu;

  long prevUpdateTime = 0;
  int deltaT = 0;

  float legRTarget = 180;
  float legLTarget = 180;

  float speedAbsoluteMode = .4;

  // these are partial modes, that can be overlayed? How to call this
  void submodeCalibrate();
  void submodeStop();
  void submodeMenu();

  void modeLegsAbsolute(int limitLow, int limitHigh, float speed);
  void modeLegsRelative();
};

#endif // JOYSTICK_CONTROL_H
