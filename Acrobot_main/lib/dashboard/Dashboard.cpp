
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

const MotorStatus Dashboard::getLeftArmStatus() const
{
  return getLimbStatus(leftArm);
}

const MotorStatus Dashboard::getRightArmStatus() const
{
  return getLimbStatus(rightArm);
}

const MotorStatus Dashboard::getLeftLegStatus() const
{
  return getLimbStatus(leftLeg);
}

const MotorStatus Dashboard::getRightLegStatus() const
{
  return getLimbStatus(rightLeg);
}

const MotorStatus Dashboard::getLimbStatus(const Limb *limb) const
{
  return limb != nullptr ? limb->getStatus() : MotorStatus{0.0, 0.0, 0.0, 0, false, 1};
}

StaticJsonDocument<90> Dashboard::getLimbStatusJson(const Limb *limb) const
{
  const MotorStatus status = getLimbStatus(limb);
  StaticJsonDocument<200> doc;
  doc["position"] = status.position;
  doc["velocity"] = status.velocity;
  doc["torque"] = status.torque;
  doc["temperature"] = status.temperature;
  doc["isOnline"] = status.isOnline;
  doc["errorCode"] = status.errorCode;

  return doc;
}

const RobotStatus Dashboard::getRobotStatus() const
{
  return RobotStatus{getLimbStatus(leftArm), getLimbStatus(rightArm), getLimbStatus(leftLeg), getLimbStatus(rightLeg)};
}

StaticJsonDocument<400> Dashboard::getRobotStatusJson() const
{
  StaticJsonDocument<400> doc;
  doc["leftArm"] = getLimbStatusJson(leftArm);
  doc["rightArm"] = getLimbStatusJson(rightArm);
  doc["leftLeg"] = getLimbStatusJson(leftLeg);
  doc["rightLeg"] = getLimbStatusJson(rightLeg);
  return doc;
}
