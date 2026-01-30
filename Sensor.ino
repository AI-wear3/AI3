/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2652

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10



Adafruit_BME280 bme; // I2C
BH1750 lightMeter;

// XIAO ESP32S3 Specific I2C Pins
#define MY_SDA 5  // D4
#define MY_SCL 6  // D5

extern float humid;
extern float tempC;
extern float luxValue;

bool bmeActive = false;
bool lightActive = false;

unsigned long delayTime;

void setupSensors() {
    Serial.println(F("BME280 test"));
    
    // Debug: Print Pins
    Serial.print("Initializing I2C on SDA: ");
    Serial.print(MY_SDA);
    Serial.print(" SCL: ");
    Serial.println(MY_SCL);

    Wire.begin(MY_SDA, MY_SCL);

    // --- I2C Scanner Start ---
    Serial.println("Scanning I2C bus...");
    byte error, address;
    int nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address<16) Serial.print("0");
            Serial.print(address,HEX);
            Serial.println("  !");
            nDevices++;
        }
        else if (error==4) {
            Serial.print("Unknown error at address 0x");
            if (address<16) Serial.print("0");
            Serial.println(address,HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
    // --- I2C Scanner End ---
    
    // BH1750 Init
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println(F("BH1750 Init Success"));
        lightActive = true;
    } else {
        Serial.println(F("Error configuring BH1750"));
        lightActive = false;
    }

    // BME280 Init
    unsigned status;
    // Try 0x76 first
    status = bme.begin(0x76);  
    if (!status) {
        Serial.println("BME280 at 0x76 failed. Trying 0x77...");
        status = bme.begin(0x77);
    }

    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        bmeActive = false;
    } else {
        Serial.println("BME280 Found!");
        bmeActive = true;
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;
}

void readSensors() { 
    printValues();
    delay(delayTime);
}

void printValues() {
    Serial.println("-------------------------");
    
    // 1. Read BME280
    if (bmeActive) {
        tempC = bme.readTemperature();
        humid = bme.readHumidity();

        // Check for NaN
        if (isnan(tempC) || isnan(humid)) {
            Serial.println("Failed to read from BME280!");
        } else {
            Serial.print("Temperature = ");
            Serial.print(tempC);
            Serial.println(" °C");

            Serial.print("Humidity = ");
            Serial.print(humid);
            Serial.println(" %");
        }
    } else {
        Serial.println("BME280 Not Connected");
        // Keep previous values or set to 0? 
        // For logic safety, maybe set to a 'safe' default or keep last good. 
        // User didn't specify, so we just don't update global vars to garbage.
    }

    // 2. Read BH1750
    if (lightActive) {
        luxValue = lightMeter.readLightLevel();
        if (luxValue < 0) { // BH1750 lib returns -1 or -2 on error sometimes
             Serial.println("Error reading BH1750");
        } else {
             Serial.print("Light: ");
             Serial.print(luxValue);
             Serial.print(" lx - ");
             
             // BH1750 Logic
            if (luxValue >= 50) {
                Serial.println("bad");
            } else if (luxValue >= 30) {
                Serial.println("careful");
            } else if (luxValue >= 10) {
                Serial.println("great");
            } else {
                Serial.println("excellent"); // 0 ~ 10
            }
        }
    } else {
        // Try to recover?
        Serial.println("BH1750 Not Connected. Retrying init...");
        if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
            lightActive = true;
        }
    }

    // BME280 Evaluation Logic (Only if valid)
    if (bmeActive && !isnan(tempC) && !isnan(humid)) {
        // 쾌적 (Perfect): 20 ~ 23°C & 45 ~ 55%
        // 위험 (Danger): (< 18°C or > 25°C) OR (< 40% or > 60%)
        // 주의 (Warning): Everything else

        bool tempDanger = (tempC < 18.0) || (tempC > 25.0);
        bool humidDanger = (humid < 40.0) || (humid > 60.0);

        bool tempPerfect = (tempC >= 20.0) && (tempC <= 23.0);
        bool humidPerfect = (humid >= 45.0) && (humid <= 55.0);

        Serial.print("Status: ");

        if (tempDanger || humidDanger) {
            Serial.println("Danger");
            if (tempC < 18.0) Serial.println(" - Reason: Temp too Low (Cold)");
            if (tempC > 25.0) Serial.println(" - Reason: Temp too High (Hot)");
            if (humid < 40.0) Serial.println(" - Reason: Humidity too Low (Dry)");
            if (humid > 60.0) Serial.println(" - Reason: Humidity too High (Humid)");
        } 
        else if (tempPerfect && humidPerfect) {
            Serial.println("Perfect");
        } 
        else {
            Serial.println("Warning");
        }
    }

    Serial.println("-------------------------");
    Serial.println();
}