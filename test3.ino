#include <Arduino.h>

// Global Variables (Shared across files)
float humid = 0.0;
float tempC = 0.0;
float luxValue = 0.0;
bool bleConnected = false;

// Function Prototypes (from other files)
void setupSensors();
void readSensors();
void setupBLE();
void runBLE();

void setup() {
  Serial.begin(115200);
  Serial.println("System Starting...");

  setupSensors();
  setupBLE();
}

void loop() {
  readSensors();
  runBLE();
}


