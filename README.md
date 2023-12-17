# Acrobot

This project is the code for the Acrobot, an acrobatic performance robot. See http://acrobot.nl/

The acrobot runs on 2 ESP32's which we call 'main' and 'joystick_bt'. The main project is setup in platformio. The joystick allows us to communicate with a wireless controller over bluetooth, but due to the bluetooth library it is setup in the Arduino IDE.

On the main, you find all the custom libraries /lib, the main file in /src, and replacements for the main file to be able to run tests in /test. You can run a test file by loading a different platformio environment.

To build the joystick_bt ESP:
Use Arduino IDE
Boardmanager, add url to preferences: https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
Install: esp32_bluepad32 1.10.2
No need for the bluepad library!