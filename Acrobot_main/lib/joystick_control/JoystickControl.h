#ifndef JOYSTICK_CONTROL_H
#define JOYSTICK_CONTROL_H

#include <Arduino.h>
#include "RemoteDebug.h"
#include "Joystick.h"
#include "Leg.h"
#include "Arm.h"
#include "ChoreoPlayer.h"
#include "menu.h"
#include "EStop.h"
#include "debugLed.h"
#include "MovePlayer.h"
#include "Sequencer.h"
#include "BottangoSocket.h"

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
  MODE_SYNCH_90,
  MODE_SYNCH_140,
  MODE_TELEPRESENCE
};

class JoystickControl
{
public:
  JoystickControl(RemoteDebug &Debug, Joystick &joystick, Leg &legL, Leg &legR, Arm &armL, Arm &armR, ChoreoPlayer &choreoPlayer, Menu &menu, EStop &eStop, MovePlayer &movePlayer, Sequencer &sequencer, BottangoSocket &bottangoSocket);
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
  JoystickControlMode controlMode = MODE_SUMMATIVE_90;
  Menu &menu;
  EStop &eStop;
  MovePlayer &movePlayer;
  Sequencer &sequencer;
  BottangoSocket &bottangoSocket;

  long prevUpdateTime = 0;
  int deltaT = 0;

  float legLTarget = 180;
  float legRTarget = 180;
  float armLTarget = 180;
  float armRTarget = 180;

  float armNeutral = 180;
  float legNeutral = 185;

  float speedAbsoluteMode = .4;
  float speedUnlimitedMode = 5;
  float speedRelativeMode = .1;
  float speedSummativeMode = .3;
  float speedSummativeFastMode = .5;
  float speedTriggerMax = .12;
  float speedSynchMode = .4;

  int variableAngle[3] = {45, 95, 140};
  int variableSetting = 1;
  bool synchronized = false;

  bool joystickConnected = false;

  // Define constants for the control gains
  float teleKp = 0.02; // Proportional gain
  float teleKd = 0.1;  // Derivative gain, prevents oscilation (or creates oscilation if too high)
  float teleK = 0.01;  // Local joint damping gain, makes it heavier to push arm

  uint8_t ledR = 0;
  uint8_t ledG = 0;
  uint8_t ledB = 0;

  void init();

  // these are partial modes, that can be overlayed? How to call this
  void submodeStopChoreo();
  void submodeStop();
  void submodeStand();
  void submodeMenu();
  void submodeMenuOption();
  void submodeArmNeutralDpad();
  void submodeArmNeutralJoystick();
  void submodeToggleSynch();
  void submodeChangeVariableAngle();
  void submodeCurrentRumble();
  void submodePoseButtons();

  void defaultSubmodes();

  void modeAbsolute(int rotDegrees, float speed);
  void modeLegsRelative();
  void modeSummative(int rotDegrees, float speed);
  void modeTelepresence();
  void modeSynch(int rotDegrees, float speed);

  float adjustByDisplacement(float currentVal, float target, float displacement);
  float calcTelepresenceTorque(float angleDiff, float velDiff, float localVel);

  void setLedValues();
};

#endif // JOYSTICK_CONTROL_H
