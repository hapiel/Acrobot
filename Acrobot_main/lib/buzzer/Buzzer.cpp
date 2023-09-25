#include <buzzer.h>

Buzzer::Buzzer(uint8_t pin, RemoteDebug& Debug) : pin(pin), Debug(Debug){
}

void Buzzer::init()
{
  pinMode(pin, OUTPUT);
}

void Buzzer::update()
{
  digitalWrite(pin, timer > millis());
}

void Buzzer::buzz(uint16_t ms)
{
  timer = millis() + ms;
  debugD("BUZZER: beep %u", ms);
}
