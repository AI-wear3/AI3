// [Comm.ino]
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// 통신용 변수들은 여기에 숨겨둠
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool oldDeviceConnected = false;

#define SERVICE_UUID        "dfb046dc-faa3-45e4-a75a-0e3175d22c4c"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      bleConnected = true; // Main에 있는 변수 변경
    };
    void onDisconnect(BLEServer* pServer) {
      bleConnected = false;
    }
};

// [설정 함수] setup()에서 불려질 녀석
void setupBLE() {
  BLEDevice::init("Smart_Baby_Monitor");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY 
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println(">> 블루투스 준비 완료");
}

// [실행 함수] loop()에서 계속 불려질 녀석
void runBLE() {
  // 연결되었고, 값이 위험하면 알림 보내기
  if (bleConnected) {
    if (shockValue > 50) { // Main에 있는 변수 shockValue 사용
      String msg = "EMERGENCY! Shock: " + String(shockValue);
      pCharacteristic->setValue(msg.c_str());
      pCharacteristic->notify();
      delay(1000); 
    }
  }

  // 재연결 처리
  if (!bleConnected && oldDeviceConnected) {
      delay(500);
      pServer->startAdvertising();
      oldDeviceConnected = bleConnected;
  }
  if (bleConnected && !oldDeviceConnected) {
      oldDeviceConnected = bleConnected;
  }
}