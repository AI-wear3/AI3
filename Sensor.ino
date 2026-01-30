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

extern float humid;
extern float tempC;
extern float luxValue;

unsigned long delayTime;

void setupSensors() {
    // Serial.begin(9600); // Already started in main
    // while(!Serial);    
    Serial.println(F("BME280 test"));

    Wire.begin();
    lightMeter.begin();
    Serial.println(F("BH1750 Test begin"));

    unsigned status;
    
    // default settings
    status = bme.begin(0x76);  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;
}

void readSensors() { 
    printValues();
    delay(delayTime);
}

void printValues() {
    // Read values
    tempC = bme.readTemperature();
    humid = bme.readHumidity();
    luxValue = lightMeter.readLightLevel();

    // Print raw values
    Serial.print("Temperature = ");
    Serial.print(tempC);
    Serial.println(" °C");

    Serial.print("Humidity = ");
    Serial.print(humid);
    Serial.println(" %");

    Serial.print("Light: ");
    Serial.print(luxValue);
    Serial.print(" lx - ");

    // BH1750 Light Logic
    if (luxValue >= 50) {
        Serial.println("bad");
    } else if (luxValue >= 30) {
        Serial.println("careful");
    } else if (luxValue >= 10) {
        Serial.println("great");
    } else {
        Serial.println("excellent"); // 0 ~ 10
    }

    // BME280 Temperature & Humidity Logic
    // 쾌적 (Perfect): 20 ~ 23°C & 45 ~ 55%
    // 위험 (Danger): (< 18°C or > 25°C) OR (< 40% or > 60%)
    // 주의 (Warning): Everything else (e.g. 18-19, 24-25, 40-44, 56-60)

    bool tempDanger = (tempC < 18.0) || (tempC > 25.0);
    bool humidDanger = (humid < 40.0) || (humid > 60.0);

    bool tempPerfect = (tempC >= 20.0) && (tempC <= 23.0);
    bool humidPerfect = (humid >= 45.0) && (humid <= 55.0);

    Serial.println("-------------------------");
    Serial.print("Status: ");

    if (tempDanger || humidDanger) {
        Serial.println("Danger");
        // Optional: Detail why
        if (tempC < 18.0) Serial.println(" - Reason: Temp too Low (Cold)");
        if (tempC > 25.0) Serial.println(" - Reason: Temp too High (Hot)");
        if (humid < 40.0) Serial.println(" - Reason: Humidity too Low (Dry)");
        if (humid > 60.0) Serial.println(" - Reason: Humidity too High (Humid)");
    } 
    else if (tempPerfect && humidPerfect) {
        Serial.println("Perfect");
    } 
    else {
        // If not Danger and not Perfect, it's Warning
        Serial.println("Warning");
    }
    Serial.println("-------------------------");

    Serial.println();
}