@echo off
REM =====================================
REM AIMLAB-VR-Data Streamer Compiler
REM Author: Pi Ko (pi.ko@nyu.edu)
REM =====================================

echo.
echo =====================================
echo  AIMLAB-VR-Data Streamer Compiler
echo  Author: Pi Ko (pi.ko@nyu.edu)
echo =====================================
echo.

REM Check if g++ is available
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] g++ compiler not found!
    echo Please install MinGW-w64 or similar and add to PATH
    echo.
    echo Download from: https://www.mingw-w64.org/
    echo.
    pause
    exit /b 1
)

echo [INFO] Found g++ compiler
echo.

REM Create data directory
if not exist "aimlab_data" (
    mkdir aimlab_data
    echo [INFO] Created data directory: aimlab_data
)

echo Compiling AIMLAB-VR Data Streamer...
echo.

REM Compile the C++ application
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11

if %errorlevel% equ 0 (
    echo [SUCCESS] aimlab_streamer.exe compiled successfully
) else (
    echo [ERROR] Failed to compile aimlab_streamer.exe
    pause
    exit /b 1
)

echo.
echo =====================================
echo   Compilation Complete!
echo =====================================
echo.
echo To use AIMLAB-VR Data Streamer:
echo.
echo 1. C++ Side:
echo    - Run: aimlab_streamer.exe
echo    - Commands: help, status, open, close, quit
echo.
echo 2. Unity Side:
echo    - Copy AIMLABVRDataStreamer.cs to Assets folder
echo    - Add to GameObject in scene
echo    - Configure UI elements (optional)
echo    - Run Unity project
echo.
echo The applications will auto-connect when both are running.
echo Data will be saved to: aimlab_data\ folder
echo.

pause
