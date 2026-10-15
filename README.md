# OhHangulKey (한글키 유틸)

> **우측 Alt, 우측 Ctrl 키가 없는 키보드(미니 배열, 텐키리스, 외산/해외 직구 키보드 등)를 위한 초경량 전역 키 매핑 및 트레이 유틸리티**

[![Build and Release](https://github.com/ohyungkr/OhHangulKey/actions/workflows/build.yml/badge.svg)](https://github.com/ohyungkr/OhHangulKey/actions/workflows/build.yml)
[![Platform](https://img.shields.io/badge/Platform-Windows%207%20%7C%208%20%7C%2010%20%7C%2011-blue.svg)](https://microsoft.com)
[![Dependency](https://img.shields.io/badge/Dependencies-Zero%20(Pure%20Win32%20API)-success.svg)]()
[![Architecture](https://img.shields.io/badge/Arch-x64%20%2F%20x86-orange.svg)]()
[![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)]()

---

## 📌 주요 특징

- **초경량 독립 실행 바이너리 (Zero-Dependency)**
  - C/C++ 순수 Win32 API 및 `/MT`(정적 C 런타임 링크)로 컴파일되어 약 **180KB**의 단일 `.exe` 파일로 동작합니다.
  - 별도의 .NET 런타임, Visual C++ 재배포 패키지(VC++ Redistributable), 관리자 권한 등이 일절 필요 없습니다.
  - **구형 Windows 10 버전(1507, 1607, 1809 등)부터 최신 Windows 11, Windows 7/8까지 완벽 호환**됩니다.
- **초소형 메모리 상주**
  - 시스템 트레이 백그라운드 상주 시 메모리(RAM)를 1~2MB 내외로만 소비하는 극단적인 경량 설계.
- **편리한 시스템 트레이 팝업 메뉴**
  - 트레이 아이콘 우클릭을 통해 모든 기능의 사용 여부를 체크박스로 즉시 전환할 수 있습니다.
- **윈도우 시작 시 자동 실행 지원**
  - 윈도우 시작프로그램 레지스트리(`HKCU`) 연동을 트레이 메뉴 클릭 한 번으로 설정/해제 가능.
- **설정 자동 저장**
  - 설정은 실행 파일 경로의 `OhHangulKey.ini`에 자동 저장되어 재부팅 후에도 유지됩니다.

---

## ⌨️ 기본 키 매핑 및 단축키

| 단축키 | 변환 동작 | 설명 |
| :--- | :--- | :--- |
| **Shift + Space** | **우측 Alt (`VK_HANGUL`)** | 한글 / 영문 입력 모드 전환 |
| **Ctrl + Space** | **우측 Ctrl (`VK_HANJA`)** | 한자 변환 창 호출 |
| **Ctrl + Shift + Space** | **전체 기능 ON/OFF 토글** | 전체 매크로 기능 즉시 일시 정지/재개 |

> 💡 **트레이 메뉴 구성**:
> - `[✓] Shift + Space - 우측 Alt (한글/영문)`
> - `[✓] Ctrl + Space  - 우측 Ctrl (한자)`
> - `[✓] Ctrl + Shift + Space - 전체 기능 ON/OFF 토글`
> - `[✓] 윈도우 시작시에 프로그램 실행`
> - `--------------------------------`
> - `전체 기능: 동작 중 (ON)` / `일시 정지 (OFF)`
> - `OhHangulKey 정보...`
> - `종료`

---

## 🚀 빠른 시작

### 1. 다운로드 및 실행
1. `bin/OhHangulKey.exe` 파일을 원하는 위치에 복사합니다.
2. 실행하면 작업 표시줄 오른쪽 시스템 트레이에 파란색 **'한'** 아이콘이 나타납니다.
3. 원하는 에디터나 웹 브라우저에서 `Shift + Space`를 눌러 한/영 전환을 사용합니다.

### 2. 빌드 방법 (소스 코드 빌드)
Visual Studio (MSVC 컴파일러)가 설치된 환경에서 다음 명령어를 실행합니다:

```bat
# 프로젝트 루트에서 빌드 스크립트 실행
build.bat
```

빌드가 완료되면 `bin/OhHangulKey.exe` 파일이 생성됩니다.

---

## 📂 프로젝트 구조

```
OhHangulKey/
├── .gitignore               # 형상 관리 제외 설정
├── README.md                # 메인 설명서
├── build.bat                # MSVC x64 무의존성 빌드 스크립트
├── src/
│   ├── main.cpp             # 메인 진입점 및 메시지 루프
│   ├── config.h / .cpp      # INI 기반 영구 설정 관리
│   ├── autostart.h / .cpp   # 윈도우 시작 프로그램 레지스트리 제어
│   ├── hook.h / .cpp        # WH_KEYBOARD_LL 저수준 키보드 후킹 엔진
│   ├── tray.h / .cpp        # 트레이 아이콘 및 컨텍스트 메뉴
│   ├── resource.h           # 리소스 ID 정의
│   └── resource.rc          # 아이콘 리소스 및 DPI-Aware 매니페스트
├── assets/
│   ├── app.ico              # 활성 상태 트레이 아이콘
│   └── app_off.ico          # 비활성 상태 트레이 아이콘
└── docs/
    ├── manual.md            # 상세 사용자 및 관리자 가이드
    └── dev/
        └── implement_20261015.md # 상세 구현 로직 및 아키텍처 문서
```

---

## 📜 라이선스

MIT License
