#include <ADS1256.h>
#include <SPI.h>

// Define pins for ESP32 (adjust as needed for your setup)
#define DRDY_PIN 16    // Data Ready (DRDY) pin
#define RESET_PIN 0    // No separate reset pin used (set to 0 if unused)
#define SYNC_PIN 21    // Power Down / Sync (PWDN) pin
#define CS_PIN 17      // Chip Select (CS) pin
#define VREF 2.500     // Reference voltage for ADS1256

// Initialize ADS1256 instance
ADS1256 adc(DRDY_PIN, RESET_PIN, SYNC_PIN, CS_PIN, VREF);

void setup() {
  // Configure SPI using the VSPI hardware SPI bus
  SPI.begin(18, 19, 23, CS_PIN); // SCLK = 18, MISO = 19, MOSI = 23, CS = 17

  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial connection
  }

  Serial.println("Initializing ADS1256...");

  // Initialize the ADS1256 ADC
  adc.InitializeADC();
  Serial.println("ADS1256 initialized.");

  // Configure the ADS1256
  adc.setPGA(PGA_64); // Set gain (e.g., 1x)
  adc.setMUX(DIFF_0_1); // Set initial differential input (channels 0 and 1)
  adc.setDRATE(DRATE_1000SPS); // Set data rate to 1000 samples per second
}

void loop() {
  // Read a single conversion in differential mode between channels 0 and 1
  long rawValue = adc.readSingle();

  // Print the raw value
  Serial.print("Raw Value: ");
  Serial.println(rawValue);

  //delay(500); // Adjust delay as needed
}
