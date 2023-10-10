#ifndef JOYSTICK_CONTROL_H
#define JOYSTICK_CONTROL_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include "Joystick.h"
#include "Leg.h"

enum JoystickControlMode
{
  NONE,
  LEGS_ABSOLUTE,
  LEGS_RELATIVE
};

// TODO:
// Update runs the relevant sub-function selected in the joystick control mode. The absolute method has a limited speed. The moment control is taken over by the joystick, the current position is registered and from there on it's only allowed to be changed by a certain amount per update delta time. 


class JoystickControl
{
public:
  JoystickControl(RemoteDebug& Debug, Joystick &joystick, Leg &legL, Leg &legR);
  void init();
  void update();


private:

  void legsAbsolute();
  void legsRelative();


  RemoteDebug& Debug;
  Joystick &joystick;
  Leg &legL;
  Leg &legR;

};

#endif // JOYSTICK_CONTROL_H
