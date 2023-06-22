#include <ArduinoHttpClient.h>

#include "SendDataModule.h"

void SendDataModule::send(HttpClient *client, int gas, int lox, int mode)
{
    Serial.println("Send data to server");
    Serial.print("Gas sensor value: ");
    Serial.print(gas); // 가스 센서값 출력
    Serial.println();
    Serial.print("Lazer sensor value: ");
    Serial.print(lox); // 레이저 센서값 출력
    Serial.println();
    Serial.print("Mode: ");
    Serial.print(mode); // 가스 센서값 출력
    Serial.println();

    // Data 정의 -> 현재 테스트 데이터이며 추후에 사진 모듈 연동 성공시 사진 데이터를 base64 인코딩해서 전송예정
    String contentType = "application/x-www-form-urlencoded";
    String postData = "gas=" + String(gas) + "&lox=" + String(lox) + "&mode=" + String(mode);
    // Serial.print(postData);
    // 서버로 post 요청
    client->post("/arduino", contentType, postData);
}
