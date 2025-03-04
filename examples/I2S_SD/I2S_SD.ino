#include "FS.h"
#include "SD.h"

#include "ESP_I2S.h"
I2SClass I2S;
uint8_t *wav_buffer;
size_t wav_size;

// IF YOU ARE READING THIS COMMENT THEN
// THIS CODE IS STILL SPECIFICALLY WRITTEN
// FOR SEEED STUDIO XIAO ESP32S3 SENSE MODULE

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("Initializing I2S bus...");
  I2S.setPinsPdmRx(42, 41);
  if (!I2S.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Can't initialize I2S bus!");
    return;
  }
  Serial.println("I2S bus initialized.");

  Serial.println("Mounting SD card...");
  if(!SD.begin(21)){
    Serial.println("Can't mount SD Card!");
    return;
  }
  Serial.println("SD card mounted.");

  Serial.println("Recording 60 seconds of audio...");
  wav_buffer = I2S.recordWAV(60, &wav_size);

  Serial.println("Opening WAV file...");
  File file = SD.open("/record.wav", FILE_WRITE);
  if (!file) {
    Serial.println("Can't open file to write!");
    return;
  }
  Serial.println("WAV file opened.");

  Serial.println("Writing audio data to file...");
  if (file.write(wav_buffer, wav_size) != wav_size) {
    Serial.println("Can't write audio data to file!");
    return;
  }
  Serial.println("Audio data written to file.");

  Serial.println("Closing file...");
  file.close();
  Serial.println("File closed.");

  Serial.println("OK");
}

void loop() {}
