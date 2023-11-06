/*
  Listfiles

  This example shows how print out the files in a
  directory on a SD card

  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe
  modified 2 Feb 2014
  by Scott Fitzgerald

  This example code is in the public domain.

*/
#include <SPI.h>
#include <SD.h>

#include "WiFi.h"
#include "Wire.h"
#include "WiFiClientSecure.h"

File root;


void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void task1(void *parameter)
{
  for (;;)
  {

    

    vTaskDelay(10);
  }
}

void task2(void *parameter)
{
  for (;;)
  {

    root = SD.open("/");

  printDirectory(root, 0);

    vTaskDelay(10);
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  SPI.begin(22, 19, 23, 18);

  Serial.print("Initializing SD card...");

  if (!SD.begin(18)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  

  Serial.println("done!");

  xTaskCreatePinnedToCore(task1, "task1", 50000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(task2, "task2", 50000, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelay(100);
}

