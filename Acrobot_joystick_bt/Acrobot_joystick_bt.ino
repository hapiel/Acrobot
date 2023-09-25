/*
ACROBOT JOYSTICK ESP
Daniel Simu - 2023
This is the code for the secondary ESP32 of the Acrobot. It communicates with the main ESP32 over hardware serial 2.
It needs to be compiled out of the Arduino IDE because this is compatible with Bluepad32.
This secondary ESP32 is used so that it can use bluetooth, whilst the main esp can run wifi at the same time.
Code mostly consists of Bluepad32 example code.
The main ESP32 can send the following commands to trigger the joystick:
set_color_led=255,0,0
set_player_leds=4

Rumble does not seem to work correctly at the moment, could be fixed later. Command for this:
set_rumble=255,100

We're controlling this project with a PS5 controller
*/

#include <Bluepad32.h>

#define DEBUG_LED_RED 35
#define DEBUG_LED_GREEN 32
#define DEBUG_LED_BLUE 33


GamepadPtr myGamepads[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedGamepad(GamepadPtr gp)
{
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    if (myGamepads[i] == nullptr)
    {
      Serial.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      GamepadProperties properties = gp->getProperties();
      Serial.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n",
                    gp->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myGamepads[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot)
  {
    Serial.println(
        "CALLBACK: Gamepad connected, but could not found empty slot");
  }
}

void onDisconnectedGamepad(GamepadPtr gp)
{
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    if (myGamepads[i] == gp)
    {
      Serial.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
      myGamepads[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad)
  {
    Serial.println(
        "CALLBACK: Gamepad disconnected, but not found in myGamepads");
  }
}

void initDebugLed(void)
{
  pinMode(DEBUG_LED_RED, OUTPUT);
  pinMode(DEBUG_LED_GREEN, OUTPUT);
  pinMode(DEBUG_LED_BLUE, OUTPUT);
  analogWrite(DEBUG_LED_RED, 255);
  analogWrite(DEBUG_LED_GREEN, 255);
  analogWrite(DEBUG_LED_BLUE, 255);
}

void setDebugLed(int red, int green, int blue)
{
  analogWrite(DEBUG_LED_RED, red);
  analogWrite(DEBUG_LED_GREEN, green);
  analogWrite(DEBUG_LED_BLUE, blue);
}

// Arduino setup function. Runs in CPU 1
void setup()
{
  initDebugLed();
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2],
                addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();
}

int rumbleForce = 0;
int rumbleDuration = 0;

// Arduino loop function. Runs in CPU 1
void loop()
{
  // This call fetches all the gamepad info from the NINA (ESP32) module.
  // Just call this function in your main loop.
  // The gamepads pointer (the ones received in the callbacks) gets updated
  // automatically.
  BP32.update();

  // It is safe to always do this before using the gamepad API.
  // This guarantees that the gamepad is valid and connected.

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    GamepadPtr myGamepad = myGamepads[i];

    if (myGamepad && myGamepad->isConnected())
    {

      // read commands
      if (Serial2.available())
      {
        String command = Serial2.readStringUntil('\n');
        Serial.println(command);
        if (command.startsWith("set_joystick_color_led"))
        {
          int red = command.substring(command.indexOf('=') + 1, command.indexOf(',')).toInt();
          int green = command.substring(command.indexOf(',') + 1, command.lastIndexOf(',')).toInt();
          int blue = command.substring(command.lastIndexOf(',') + 1).toInt();
          setColorLED(myGamepad, red, green, blue);
        }
        else if (command.startsWith("set_joystick_player_leds"))
        {
          int ledValue = command.substring(command.indexOf('=') + 1).toInt();
          setPlayerLEDs(myGamepad, ledValue);
        }
        else if (command.startsWith("set_rumble"))
        {
          rumbleForce = command.substring(command.indexOf('=') + 1, command.indexOf(',')).toInt();
          rumbleDuration = command.substring(command.lastIndexOf('=') + 1).toInt();
          setRumble(myGamepad, rumbleForce, rumbleDuration);
        }
        else if (command.startsWith("set_debug_led"))
        {
          int red = command.substring(command.indexOf('=') + 1, command.indexOf(',')).toInt();
          int green = command.substring(command.indexOf(',') + 1, command.lastIndexOf(',')).toInt();
          int blue = command.substring(command.lastIndexOf(',') + 1).toInt();
          setDebugLed(red, green, blue);
        }
      }

      // Another way to query the buttons, is by calling buttons(), or
      // miscButtons() which return a bitmask.
      // Some gamepads also have DPAD, axis and more.
      // Serial.printf(
      //     "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: "
      //     "%4d, %4d, brake: %4d, throttle: %4d, misc: 0x%02x, gyro x:%6d y:%6d "
      //     "z:%6d, accel x:%6d y:%6d z:%6d bat:%3d\n",
      //     i,                        // Gamepad Index
      //     myGamepad->dpad(),        // DPAD
      //     myGamepad->buttons(),     // bitmask of pressed buttons
      //     myGamepad->axisX(),       // (-511 - 512) left X Axis
      //     myGamepad->axisY(),       // (-511 - 512) left Y axis
      //     myGamepad->axisRX(),      // (-511 - 512) right X axis
      //     myGamepad->axisRY(),      // (-511 - 512) right Y axis
      //     myGamepad->brake(),       // (0 - 1023): brake button
      //     myGamepad->throttle(),    // (0 - 1023): throttle (AKA gas) button
      //     myGamepad->miscButtons(), // bitmak of pressed "misc" buttons
      //     myGamepad->gyroX(),       // Gyro X
      //     myGamepad->gyroY(),       // Gyro Y
      //     myGamepad->gyroZ(),       // Gyro Z
      //     myGamepad->accelX(),      // Accelerometer X
      //     myGamepad->accelY(),      // Accelerometer Y
      //     myGamepad->accelZ(),      // Accelerometer Z
      //     myGamepad->battery()
      // );
      // send to other esp
      Serial2.printf(
          "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: "
          "%4d, %4d, brake: %4d, throttle: %4d, misc: 0x%02x, gyro x:%6d y:%6d "
          "z:%6d, accel x:%6d y:%6d z:%6d bat:%3d\n",
          i,                        // Gamepad Index
          myGamepad->dpad(),        // DPAD
          myGamepad->buttons(),     // bitmask of pressed buttons
          myGamepad->axisX(),       // (-511 - 512) left X Axis
          myGamepad->axisY(),       // (-511 - 512) left Y axis
          myGamepad->axisRX(),      // (-511 - 512) right X axis
          myGamepad->axisRY(),      // (-511 - 512) right Y axis
          myGamepad->brake(),       // (0 - 1023): brake button
          myGamepad->throttle(),    // (0 - 1023): throttle (AKA gas) button
          myGamepad->miscButtons(), // bitmak of pressed "misc" buttons
          myGamepad->gyroX(),       // Gyro X
          myGamepad->gyroY(),       // Gyro Y
          myGamepad->gyroZ(),       // Gyro Z
          myGamepad->accelX(),      // Accelerometer X
          myGamepad->accelY(),      // Accelerometer Y
          myGamepad->accelZ(),      // Accelerometer Z
          myGamepad->battery());

      // You can query the axis and other properties as well. See Gamepad.h
      // For all the available functions.
    }
  }

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

  // vTaskDelay(1);
  delay(1);
}

void setColorLED(GamepadPtr gamepad, int red, int green, int blue)
{
  gamepad->setColorLED(red, green, blue);
}

void setPlayerLEDs(GamepadPtr gamepad, int ledValue)
{
  gamepad->setPlayerLEDs(ledValue & 0x0F);
  Serial.println(ledValue);
}

void setRumble(GamepadPtr gamepad, unsigned int force, unsigned int duration)
{
  gamepad->setRumble(force, duration);
}
