#include <ArduinoHttpClient.h>

#include "SendDataModule.h"

void SendDataModule::send(HttpClient *client, int gas)
{
    Serial.println("Send data to server");
    Serial.print("Gas sensor value: ");
    Serial.print(gas); // 가스 센서값 출력
    Serial.println();
    Serial.print("Photo");

    // Data 정의 -> 현재 테스트 데이터이며 추후에 사진 모듈 연동 성공시 사진 데이터를 base64 인코딩해서 전송예정
    String contentType = "application/x-www-form-urlencoded";
    String postData = "gas=" + String(gas) + "&image=89504e470d0a1a0a0000000d48936cfe1f171e4a92e6ff01f5407b76dfe2421a0000000049454e44ae426082";
    Serial.print(postData);
    // 서버로 post 요청
    client->post("/arduino", contentType, postData);
}
