#include <SoftwareSerial.h> //아두이노 2개 연결하기 위한 라이브러리
#include "Adafruit_VL53L0X.h"

// Ethernet Modules
#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

// Ethernet headers
#include "IPManagementModule.h"
#include "SendDataModule.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// RGB led 연결 핀
const PROGMEM int ledRed = 9;   // red
const PROGMEM int ledGreen = 8; // green

// 가스 센서 3개 각각의 연결 핀
const PROGMEM int gas1 = A0; // 가스센서 1번 연결 핀
const PROGMEM int gas2 = A1; // 가스센서 2번 연결 핀
const PROGMEM int gas3 = A2; // 가스센서 3번 연결 핀

// 가스 센서 값
int gasValue1 = 0; // 1번 가스 센서 값
int gasValue2 = 0; // 2번 가스 센서 값
int gasValue3 = 0; // 3번 가스 센서 값

// 가스 센서 임계값
const PROGMEM int gasLimit1 = 230; // 1번 센서
const PROGMEM int gasLimit2 = 450; // 2번 센서
const PROGMEM int gasLimit3 = 0; // 3번 센서

// 피에조 부저 연결 핀
const PROGMEM int buzzer = 7;

// 모터 연결 핀
const PROGMEM int motorA = 2;
const PROGMEM int motorB = 3;

// 상태 변수 svState;
int svState = 0;

// 이더넷 연결 세팅 //
// MAC address setting
const PROGMEM byte mac[] = {
    0x04, 0xBC, 0xAA, 0xF2, 0x32, 0x52 // MAC address (example)
};

// Server address setting
char serverAddress[] = "15.164.98.225"; // server address -> AWS EC2 IP
int port = 80;                          // server port

// Ethernet Client Setting
EthernetClient c;
HttpClient client = HttpClient(c, serverAddress, port);

IPManagementModule ipManager; // IP 관리 모듈
SendDataModule dataModule;    // 데이터 전송 모듈
// 이더넷 연결 파트 끝 //

void setup()
{
    Serial.begin(9600);      // 시리얼 통신 초기화.
    pinMode(buzzer, OUTPUT); // 피에조는 출력센서.
    pinMode(gas1, INPUT);    // 가스 1,2,3은 입력센서.
    pinMode(gas2, INPUT);
    pinMode(gas3, INPUT);
    pinMode(ledRed, OUTPUT); // led는 출력센서.
    pinMode(ledGreen, OUTPUT);
    pinMode(motorA, OUTPUT);
    pinMode(motorB, OUTPUT); // 모터는 출력센서
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW); // 모터 최초 상태 OFF

    while (!Serial) // 시리얼 포트 연결확인
    {
        delay(1); // 레이져 거리센서
    }

    // 이더넷 연결 시작 //
    Serial.println("Initialize Ethernet with DHCP");
    // ipManager.initIP(mac); // IP 할당
    // 이더넷 연결 완료 //

    if (!lox.begin())
    {
        Serial.println(F("Failed to boot VL53L0X"));
        while (1)
            ;
    }
    
    Serial.println(String(serverAddress));      // 시리얼 통신 초기화.
}

void loop()
{
    // 가스 센서값 측정
    gasValue1 = analogRead(gas1);
    gasValue2 = analogRead(gas2);
    gasValue3 = analogRead(gas3);

    // 가스 센서값 출력
    Serial.print("gas1 : ");
    Serial.print(gasValue1);   // 디폴트 : 약 197
    Serial.print(", gas2 : "); 
    Serial.print(gasValue2);   // 디폴트 : 약 190
    Serial.print(", gas3 : ");
    Serial.println(gasValue3);  // 디폴트 : 약 50
    
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);

    VL53L0X_RangingMeasurementData_t measure;

    lox.rangingTest(&measure, false);
    int distance = measure.RangeMilliMeter;

    // 최대 가스값 체크 (maxValue)
    int maxValue = max(gasValue1, gasValue2);
    maxValue = max(maxValue, gasValue3);

    // 센서값 탐지 -> 가스 특정값 이상이면 내부 동작
    if (gasValue1 > gasLimit1 || gasValue2 > gasLimit2 || gasValue3 > gasLimit3)
    {
        // LED 점등
        // 녹색 점등, 적색 소등
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledRed, HIGH);
        
        // 피에조 부저 작동
        tone(buzzer, 700, 10500);

        // 모터 작동
        // 모터 on
        digitalWrite(motorA, HIGH);
        digitalWrite(motorB, LOW);
        
        if (distance < 300) // 거리 300이하
        { 
            Serial.println("사람이 감지되었습니다.");
            svState++;

            //// 이더넷 통신//
            ipManager.maintainIP(); // IP 체크

            dataModule.send(&client, maxValue, distance, 0); // 가스, 레이저 센서 값, 모드 전송

            // 이더넷 통신 결과값 반환받기
            int statusCode = client.responseStatusCode();
            String response = client.responseBody();

            Serial.print("Status code: ");
            Serial.println(statusCode); // 상태코드 출력
            Serial.print("Response: ");
            Serial.println(response); // 반환값 출력
            ///// 이더넷 통신 완료 //
        }
        else
        {
            Serial.println("가스가 감지되었습니다.");
            svState++;
            //// 이더넷 통신//
            ipManager.maintainIP(); // IP 체크

            dataModule.send(&client, maxValue, distance, 1); // 가스, 레이저 센서 값, 모드 전송

            // 이더넷 통신 결과값 반환받기
            int statusCode = client.responseStatusCode();
            String response = client.responseBody();

            Serial.print("Status code: ");
            Serial.println(statusCode); // 상태코드 출력
            Serial.print("Response: ");
            Serial.println(response); // 반환값 출력
            ///// 이더넷 통신 완료 //
        }
        
        delay(60000); // 1분 대기
    }
    else
    {
        Serial.println("현재 안전합니다.");

        // LED
        // 녹색 점등, 적색 소등
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledRed, LOW);

        // 모터
        // 모터 off
        digitalWrite(motorA, LOW);
        digitalWrite(motorB, LOW);
        
        noTone(buzzer); //시끄러우면 끄기(임시코드)
        
        delay(1000);

    }
}
