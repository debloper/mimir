#include <ESP_I2S.h>
I2SClass I2S;

// Configure your board specific pins here
const int I2S_DATA_PIN = 41;
const int I2S_CLOCK_PIN = 42;
// Operation modes: I2S_MODE_STD, I2S_MODE_TDM, I2S_MODE_PDM_TX, I2S_MODE_PDM_RX
const i2s_mode_t I2S_OPERATION_MODE = I2S_MODE_PDM_RX;
// Sample rates: no specific rule, but commonly 8000, 16000, 32000, 44100, 48000 in Hz
const uint32_t I2S_SAMPLE_RATE = 16000;
// Bit width: I2S_DATA_BIT_WIDTH_nBIT, where n ∈ {8, 16, 24, 32} (may need MCLK multiplier)
const i2s_data_bit_width_t I2S_BIT_WIDTH = I2S_DATA_BIT_WIDTH_16BIT;
// Channel mode: either I2S_SLOT_MODE_MONO (left channel only) or I2S_SLOT_MODE_STEREO
const i2s_slot_mode_t I2S_CHANNEL_MODE = I2S_SLOT_MODE_MONO;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Configure pin 42 as I2S clock and pin 41 as I2S data pins
  I2S.setPinsPdmRx(I2S_CLOCK_PIN, I2S_DATA_PIN);

  // Initialize I2S in PDM mode
  if (!I2S.begin(I2S_MODE_PDM_RX, I2S_SAMPLE_RATE, I2S_BIT_WIDTH, I2S_CHANNEL_MODE)) {
    Serial.println("Failed to initialize I2S!");
    return;
  }
}

void loop() {
  // Read I2S PDM samples
  int sample = I2S.read();

  // The following will spew 16bit integers
  // Representing the amplitude of the samples
  // Use serial plotter to visualize the waveform
  if (sample != -1) Serial.println(sample);
}
