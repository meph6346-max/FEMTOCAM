[English](INSTALL_EASY_EN.md) / 한국어

# FEMTOCAM 설치 가이드 (일반 사용자)

컴파일 없이, 빌드된 펌웨어를 ESP32-CAM-MB에 설치하는 방법입니다.

---

## 준비물

- **ESP32-CAM-MB** (ESP32-CAM + USB 도킹보드 세트)
- **MicroSD 카드** (FAT32 포맷, Class 10 이상)
- **Micro USB 케이블**

> ESP32-CAM-MB는 국내 쇼핑몰이나 AliExpress에서 5,000원 내외로 구할 수 있습니다.

---

## 1단계: 펌웨어 다운로드

[Releases](../../releases) 페이지에서 최신 `femtocam.bin` 파일을 다운로드합니다.

---

## 2단계: 펌웨어 설치

### 방법 A: ESP Web Flasher (가장 쉬움)

PC에 아무것도 설치하지 않고, 브라우저에서 바로 설치합니다.

1. ESP32-CAM을 도킹보드에 꽂습니다
2. USB로 PC에 연결합니다
3. Chrome 또는 Edge에서 [ESP Web Flasher](https://esp.huhn.me/) 접속
4. **Connect** → 시리얼 포트 선택 (CH340 또는 COM 포트)
5. Address에 `0x10000` 입력
6. 다운로드한 `femtocam.bin` 선택
7. **Program** 클릭 → 완료까지 대기

### 방법 B: ESP Flash Download Tool (Windows)

1. [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools) 다운로드 & 실행
2. **ESP32** → **Develop** 선택
3. 설정:
   - `femtocam.bin` → 주소: `0x10000`
   - SPI SPEED: 40MHz / SPI MODE: DIO
   - COM 포트 선택
4. **START** 클릭

---

## 3단계: 조립

1. MicroSD 카드를 ESP32-CAM 뒷면 슬롯에 삽입합니다
2. 도킹보드에 꽂은 채로 USB 전원을 연결하면 바로 사용 가능합니다

> 💡 **팁**: 도킹보드를 그대로 사용해도 대부분 문제없습니다. 만약 WiFi가 불안정하다면 도킹보드에서 분리하고 5V/GND 핀에 직접 전원을 공급해보세요.

### 전원 연결 방법

| 방법 | 설명 |
|------|------|
| USB → 도킹보드 | 가장 간편. 대부분 이걸로 충분 |
| 5V 어댑터 → 5V/GND 핀 | WiFi 불안정 시 권장 |
| 프린터 메인보드 5V | 프린터와 함께 켜짐/꺼짐 |

---

## 4단계: 첫 접속

1. 전원을 연결하면 ESP32-CAM의 **빨간 LED가 켜집니다** (AP 모드)
2. 스마트폰에서 WiFi **FEMTOCAM**에 접속 (비밀번호 없음)
3. 브라우저에서 `http://192.168.4.1/` 접속
4. 카메라 스트리밍이 보이면 성공!

> LED가 깜빡이면 WiFi 접속을 시도하는 중입니다. 계속 깜빡이다가 켜지면 AP 모드로 진입한 것입니다.

---

## 5단계: WiFi 설정 (선택)

작업실 WiFi에 연결하면 같은 네트워크의 어떤 기기에서든 접속할 수 있습니다.

1. Settings 탭 → **Scan** 버튼
2. WiFi 목록에서 네트워크 선택
3. 비밀번호 입력
4. **Save & reboot** 클릭
5. 재부팅 후 `http://femtocam.local/` 로 접속

> mDNS(`femtocam.local`)가 안 되는 경우, 공유기 관리 페이지에서 ESP32-CAM에 할당된 IP를 확인하세요.

---

## 문제 해결

| 증상 | 원인 | 해결 |
|------|------|------|
| LED가 빠르게 깜빡임 후 꺼지지 않음 | 카메라 초기화 실패 | 카메라 커넥터 재삽입 후 재부팅 |
| WiFi에 FEMTOCAM이 안 보임 | 부팅 실패 | 전원 분리 후 재연결 |
| 웹페이지가 느리거나 멈춤 | 도킹보드가 연결된 상태 | 도킹보드 분리 + 별도 5V |
| 스트림은 되는데 녹화 안 됨 | SD 카드 문제 | FAT32 포맷, Class 10 이상 확인 |
| VGA 스트림이 끊김 | WiFi 대역폭 부족 | QVGA로 변경하거나 공유기에 가까이 |

---

## SD 카드 없이 사용

SD 카드가 없어도 **스트리밍과 설정**은 정상 동작합니다. 녹화 기능만 비활성화됩니다.

---

[← README로 돌아가기](../README_KO.md)
