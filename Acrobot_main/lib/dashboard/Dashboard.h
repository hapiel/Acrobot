#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "BatterySensor.h"
#include "Limb.h"
#include "Motor.h"
#include <ArduinoJson.h>

struct RobotStatus
{
  MotorStatus leftArm;
  MotorStatus rightArm;
  MotorStatus leftLeg;
  MotorStatus rightLeg;
};

class Dashboard
{
public:
  Dashboard &setLeftArm(const Limb &arm);
  Dashboard &setRightArm(const Limb &arm);
  Dashboard &setRightLeg(const Limb &leg);
  Dashboard &setLeftLeg(const Limb &leg);
  Dashboard &setBatterySensor(const BatterySensor &sensor);

  const MotorStatus getLeftArmStatus() const;
  const MotorStatus getRightArmStatus() const;
  const MotorStatus getLeftLegStatus() const;
  const MotorStatus getRightLegStatus() const;

  const RobotStatus getRobotStatus() const;
  StaticJsonDocument<400> getRobotStatusJson() const;

private:
  const Limb *leftArm = nullptr;
  const Limb *rightArm = nullptr;
  const Limb *leftLeg = nullptr;
  const Limb *rightLeg = nullptr;
  const BatterySensor *batterySensor = nullptr;

  const MotorStatus getLimbStatus(const Limb *limb) const;
  StaticJsonDocument<90> getLimbStatusJson(const Limb *limb) const;
};

#endif // DASHBOARD_H
