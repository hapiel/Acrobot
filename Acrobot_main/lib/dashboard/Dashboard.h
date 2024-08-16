#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "BatterySensor.h"
#include "Limb.h"
#include <ArduinoJson.h>

struct RobotStatus
{
  LimbStatus leftArm;
  LimbStatus rightArm;
  LimbStatus leftLeg;
  LimbStatus rightLeg;
};

class Dashboard
{
public:
  Dashboard &setLeftArm(const Limb &arm);
  Dashboard &setRightArm(const Limb &arm);
  Dashboard &setRightLeg(const Limb &leg);
  Dashboard &setLeftLeg(const Limb &leg);
  Dashboard &setBatterySensor(const BatterySensor &sensor);

  const LimbStatus getLeftArmStatus() const;
  const LimbStatus getRightArmStatus() const;
  const LimbStatus getLeftLegStatus() const;
  const LimbStatus getRightLegStatus() const;

  const RobotStatus getRobotStatus() const;
  StaticJsonDocument<600> getRobotStatusJson() const;

private:
  const Limb *leftArm = nullptr;
  const Limb *rightArm = nullptr;
  const Limb *leftLeg = nullptr;
  const Limb *rightLeg = nullptr;
  const BatterySensor *batterySensor = nullptr;

  const LimbStatus getLimbStatus(const Limb *limb) const;
  StaticJsonDocument<120> getLimbStatusJson(const Limb *limb) const;
};

#endif // DASHBOARD_H
