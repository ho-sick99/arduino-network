#include <SoftwareSerial.h> //아두이노 2개 연결하기 위한 라이브러리
#include <Servo.h> //서보모터 라이브라리
//#include <Adafruit_VC0706.h> //카메라 라이브러리 1
//#include <Adafruit_OV7670.h> //카메라 라이브러리 2

//카메라 객체 생성
//Adafruit_OV7670 camera;

//아두이노 2개 연결 핀
//const int RxPin = 5;
//const int TxPin = 4;

//RGB led 연결 핀
int ledRed = 9;//red 
int ledGreen = 8; //green 

//가스 센서 3개 각각의 연결 핀
int gas1=A0; //가스센서 1번 연결 핀
int gas2=A1; //가스센서 2번 연결 핀
int gas3=A2; //가스센서 3번 연결 핀

//피에조 부저 연결 핀
int buzzer = 7;

// ??? 모터 연결 핀 
int motor = 2;

//서보 모터 변수 및 상태 변수 svState;
Servo sv;
int svState = 0;

//AC모터 변수 및 상태 변수 acState
int acState = 0;

//안전상태에서 부저 1회만 울리게 하기 위해 상태변수사용
int isSafe = 1;


void setup() {
  Serial.begin(9600);       // 시리얼 통신 초기화. 
  pinMode(buzzer,OUTPUT);   // 피에조는 출력센서.
  pinMode(gas1, INPUT);     // 가스 1,2,3은 입력센서.
  pinMode(gas2, INPUT); 
  pinMode(gas3, INPUT);
  pinMode(ledRed, OUTPUT);  // led는 출력센서.
  pinMode(ledGreen, OUTPUT);
  
  // 카메라 모듈 초기화
  // if (!camera.begin(OV7670_CAM)) {
  //   Serial.println(F("카메라 모듈을 찾을 수 없습니다. 연결을 확인하세요."));
  //   while (1);
  // }
  // 카메라 모듈 해상도 설정 (QQVGA, QVGA, VGA 등)
  //camera.setResolution(OV7670_320x240);  // 원하는 해상도로 변경 가능
}

void loop() {
  // 버튼 또는 다른 트리거 이벤트를 기반으로 사진을 찍을 수 있습니다. 
  // 사진 촬영
  //camera.takePicture();

  // 사진 데이터 가져오기
  // uint8_t* buffer;
  // uint8_t length = camera.readPicture(buffer);

  // 사진 데이터를 원하는 형식으로 사용하거나 저장할 수 있습니다.
  // 사진 데이터를 시리얼 모니터에 출력
  // for (uint8_t i = 0; i < length; i++) {
  //   Serial.write(buffer[i]);
  // }
  // // 메모리 해제
  // camera.freePicture();

  delay(1000); // 사진 찍기 간격을 조절할 수 있습니다.

  //가스센서 항시 감지 (시리얼 모니터 에서 확인) 
  Serial.print("gas1 : ");
  Serial.print(analogRead(gas1)); //디폴트 : 약 197
  Serial.print(", gas2 : ");      //디폴트 : 약 190
  Serial.print(analogRead(gas2)); //디폴트 : 약 50
  Serial.print(", gas3 : ");
  Serial.println(analogRead(gas3));

  //센서값 탐지 -> 센서 3개중 한개라도 100이상이면 if문 내부 동작
  if(analogRead(gas1)>198 || analogRead(gas2)>190 || analogRead(gas3)>50){ //기준치 100으로 설정(적당히 바꾸세요)
    delay(1000);
    svState++;
  }
  else{
    delay(1000);
      svState--;
    
    if(svState<0){
      svState=0;
    }
  }

  //svState에 따라 작동 제어
  if(svState==1){
    Serial.println("현재 안전합니다.");
   
    //카메라촬영 사진 이미지 넘기기 센서값 넘기기 이부분에 코드 추가

    //모터 작동
    digitalWrite(motor,255);

    //녹색 점등, 적색 소등
    digitalWrite(ledGreen,HIGH);
    digitalWrite(ledRed,LOW);

    sv.write(180); //서보모터
    delay(1000);
    sv.detach(); //서보모터 1초 동작 후 멈춤
}
  else if(svState>3){
      Serial.println("경고!!! 실내 흡연자 발생!!");
     //카메라촬영 사진 이미지 넘기기 센서값 넘기기 이부분에 코드 추가

      //적색 점등, 녹색 소등
      digitalWrite(ledRed,HIGH);
      digitalWrite(ledGreen,LOW);
      //피에조 작동
      tone(buzzer,700,10500); 
      
      noTone(buzzer); //시끄러우면 끄는곳

      //서보모터
      sv.write(0);
      delay(1000);
      sv.detach(); //서보모터 1초 동작 후 멈춤
    }
}
