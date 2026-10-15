@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   OhHangulKey - Build Script (MSVC x64 /MT)
echo ========================================================

where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo [INFO] MSVC compiler already available in PATH.
    goto :compile
)

set "VS_PATH="
if exist "%ProgramFiles%\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
)

if "%VS_PATH%"=="" (
    echo [ERROR] Visual Studio MSVC environment not found.
    exit /b 1
)

echo [1/4] Initializing MSVC environment...
call "%VS_PATH%" >nul
if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    exit /b 1
)

:compile

if not exist "build" mkdir build
if not exist "bin" mkdir bin

echo [2/4] Compiling resource file (rc.exe)...
rc /nologo /fo build\resource.res src\resource.rc
if errorlevel 1 (
    echo [ERROR] Resource compilation failed.
    exit /b 1
)

echo [3/4] Compiling and linking C++ sources (cl.exe)...
cl /nologo /utf-8 /O2 /MT /W4 /WX /DUNICODE /D_UNICODE /DNDEBUG /EHsc ^
   src\main.cpp src\config.cpp src\autostart.cpp src\hook.cpp src\tray.cpp ^
   /Fo:build\ /Fe:bin\OhHangulKey.exe ^
   /link /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup /MANIFEST:NO ^
   user32.lib shell32.lib advapi32.lib build\resource.res

if errorlevel 1 (
    echo [ERROR] Build failed!
    exit /b 1
)

echo [4/4] Embedding manifest with mt.exe...
mt.exe -nologo -manifest src\app.manifest -outputresource:bin\OhHangulKey.exe;#1
if errorlevel 1 (
    echo [ERROR] Manifest embedding failed!
    exit /b 1
)

echo ========================================================
echo   [SUCCESS] Build finished: bin\OhHangulKey.exe
echo ========================================================
dir bin\OhHangulKey.exe
