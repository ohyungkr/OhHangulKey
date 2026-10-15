#pragma once
#include <windows.h>

// 윈도우 시작 시 자동 실행 여부 확인
bool IsAutoStartEnabled();

// 윈도우 시작 시 자동 실행 설정 / 해제
bool SetAutoStart(bool enable);
