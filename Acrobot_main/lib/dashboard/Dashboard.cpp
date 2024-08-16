
#include "Dashboard.h"

Dashboard &Dashboard::setLeftArm(const Limb &arm)
{
  leftArm = &arm;
  return *this;
}

Dashboard &Dashboard::setRightArm(const Limb &arm)
{
  rightArm = &arm;
  return *this;
}

Dashboard &Dashboard::setRightLeg(const Limb &leg)
{
  rightLeg = &leg;
  return *this;
}

Dashboard &Dashboard::setLeftLeg(const Limb &leg)
{
  leftLeg = &leg;
  return *this;
}

Dashboard &Dashboard::setBatterySensor(const BatterySensor &sensor)
{
  batterySensor = &sensor;
  return *this;
}

const LimbStatus Dashboard::getLeftArmStatus() const
{
  return getLimbStatus(leftArm);
}

const LimbStatus Dashboard::getRightArmStatus() const
{
  return getLimbStatus(rightArm);
}

const LimbStatus Dashboard::getLeftLegStatus() const
{
  return getLimbStatus(leftLeg);
}

const LimbStatus Dashboard::getRightLegStatus() const
{
  return getLimbStatus(rightLeg);
}

const LimbStatus Dashboard::getLimbStatus(const Limb *limb) const
{
  if (limb == nullptr)
  {
    return LimbStatus{0.0, 0.0, 0.0, 0.0, 0, true, false, 1};
  }

  return limb->getStatus();
}

StaticJsonDocument<120> Dashboard::getLimbStatusJson(const Limb *limb) const
{
  const LimbStatus status = getLimbStatus(limb);
  StaticJsonDocument<300> doc;
  doc["target"] = status.target;
  doc["position"] = status.position;
  doc["velocity"] = status.velocity;
  doc["torque"] = status.torque;
  doc["temperature"] = status.temperature;
  doc["isCalibrating"] = status.isCalibrating;
  doc["isOnline"] = status.isOnline;
  doc["errorCode"] = status.errorCode;

  return doc;
}

const RobotStatus Dashboard::getRobotStatus() const
{
  return RobotStatus{getLimbStatus(leftArm), getLimbStatus(rightArm), getLimbStatus(leftLeg), getLimbStatus(rightLeg)};
}

StaticJsonDocument<600> Dashboard::getRobotStatusJson() const
{
  StaticJsonDocument<600> doc;
  doc["leftArm"] = getLimbStatusJson(leftArm);
  doc["rightArm"] = getLimbStatusJson(rightArm);
  doc["leftLeg"] = getLimbStatusJson(leftLeg);
  doc["rightLeg"] = getLimbStatusJson(rightLeg);
  return doc;
}
