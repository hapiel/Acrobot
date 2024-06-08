
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
  return limb != nullptr ? limb->getStatus() : MotorStatus{};
}
