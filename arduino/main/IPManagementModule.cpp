#include <Ethernet.h>
#include "IPManagementModule.h"

// IP 발급 메서드
void IPManagementModule::initIP(byte *mac)
{
    Serial.print("Mac ip address: "); // mac 주소 출력
    for (int i = 0; i < 7; i++)
    {
        Serial.print(mac[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    Serial.println("이더넷 연결 시도");
    int status = Ethernet.begin(mac);
    Serial.println(status);
    if (status == 1)
    {
        Serial.print("My IP Address: ");
        Serial.println(Ethernet.localIP());
    }
    else if (status == 0) // 연결 실패
    {
        Serial.println("Failed to configure Ethernet using DHCP"); //

        // 예외처리
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
        }
        else if (Ethernet.linkStatus() == LinkOFF)
        {
            Serial.println("Ethernet cable is not connected.");
        }
        Serial.println("아두이노 정지");

        // 아두이노 정지
        while (true)
        {
            delay(1);
        }
    }
}
// IP 관리 메서드
void IPManagementModule::maintainIP()
{
    Serial.println("IP Check");
    // IP 주소 변동 체크
    switch (Ethernet.maintain())
    {
    case 1: // renewed fail
        Serial.println("Error: renewed fail");
        break;
    case 2: // renewed success
        Serial.println("Renewed success");
        // IP 주소 출력
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;
    case 3: // rebind fail
        Serial.println("Error: rebind fail");
        break;
    case 4: // rebind success
        Serial.println("Rebind success");
        // IP 주소 출력
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;
    default: // nothing happened
        break;
    }
}
