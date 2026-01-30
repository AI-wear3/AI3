#include <Arduino.h>
#include <Wire.h>
#include <BMx280I2C.h>

#define I2C_ADDRESS 0x76

// Create a BMx280I2C object using the I2C interface with I2C Address 0x76
BMx280I2C bmx280(I2C_ADDRESS);

void setup() {
  // Initialize Serial at 115200 baud
  Serial.begin(115200);

  // Wait for serial connection to open
  while (!Serial);

  Wire.begin();

  // Initialize sensor
  if (!bmx280.begin()) {
    Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
    while (1);
  }

  // Check sensor type
  if (bmx280.isBME280())
    Serial.println("Sensor is a BME280");
  else
    Serial.println("Sensor is a BMP280");

  // Reset sensor to default parameters
  bmx280.resetToDefaults();

  // Enable oversampling
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

  if (bmx280.isBME280())
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
}

void loop() {
  delay(1000);

  // Start a measurement
  if (!bmx280.measure()) {
    Serial.println("Could not start measurement, is a measurement already running?");
    return;
  }

  // Wait for the measurement to finish
  do {
    delay(100);
  } while (!bmx280.hasValue());

  float temperature = bmx280.getTemperature();
  float humidity = 0.0;
  
  if (bmx280.isBME280()) {
    humidity = bmx280.getHumidity();
  }

  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
  
  if (bmx280.isBME280()) {
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  }

  // Baby Room Conditions:
  // Temp: 20 ~ 24 C
  // Humidity: 40 ~ 60 % (Only check if BME280)

  bool tempOk = (temperature >= 20.0 && temperature <= 24.0);
  bool humOk = true; // Default true if BMP280 (no humidity)
  
  if (bmx280.isBME280()) {
    humOk = (humidity >= 40.0 && humidity <= 60.0);
  }

  if (tempOk && humOk) {
    Serial.println("Status: GOOD");
  } else {
    Serial.println("Status: BAD");
    if (!tempOk) Serial.print(" -> Temperature out of range! ");
    if (!humOk && bmx280.isBME280()) Serial.print(" -> Humidity out of range! ");
    Serial.println();
  }
  
  Serial.println("-----------------------------");
}
