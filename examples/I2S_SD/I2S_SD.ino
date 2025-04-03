#include "FS.h"
#include "SD.h"

// NOTE: REQUIRES OPI PSRAM TO BE ENABLED

#include "ESP_I2S.h"
I2SClass I2S;

File file;
bool isRecording = false;
volatile bool toggleRequested = false;

// IF YOU ARE READING THIS COMMENT THEN
// THIS CODE IS STILL SPECIFICALLY WRITTEN
// FOR SEEED STUDIO XIAO ESP32S3 SENSE MODULE

// Set pin as capacitive touch sensor
const int TOUCH_PIN = 1;
// Set uint32_t threshold for touch detection on ESP32 S3
const int TOUCH_THRESHOLD = 65536;

const unsigned long DEBOUNCE_DELAY = 1000;
unsigned long lastToggleTime = 0;

void IRAM_ATTR toggleRecording() {
  toggleRequested = true;
}

// Helper function to write WAV header
void writeWavHeader(File file, uint32_t dataLength) {
    uint32_t sampleRate = 16000;
    uint16_t bitsPerSample = 16;
    uint16_t numChannels = 1;
    uint32_t byteRate = sampleRate * numChannels * bitsPerSample / 8;
    uint16_t blockAlign = numChannels * bitsPerSample / 8;
    uint32_t chunkSize = dataLength + 36;

    file.seek(0);
    file.write((const uint8_t*)"RIFF", 4);
    file.write((uint8_t *)&chunkSize, 4);
    file.write((const uint8_t*)"WAVE", 4);
    file.write((const uint8_t*)"fmt ", 4);
    uint32_t subChunk1Size = 16;
    file.write((uint8_t *)&subChunk1Size, 4);
    uint16_t audioFormat = 1;
    file.write((uint8_t *)&audioFormat, 2);
    file.write((uint8_t *)&numChannels, 2);
    file.write((uint8_t *)&sampleRate, 4);
    file.write((uint8_t *)&byteRate, 4);
    file.write((uint8_t *)&blockAlign, 2);
    file.write((uint8_t *)&bitsPerSample, 2);
    file.write((const uint8_t*)"data", 4);
    file.write((uint8_t *)&dataLength, 4);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  pinMode(LED_BUILTIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, toggleRecording, TOUCH_THRESHOLD);

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
}

void loop() {
  if (toggleRequested) {
    toggleRequested = false;
    unsigned long currentTime = millis();
    if (currentTime - lastToggleTime >= DEBOUNCE_DELAY) {
      lastToggleTime = currentTime;
      if (!isRecording) {
        String filename = "/record_" + String(currentTime) + ".wav";
        file = SD.open(filename.c_str(), FILE_WRITE);
        if (!file) {
          Serial.println("Can't open file to write!");
          digitalWrite(LED_BUILTIN, isRecording);
          return;
        }
        // Write placeholder header with 0 data length
        writeWavHeader(file, 0);
        isRecording = true;
        digitalWrite(LED_BUILTIN, isRecording);
        Serial.print("Recording started: ");
        Serial.println(filename);
      } else {
        // Before closing, update header with proper data length
        uint32_t fileLength = file.size();
        uint32_t dataLength = (fileLength > 44) ? fileLength - 44 : 0;
        file.seek(0);
        writeWavHeader(file, dataLength);
        isRecording = false;
        digitalWrite(LED_BUILTIN, isRecording);
        file.close();
        Serial.println("Recording stopped and file saved.");
      }
    }
  }

  if (isRecording && file) {
    int bytesAvailable = I2S.available();
    if (bytesAvailable > 0) {
      char buffer[512];
      int bytesToRead = (bytesAvailable < 512) ? bytesAvailable : 512;
      int bytesRead = I2S.readBytes(buffer, bytesToRead);
      if (bytesRead > 0) {
        file.write((uint8_t*)buffer, bytesRead);
      }
    }
  }
}
