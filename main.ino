// [Main.ino]
// 여기에 모든 변수를 다 선언해둡니다. (공유 창고)
float shockValue = 0;   // 센서 친구가 채울 값
float tempValue = 0;    // 센서 친구가 채울 값
bool bleConnected = false; // 통신 상태 확인용

void setup() {
  Serial.begin(115200);

  // 1. 센서 설정 함수 호출 (Sensor.ino에 있는 것)
  setupSensors(); 
  
  // 2. 통신 설정 함수 호출 (Comm.ino에 있는 것)
  setupBLE();     
}

void loop() {
  // 1. 센서 값 읽어오기 (Sensor.ino에 있는 함수)
  readSensors(); 

  // 2. 통신으로 보내기 (Comm.ino에 있는 함수)
  runBLE();      
}