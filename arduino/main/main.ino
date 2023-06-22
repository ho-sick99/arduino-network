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
    0x12, 0xAC, 0xAD, 0xFF, 0x22, 0x37 // MAC address (example)
};

// Server address setting
const PROGMEM char serverAddress[] = "15.164.98.225"; // server address -> AWS EC2 IP
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
    ipManager.initIP(mac); // IP 할당
    // 이더넷 연결 완료 //

    if (!lox.begin())
    {
        Serial.println(F("Failed to boot VL53L0X"));
        while (1)
            ;
    }
}

void loop()
{
    gasValue1 = analogRead(gas1);
    gasValue2 = analogRead(gas2);
    gasValue3 = analogRead(gas3);
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);

    VL53L0X_RangingMeasurementData_t measure;

    lox.rangingTest(&measure, false);
    int distance = measure.RangeMilliMeter;

    // 최대 가스값 체크 (maxValue)
    int maxValue = max(gasValue1, gasValue2);
    maxValue = max(maxValue, gasValue3);

    // 센서값 탐지 -> 가스 특정값 이상이면 내부 동작
    if (gasValue1 > 230 || gasValue2 > 450 || gasValue3 > 130)
    {

        if (distance < 300)
        { // 거리 300이하
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
    }
    else
    {
        delay(1000);
        svState--;

        if (svState < 0)
        {
            svState = 0;
        }
    }

    // svState에 따라 작동 제어
    if (svState == 1)
    {
        Serial.println("현재 안전합니다.");

        // 녹색 점등, 적색 소등
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledRed, LOW);

        // 모터 off
        digitalWrite(motorA, LOW);
        digitalWrite(motorB, LOW);

    }
    else if (svState > 3)
    {
        Serial.println("경고!!! 실내 흡연자 발생!!");

        // 적색 점등, 녹색 소등
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);

        // 모터 on
        digitalWrite(motorA, HIGH);
        digitalWrite(motorB, LOW);

        // 피에조 작동
        tone(buzzer, 700, 10500);

        // noTone(buzzer); //시끄러우면 끄기(임시코드)

    }

    delay(1000);

    while (1) // 스핀락
    {
        delay(1); // 레이져 거리센서
    }
}
