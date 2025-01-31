#include "FS.h"
#include "SD.h"

// Configure your board specific pins here
const int SD_CS = 21;
const char * FILE_TEXT = "Hello, World!";
const char * FILE_NAME = "/example.txt";

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.print("Mounting SD card ");
  if(!SD.begin(SD_CS)){
    Serial.println("failed!");
    return;
  }
  Serial.println("succeeded.");

  Serial.print("Opening file ");
  File file = SD.open(FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.println("failed!");
    return;
  }
  Serial.println("succeeded.");

  Serial.print("Writing data to file ");
  if (file.write((const uint8_t*)FILE_TEXT, strlen(FILE_TEXT)) != strlen(FILE_TEXT)) {
    Serial.println("failed!");
    Serial.print("Closing file ");
    file.close();
    Serial.println("succeeded.");
    return;
  }
  Serial.println("succeeded.");

  Serial.print("Closing file ");
  file.close();
  Serial.println("succeeded.");

  Serial.println("OK");
}

void loop() {}
