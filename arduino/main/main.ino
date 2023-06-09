#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

#include "IPManagementModule.h"
#include "SendDataModule.h"

// MAC address setting
byte mac[] = {
    0x02, 0xAB, 0xCD, 0xEF, 0x12, 0x34 // MAC address (example)
};

// Server address setting
char serverAddress[] = "15.164.98.225"; // server address -> AWS EC2 IP
int port = 80;                       // server port

// Ethernet Client Setting
EthernetClient c;
HttpClient client = HttpClient(c, serverAddress, port);

IPManagementModule ipManager; // IP 관리 모듈
SendDataModule dataModule;    // 데이터 전송 모듈

void setup()
{
    Serial.begin(9600); // 시리얼 통신 초기화

    while (!Serial) // 시리얼 포트 연결확인
    {
        ;
    }

    // 이더넷 연결 시작
    Serial.println("Initialize Ethernet with DHCP");
    ipManager.initIP(mac); // IP 할당

    // 할당받은 IP 주소 출력
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
    ipManager.maintainIP(); // IP 체크

    // post request
    dataModule.send(&client, 140);

    // read the status code and body of the response
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    // Retry delay
    delay(5000);
}