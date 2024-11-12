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

TaskHandle_t channelTaskHandles[4];
uint8_t differentialChannels[] = {DIFF_0_1, DIFF_2_3, DIFF_4_5, DIFF_6_7};

// Array to hold raw values from each channel
long rawValues[4];

// Mutex for shared access (if needed for synchronization)
SemaphoreHandle_t xMutex;

void readChannelTask(void *parameter) {
  int channelIndex = *(int *)parameter;
  for (;;) {
    adc.setMUX(differentialChannels[channelIndex]); // Set MUX for the specified channel
    rawValues[channelIndex] = adc.readSingle();    // Read raw ADC value
    // No delay to maximize speed
  }
}

void setup() {
  // Configure SPI using the VSPI hardware SPI bus
  SPI.begin(18, 19, 23, CS_PIN); // SCLK = 18, MISO = 19, MOSI = 23, CS = 17
  SPI.setFrequency(2000000); // Increase SPI speed to 2 MHz (adjust if needed)

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
  adc.setDRATE(DRATE_100SPS); // Set data rate to 1000 samples per second

  // Create tasks for each channel
  for (int i = 0; i < 4; i++) {
    xTaskCreatePinnedToCore(
      readChannelTask,         // Function to run as a task
      "ReadChannelTask",       // Name of the task
      4096,                    // Stack size (in bytes)
      (void *)&i,              // Task parameter (channel index)
      1,                       // Priority of the task
      &channelTaskHandles[i],  // Task handle
      0                        // Core to run the task on (0 or 1, or tskNO_AFFINITY)
    );
    delay(100); // Small delay to ensure the parameter is set correctly
  }

  // Create mutex for safe access (if needed)
  xMutex = xSemaphoreCreateMutex();
}

void loop() {
  // Print all raw values side by side
  if (xMutex != NULL && xSemaphoreTake(xMutex, (TickType_t)10) == pdTRUE) {
    Serial.print("CH0-1: ");
    Serial.print(rawValues[0]);
    Serial.print("\tCH2-3: ");
    Serial.print(rawValues[1]);
    Serial.print("\tCH4-5: ");
    Serial.print(rawValues[2]);
    Serial.print("\tCH6-7: ");
    Serial.println(rawValues[3]);
    xSemaphoreGive(xMutex);
  }
}
