#include "FS.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "time.h"

#include "ESP_I2S.h"
I2SClass I2S;
uint8_t *wav_buffer;
size_t wav_size;

// WiFi credentials
const char* ssid = "wifi-ssid";
const char* password = "wifi-password";

// Object storage credentials
const char* s3_bucket_base_url = "";
const char* s3_access_key = "";
const char* s3_secret_key = "";

// IF YOU ARE READING THIS COMMENT THEN
// THIS CODE IS STILL SPECIFICALLY WRITTEN
// FOR SEEED STUDIO XIAO ESP32S3 SENSE MODULE

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi.");

  // Initialize time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println("Initializing I2S bus...");
  I2S.setPinsPdmRx(42, 41);
  if (!I2S.begin(I2S_MODE_PDM_RX, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Can't initialize I2S bus!");
    return;
  }
  Serial.println("I2S bus initialized.");

  Serial.println("Recording 60 seconds of audio...");
  wav_buffer = I2S.recordWAV(60, &wav_size);

  // Generate unique file name based on timestamp
  char file_name[64];
  getLocalTime(&timeinfo);
  strftime(file_name, sizeof(file_name), "record_%Y%m%d_%H%M%S.wav", &timeinfo);

  // Create full URL
  char s3_url[256];
  snprintf(s3_url, sizeof(s3_url), "%s%s", s3_bucket_base_url, file_name);

  Serial.println("Uploading audio data to S3...");
  if (uploadToS3(s3_url, wav_buffer, wav_size)) {
    Serial.println("Audio data uploaded to S3.");
  } else {
    Serial.println("Failed to upload audio data to S3.");
  }

  Serial.println("OK");
}

bool uploadToS3(const char* url, uint8_t* buffer, size_t size) {
  HTTPClient http;
  Serial.printf("Uploading to URL: %s\n", url);
  http.begin(url);
  http.addHeader("Content-Type", "audio/wav");

  int httpResponseCode = http.PUT(buffer, size);

  if (httpResponseCode > 0 && httpResponseCode < 400) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    return true;
  } else {
    Serial.printf("Error code: %d\n", httpResponseCode);
    Serial.printf("Error message: %s\n", http.errorToString(httpResponseCode).c_str());
    return false;
  }

  http.end();
}

void loop() {}
