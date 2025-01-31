// Set pin as capacitive touch sensor
const int TOUCH_PIN = 1;

// Set threshold for touch detection
// uint32_t on ESP32 S2/S3, uint16_t on other ESP32
const int TOUCH_THRESHOLD = 65536; // Set to 64 for uint16_t modules

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, toggleLED, TOUCH_THRESHOLD);
}

void toggleLED() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void loop() {}
