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
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    Wire.begin();
    lightMeter.begin();
    Serial.println(F("BH1750 Test begin"));

    unsigned status;
    
    // default settings
    status = bme.begin(0x76);  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();
}


void loop() { 
    printValues();
    delay(delayTime);
}

void printValues() {
    // Read values
    float tempC = bme.readTemperature();
    float humid = bme.readHumidity();


    // Print raw values
    Serial.print("Temperature = ");
    Serial.print(tempC);
    Serial.println(" °C");



    Serial.print("Humidity = ");
    Serial.print(humid);
    Serial.println(" %");

    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.print(" lx - ");

    if (lux >= 50) {
        Serial.println("bad");
    } else if (lux >= 30) {
        Serial.println("careful");
    } else if (lux >= 10) {
        Serial.println("great");
    } else {
        Serial.println("excellent");
    }

    // Evaluation Logic
    // Perfect: 20-23°C AND 45-55%
    // Warning: (18-19°C OR 24-25°C) OR (40-44% OR 56-60%)
    // Danger: <18°C OR >25°C OR <40% OR >60%

    // Determine Temperature Status
    int tempStatus = 0; // 0: Perfect, 1: Warning, 2: Danger
    if (tempC >= 20.0 && tempC <= 23.0) {
        tempStatus = 0;
    } else if ((tempC >= 18.0 && tempC < 20.0) || (tempC > 23.0 && tempC <= 25.0)) {
        tempStatus = 1;
    } else {
        tempStatus = 2; // < 18 or > 25
    }

    // Determine Humidity Status
    int humStatus = 0; // 0: Perfect, 1: Warning, 2: Danger
    if (humid >= 45.0 && humid <= 55.0) {
        humStatus = 0;
    } else if ((humid >= 40.0 && humid < 45.0) || (humid > 55.0 && humid <= 60.0)) {
        humStatus = 1;
    } else {
        humStatus = 2; // < 40 or > 60
    }

    // Overall Status Logic: Worst case determines status
    // Danger overrides everything.
    // Warning overrides Perfect.
    Serial.println("-------------------------");
    Serial.print("Status: ");

    if (tempStatus == 2 || humStatus == 2) {
        Serial.println("Danger");
        // Optional: Detail why
        if (tempC < 18.0) Serial.println(" - Reason: Temp too Low (Cold)");
        if (tempC > 25.0) Serial.println(" - Reason: Temp too High (Hot)");
        if (humid < 40.0) Serial.println(" - Reason: Humidity too Low (Dry)");
        if (humid > 60.0) Serial.println(" - Reason: Humidity too High (Humid)");
    } 
    else if (tempStatus == 1 || humStatus == 1) {
        Serial.println("Warning");
    } 
    else {
        Serial.println("Perfect");
    }
    Serial.println("-------------------------");

    Serial.println();
}
