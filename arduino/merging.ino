#include <SoftwareSerial.h> 
#include <Servo.h> //서보모터 라이브라리
#include <WiFi.h>
#include "esp_camera.h"
#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include "IPManagementModule.h" //ip 관리모듈

#define CAMERA_MODEL_AI_THINKER //
#include "camera_pins.h"

const char* ssid = "sexyjinyoung"; //ESP32는 자체적으로 wifi연결 칩같은게 있나봄 ㅇㅇ
const char* password = "dlsqja11@@!!"; //ㅇㅇ 

void startCameraServer();

//새로운 시리얼 포트를 위한 변수들, 그리고 새로운 시리얼 선언 
const int rxPin = 2; //2,3 해도 되는 포트 맞는지 아닌지 모르겟음 
const int txPin = 3; 
SoftwareSerial cameraSerial(rxPin, txPin);

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
int motor = 5; 

//서보 모터 변수 및 상태 변수 svState;
Servo sv;
int svState = 0;

//AC모터 변수 및 상태 변수 acState
int acState = 0;

//안전상태에서 부저 1회만 울리게 하기 위해 상태변수사용
int isSafe = 1;

// Ethernet 설정
byte mac[] = {
    0x02, 0xAB, 0xCD, 0xEF, 0x12, 0x34 // MAC address (example)
};
char serverAddress[] = "220.92.63.82"; // server address
int port = 3000;                       // server port
EthernetClient client;
HttpClient httpClient = HttpClient(client, serverAddress, port);
IPManagementModule ipManager; // IP 관리 모듈
SendDataModule dataModule;    // 데이터 전송 모듈


void setup() {
  Serial.begin(9600);       // 시리얼 통신 초기화. 
  //문제점 : 아두이노 우노는 시리얼 포트가 한쌍만 존재. 따라서 디지털 핀을 시리얼 통신 핀으로 사용해야 함. https://kocoafab.cc/fboard/view/2559
  //시도한 방법 -> https://juahnpop.tistory.com/90
  cameraSerial.begin(115200); //카메라 시리얼 통신 열기
  cameraSerial.setDebugOutput(true);
  cameraSerial.println();

  /////
  /////카메라 기본설정 시작 (예제코드 그대로 가져옴)
  /////
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; //이게 제발 ESP32의 GPIO를 말하는 거면 좋겠다. 이런식으로 입출력포트를 재설정했는데 이게 아두이노 우노 꺼면 ㅈ댐
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  }
  /////
  /////카메라 기본설정 끝 (예제코드 그대로 가져옴)
  /////

  //피에조, 가스, LED 센서를 출력 센서라고 알려줌.
  pinMode(buzzer,OUTPUT);   
  pinMode(gas1, INPUT);    
  pinMode(gas2, INPUT); 
  pinMode(gas3, INPUT);
  pinMode(ledRed, OUTPUT);  
  pinMode(ledGreen, OUTPUT);
  
  // Ethernet 연결 시작, 설정, 할당받은 IP주소 출력
  Serial.println("Initialize Ethernet with DHCP");
  ipManager.initIP(mac); // IP 할당
  Ethernet.begin(mac); //이더넷 시작
  Serial.print("My IP address: "); // 할당받은 IP 주소 출력
  Serial.println(Ethernet.localIP());

}

void loop() {
  delay(1000); // 사진 찍기 간격을 조절할 수 있습니다.

  //현재 연결이 정상적인지 계속 확인하고 있음을 시리얼 모니터에서 확인
  ipManager.maintainIP(); // IP 체크
  dataModule.send(&client, 140); // post request
  int statusCode = client.responseStatusCode(); // read the status code 
  String response = client.responseBody(); //read the body of the response
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  //가스센서 항시 감지되고 있음을 시리얼 모니터 에서 확인. 잘되는거 확인되면 추후에 그냥 주석처리하거나 지워버려도 됨
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

      //사진촬영 및 버퍼 저장. 
      //jpeg 인코딩 미완성
      camera_fb_t *fb = esp_camera_fb_get(); 
      uint8_t *buffer;
      fb->buffer; //아마도 버퍼에 사진이 저장되었음

      if (!fb) { //사진 촬영 실패 상황시 에러 출력(ㅇㄷ에?)
        cameraSerial.println("사진 캡처 실패!");
        return;
      }
      // 캡처된 이미지 정보 출력(어디에..???? 시리얼 모니터 2개 켜기 가능한가??)
      cameraSerial.printf("캡처된 이미지 해상도: %dx%d, 픽셀 형식: %d\n", fb->width, fb->height, fb->format);

      //적색 점등, 녹색 소등
      digitalWrite(ledRed,HIGH);
      digitalWrite(ledGreen,LOW);

      // 데이터 전송 
      String data = "경고!!! 실내 흡연자 발생!!";
      dataModule.send(&client, analogRead(gas1)); //가스값 전송
      dataModule.send(&client, analogRead(gas2));
      dataModule.send(&client, analogRead(gas3));
      dataModule.send($client, buffer); //사진 데이터 전송
      //여기에서 아두이노 버퍼 비워야함


      //아래 코드는 뭔지 모르겠음!! 이 주석을 보는 사람은 설명 좀 부탁합니다.
      // httpClient.beginRequest();
      // httpClient.post("/");
      // httpClient.sendHeader("Content-Type", "text/plain");
      // httpClient.sendHeader("Content-Length", data.length());
      // httpClient.sendHeader("Content-Length", data.length());
      // httpClient.beginBody();
      // httpClient.print(data);
      // httpClient.endRequest();


      // 사진 데이터 메모리 해제
      esp_camera_fb_return(fb); 


      //피에조 작동
      tone(buzzer,700,10500); 
      //noTone(buzzer); //시끄러우면 끄는곳

      //서보모터
      sv.write(0);
      delay(1000);
      sv.detach(); //서보모터 1초 동작 후 멈춤


    }
}
