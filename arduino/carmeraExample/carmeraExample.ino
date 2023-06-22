#include <Adafruit_VC0706.h>
#include <Adafruit_OV7670.h>

Adafruit_OV7670 camera;

void setup()
{
  // 시리얼 통신 초기화
  Serial.begin(9600);

  // 카메라 모듈 초기화
  if (!camera.begin(OV7670_CAM))
  {
    Serial.println(F("카메라 모듈을 찾을 수 없습니다. 연결을 확인하세요."));
    while (1)
      ;
  }

  // 카메라 모듈 해상도 설정 (QQVGA, QVGA, VGA 등)
  camera.setResolution(OV7670_320x240); // 원하는 해상도로 변경 가능
}

void loop()
{
  // 버튼 또는 다른 트리거 이벤트를 기반으로 사진을 찍을 수 있습니다.

  // 사진 촬영
  camera.takePicture();

  // 사진 데이터 가져오기
  uint8_t *buffer;
  uint8_t length = camera.readPicture(buffer);

  // 사진 데이터를 시리얼 모니터에 출력
  for (uint8_t i = 0; i < length; i++)
  {
    Serial.write(buffer[i]);
  }

  // 메모리 해제
  camera.freePicture();

  delay(1000); // 사진 찍기 간격을 조절할 수 있습니다.
}