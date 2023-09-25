#include "DebugLed.h"

DebugLed::DebugLed() {}

void DebugLed::setRGB(uint8_t _red, uint8_t _green, uint8_t _blue)
{
  red = _red;
  green = _green;
  blue = _blue;
  colorsChanged = true;
}

void DebugLed::setR(uint8_t _red)
{
  red = _red;
  colorsChanged = true;
}

void DebugLed::setG(uint8_t _green)
{
  green = _green;
  colorsChanged = true;
}

void DebugLed::setB(uint8_t _blue)
{
  blue = _blue;
  colorsChanged = true;
}

void DebugLed::flicker(int time)
{
  flickerTimer = millis() + time;
  colorsChanged = true;
  flickerOn = true;
}

void DebugLed::update()
{
  // reset flicker
  if (flickerOn && flickerTimer < millis())
  {
    flickerOn = false;
    colorsChanged = true;
  }

  // update colors
  if (colorsChanged)
  {
    colorsChanged = false;
    if (flickerOn)
    {
      Serial2.printf("set_debug_led=%u,%u,%u\n", red / brightnessDivider, green / brightnessDivider, blue / brightnessDivider);
    }
    else
    {
      Serial2.printf("set_debug_led=%u,%u,%u\n", red, green, blue);
    }
  }
}