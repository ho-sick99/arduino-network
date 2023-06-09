# 서버

## 서버 구조
```
📂 server
┣ 📂 bin
┃ ┗ www.js
┣ 📂 images
┣ 📂 src
┃ ┃ indes.ejs
┃ ┗ index.js
┣  app.js
┗  package.json
```
<br>

## 서버 경로
http://ec2-15-164-98-225.ap-northeast-2.compute.amazonaws.com/

## 서버 구동 방법 -> 클라우드에서 가동중
1. server 폴더 내에서 터미널 실행 ( server 폴더 켠 후 마우스 우측버튼 -> 터미널에서 열기 ) <br>
2. 터미널에 npm install 입력 ( 서버 가동에 필요한 모듈들 npm 서버에서 다운로드 ) <br>
3. 터미널에 npm start 입력 ( 현재 디바이스에서 서버 가동 )<br>
<br> 

## 서버 작동 시나리오
1. 서버 구동
2. 아두이노에서 데이터 전송 시, 서버에서 이를 확인 
    - "[서버 IP]/arduino" URL로 요청 시 확인 가능
    - src 폴더 내의 index.js 파일에서 이를 수행
3. 전송받은 데이터 가공
    - 가스 센서값 확인
    - 사진 데이터 디코딩 후 images 폴더 내에 저장
4. 아두이노에 응답 전송 ( 정상 작동 or 오류 발생 )
5. 사용자는 서버에 접속 시 로그(가스값, 사진) 확인 가능