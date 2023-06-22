#include <SoftwareSerial.h> //아두이노 2개 연결하기 위한 라이브러리
#include <Servo.h>          //서보모터 라이브라리
// #include "Adafruit_VL53L0X.h"

// Ethernet Modules
#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

// Ethernet headers
#include "IPManagementModule.h"
#include "SendDataModule.h"

// Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// RGB led 연결 핀
int ledRed = 9;   // red
int ledGreen = 8; // green

// 가스 센서 3개 각각의 연결 핀
int gas1 = A0; // 가스센서 1번 연결 핀
int gas2 = A1; // 가스센서 2번 연결 핀
int gas3 = A2; // 가스센서 3번 연결 핀

// 가스 센서 값
int gasValue1 = 0; // 1번 가스 센서 값
int gasValue2 = 0; // 2번 가스 센서 값
int gasValue3 = 0; // 3번 가스 센서 값

// 피에조 부저 연결 핀
int buzzer = 7;

// ??? 모터 연결 핀
int motorA = 2;
int motorB = 3;

// 서보 모터 변수 및 상태 변수 svState;
Servo sv;
int svState = 0;

// AC모터 변수 및 상태 변수 acState
int acState = 0;

// 안전상태에서 부저 1회만 울리게 하기 위해 상태변수사용
int isSafe = 1;

// 이더넷 연결 세팅 //
// MAC address setting
byte mac[] = {
    0x12, 0xAC, 0xAD, 0xFF, 0x22, 0x37 // MAC address (example)
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
    pinMode(motorB, OUTPUT);
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);

    while (!Serial) // 시리얼 포트 연결확인
    {
        delay(1); // 레이져 거리센서
    }

    // // 이더넷 연결 시작 //
     Serial.println("Initialize Ethernet with DHCP");
     ipManager.initIP(mac); // IP 할당

    // Serial.print("My IP address: ");
    // Serial.println(Ethernet.localIP()); // 할당받은 IP 주소 출력
    // // 이더넷 연결 완료 //

    Serial.println("Adafruit VL53L0X test");

    //    if (!lox.begin())
    //    {
    //        Serial.println(F("Failed to boot VL53L0X"));
    //        while (1)
    //            ;
    //    }
    // power
    Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}

void loop()
{
    gasValue1 = analogRead(gas1);
    gasValue2 = analogRead(gas2);
    gasValue3 = analogRead(gas3);
    Serial.print("gas1 : ");
    Serial.print(gasValue1);   // 디폴트 : 약 197
    Serial.print(", gas2 : "); // 디폴트 : 약 190
    Serial.print(gasValue2);   // 디폴트 : 약 50
    Serial.print(", gas3 : ");
    Serial.println(gasValue3);

    // VL53L0X_RangingMeasurementData_t measure;

    // lox.rangingTest(&measure, false);

    // 센서값 탐지 -> 센서 3개중 한개라도 100이상이면 if문 내부 동작
    // if (measure.RangeMilliMeter < 300 && (gasValue1 > 230 || gasValue2 > 450 || gasValue3 > 130)) // -> 아래줄 이걸로 바꿔야해요
    if (gasValue1 > 0)
    { // 기준치 100으로 설정(적당히 바꾸세요)
        delay(1000);
        svState++;

        // 최대 가스값 체크
        int maxValue = max(gasValue1, gasValue2);
        maxValue = max(maxValue, gasValue3);

        Serial.print("Gas value : ");
        Serial.print(maxValue);

        // 이더넷 통신 시도 //
        ipManager.maintainIP(); // IP 체크

        dataModule.send(&client, maxValue); // 최대 가스값 전송

        // 이더넷 통신 결과값 반환받기
        int statusCode = client.responseStatusCode();
        String response = client.responseBody();

        Serial.print("Status code: ");
        Serial.println(statusCode); // 상태코드 출력
        Serial.print("Response: ");
        Serial.println(response); // 반환값 출력
        // 이더넷 통신 완료 //
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

        // 카메라촬영 사진 이미지 넘기기 센서값 넘기기 이부분에 코드 추가

        // 모터 작동

        // 녹색 점등, 적색 소등
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledRed, LOW);
        digitalWrite(motorA, LOW);
        digitalWrite(motorB, LOW);

        sv.write(1800); // 서보모터
        delay(1000);
        sv.detach(); // 서보모터 1초 동작 후 멈춤
    }
    else if (svState > 3)
    {
        Serial.println("경고!!! 실내 흡연자 발생!!");

        // 적색 점등, 녹색 소등
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(motorA, HIGH);
        digitalWrite(motorB, LOW);

        // 피에조 작동
        tone(buzzer, 700, 10500);

        // noTone(buzzer); //시끄러우면 끄는곳

        // 서보모터
        sv.write(0);
        delay(1000);
        sv.detach(); // 서보모터 1초 동작 후 멈춤
    }

    delay(1000);

    while (1) // 스핀락
    {
        delay(1); // 레이져 거리센서
    }
}
