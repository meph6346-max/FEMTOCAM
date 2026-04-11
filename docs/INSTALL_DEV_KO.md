[English](INSTALL_DEV_EN.md) / 한국어

# FEMTOCAM 설치 가이드 (고급 사용자)

소스 코드를 직접 빌드하여 설치하는 방법입니다.

---

## 준비물

- **AI Thinker ESP32-CAM** + MicroSD
- **ESP32-CAM-MB** 또는 FTDI 어댑터
- **VS Code** + **PlatformIO** 확장
- **Git**

---

## 1단계: 소스 클론

```bash
git clone https://github.com/gorogepapa/femtocam.git
cd femtocam
```

---

## 2단계: 빌드 & 업로드

```bash
pio run --target upload
```

> ESP32-CAM-MB 사용 시 자동 업로드. FTDI 사용 시 IO0를 GND에 연결한 상태에서 업로드 후 제거.

---

## 프로젝트 구조

```
femtocam/
├── platformio.ini
├── README.md / README_KO.md
├── LICENSE
├── docs/
│   ├── images/
│   ├── INSTALL_EASY_KO.md / INSTALL_EASY_EN.md
│   └── INSTALL_DEV_KO.md / INSTALL_DEV_EN.md
├── include/
│   ├── config.h          핀맵, 상수, 구조체
│   ├── avi_writer.h      MJPEG AVI 컨테이너
│   └── web_ui.h          웹UI HTML/CSS/JS + i18n
└── src/
    └── main.cpp          펌웨어 전체
```

---

## 주요 파일 설명

| 파일 | 역할 | 수정 시나리오 |
|------|------|-------------|
| `config.h` | 핀 배치, 해상도, 기본값, 구조체 | 새 해상도, 기본값 변경 |
| `main.cpp` | 전체 펌웨어, HTTP 핸들러 | 엔드포인트 추가, 로직 수정 |
| `web_ui.h` | 웹UI (HTML+CSS+JS+i18n) | UI 레이아웃, 스타일, 번역 |
| `avi_writer.h` | AVI 컨테이너 | 영상 포맷 관련 |

---

## API 레퍼런스

웹 서버: 포트 80 / 스트림 서버: 포트 81. 모든 엔드포인트는 HTTP GET.

| 엔드포인트 | 설명 |
|-----------|------|
| `/` | 웹UI |
| `/stream` (포트 81) | MJPEG 스트리밍 |
| `/status` | 시스템 상태 JSON |
| `/start` · `/stop` | 녹화 시작 / 정지 |
| `/set?res=VGA&capMs=100&pbFps=15&dur=60&hostname=cam1` | 설정 변경 |
| `/flash?on=1` | 플래시 LED |
| `/files` | 파일 목록 JSON |
| `/dl?f=파일명` | 다운로드 |
| `/rm?f=파일명` | 삭제 |
| `/rename?f=기존&n=새이름` | 이름 변경 |
| `/cam` · `/camset?brightness=1` | 카메라 설정 조회/변경 |
| `/recparams` · `/recset?sdMinMB=10` | 녹화 안전 설정 조회/변경 |
| `/log` · `/logclear` · `/loglevel?v=2` | 로그 조회/초기화/레벨 |
| `/sysinfo` | 시스템 정보 |
| `/scan` | WiFi 스캔 |
| `/wifi?ssid=...&pass=...` | WiFi 저장 & 재부팅 |
| `/reset` | 공장 초기화 & 재부팅 |

---

## 새 언어 추가

`web_ui.h`의 `<script>` 섹션에서 `var L={...}` 블록을 찾아 새 언어를 추가합니다.

1. `ko` 블록을 복사
2. 키 변경 (예: `ja`, `de`, `zh`)
3. 값 번역
4. `toggleLang()` 함수에서 새 언어 순환 추가

---

## FTDI 어댑터 배선 (ESP32-CAM-MB 없이)

| FTDI | ESP32-CAM |
|------|-----------|
| 5V | 5V |
| GND | GND |
| TX | U0R (GPIO 3) |
| RX | U0T (GPIO 1) |
| — | IO0 → GND (플래시 모드) |

1. IO0를 GND에 연결
2. 전원 재연결
3. `pio run --target upload`
4. **IO0-GND 연결 제거**
5. RST로 정상 부팅

---

[← README로 돌아가기](../README_KO.md)
