#include "DebugLed.h"

DebugLed::DebugLed()  {}

void DebugLed::setRGB(uint8_t _red, uint8_t _green, uint8_t _blue)
{
  red = _red;
  green = _green;
  blue = _blue;
  colorsChanged = true;
}

void DebugLed::setRTemp(uint8_t _red, uint16_t ms)
{
  redTimer = millis() + ms;

  redTimerActive = true;
  setR(_red);
  
}

void DebugLed::setR(uint8_t _red)
{
  if (red == _red)
  {
    return;
  }

  red = _red;
  colorsChanged = true;
}

void DebugLed::setG(uint8_t _green)
{
  if (green == _green)
  {
    return;
  }
  green = _green;
  colorsChanged = true;
}

void DebugLed::setB(uint8_t _blue)
{
  if (blue == _blue)
  {
    return;
  }
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

  if(redTimerActive && redTimer < millis()){
    setR(0);
  }

  // update colors
  if (colorsChanged)
  {
    colorsChanged = false;
    if (flickerOn)
    {
      Serial2.printf("dL=%u,%u,%u\n", (int)((float)red * brightnessMultiplier), (int)((float)green * brightnessMultiplier), (int)((float)blue * brightnessMultiplier));
    }
    else
    {
      Serial2.printf("dL=%u,%u,%u\n", red, green, blue);
    }
  }
}