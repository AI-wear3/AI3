#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "dfb046dc-faa3-45e4-a75a-0e3175d22c4c"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
// 터치 센서 핀 설정 (예: GPIO 3).
#define TOUCH_PIN 3 

// 터치 임계값 수정: 30000 이상일 때 알림
#define TOUCH_THRESHOLD 30000 

BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  Serial.println("Start BLE Service");

  BLEDevice::init("XIAO_ESP32S3");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setValue("weareable");
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  
  // 디버깅을 위해 평소 값을 계속 확인하세요
  Serial.print("Touch Value: ");

  Serial.println(touchValue);

  if (touchValue >= TOUCH_THRESHOLD) {
    String signalMessage = "SOS"; 
    pCharacteristic->setValue(signalMessage.c_str());
    pCharacteristic->notify();
    Serial.println("Sent Signal to Client!");
    delay(500); 
  }
  
  delay(100);
}


