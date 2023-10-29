#ifndef JOYSTICK_CONTROL_H
#define JOYSTICK_CONTROL_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include "Joystick.h"
#include "Leg.h"
#include "Arm.h"
#include "ChoreoPlayer.h"
#include "menu.h"

enum JoystickControlMode
{
  MODE_NONE,
  MODE_ABSOLUTE_90_LIMITED,
  MODE_ABSOLUTE_90_UNLIMITED,
  MODE_ABSOLUTE_140_LIMITED,
  MODE_ABSOLUTE_20_LIMITED,
  MODE_LEGS_RELATIVE,
  MODE_SUMMATIVE_90,
  MODE_SUMMATIVE_90_FAST,
  MODE_SUMMATIVE_140,
  MODE_POSE,
  MODE_TELEPRESENCE
};

class JoystickControl
{
public:
  JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, Arm &armL, Arm &armR, ChoreoPlayer &choreoPlayer, Menu &menu);
  void update();

  void setMode(JoystickControlMode mode);

private:
  RemoteDebug &Debug;
  Joystick &joystick;
  Leg &legL;
  Leg &legR;
  Arm &armL;
  Arm &armR;
  ChoreoPlayer &choreoPlayer;
  JoystickControlMode controlMode = MODE_ABSOLUTE_90_LIMITED;
  Menu &menu;

  long prevUpdateTime = 0;
  int deltaT = 0;

  float legLTarget = 180;
  float legRTarget = 180;
  float armLTarget = 180;
  float armRTarget = 180;

  float armLNeutral = 180;
  float armRNeutral = 180;
  float legLNeutral = 185;
  float legRNeutral = 185;

  float speedAbsoluteMode = .4;
  float speedUnlimitedMode = 5;
  float speedRelativeMode = .1;
  float speedSummativeMode = .3;
  float speedSummativeFastMode = .5;
  float speedTriggerMax = .12;

  // Define constants for the control gains
  float teleKp = 0.02; // Proportional gain
  float teleKd = 0.1;  // Derivative gain, prevents oscilation (or creates oscilation if too high)
  float teleK = 0.01;  // Local joint damping gain, makes it heavier to push arm

  // these are partial modes, that can be overlayed? How to call this
  void submodeCalibrate();
  void submodeStop();
  void submodeStand();
  void submodeMenu();
  void submodeMenuOption();
  void submodeArmNeutral();

  void defaultSubmodes();

  void modeAbsolute(int rotDegrees, float speed);
  void modeLegsRelative();
  void modeSummative(int rotDegrees, float speed);
  void modePose();
  void modeTelepresence();

  float adjustByDisplacement(float currentVal, float target, float displacement);
  float calcTelepresenceTorque(float angleDiff, float velDiff, float localVel);
};

#endif // JOYSTICK_CONTROL_H