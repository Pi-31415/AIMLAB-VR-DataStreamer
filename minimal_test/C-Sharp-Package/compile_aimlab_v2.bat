@echo off
REM =====================================
REM AIMLAB-VR-Data Streamer Compiler v2
REM Author: Pi Ko (pi.ko@nyu.edu)
REM =====================================

echo.
echo =====================================
echo  AIMLAB-VR-Data Streamer Compiler v2
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

echo Please select which version to compile:
echo 1. Simplified version (recommended - better compatibility)
echo 2. Fixed version (with proper Windows headers)
echo 3. Both versions
echo.
set /p choice="Enter your choice (1-3): "

if "%choice%"=="1" goto compile_simple
if "%choice%"=="2" goto compile_fixed
if "%choice%"=="3" goto compile_both
goto invalid_choice

:compile_simple
echo.
echo Compiling simplified version...
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer_simple.cpp -lws2_32 -std=c++11
if %errorlevel% equ 0 (
    echo [SUCCESS] aimlab_streamer.exe compiled successfully
) else (
    echo [ERROR] Failed to compile simplified version
    pause
    exit /b 1
)
goto success

:compile_fixed
echo.
echo Compiling fixed version...
g++ -o aimlab_streamer_fixed.exe aimlab_vr_datastreamer_fixed.cpp -lws2_32 -std=c++11
if %errorlevel% equ 0 (
    echo [SUCCESS] aimlab_streamer_fixed.exe compiled successfully
) else (
    echo [ERROR] Failed to compile fixed version
    pause
    exit /b 1
)
goto success

:compile_both
echo.
echo Compiling both versions...

echo [1/2] Compiling simplified version...
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer_simple.cpp -lws2_32 -std=c++11
if %errorlevel% equ 0 (
    echo [SUCCESS] aimlab_streamer.exe compiled successfully
) else (
    echo [ERROR] Failed to compile simplified version
)

echo.
echo [2/2] Compiling fixed version...
g++ -o aimlab_streamer_fixed.exe aimlab_vr_datastreamer_fixed.cpp -lws2_32 -std=c++11
if %errorlevel% equ 0 (
    echo [SUCCESS] aimlab_streamer_fixed.exe compiled successfully
) else (
    echo [ERROR] Failed to compile fixed version
)
goto success

:invalid_choice
echo [ERROR] Invalid choice. Please run again and select 1, 2, or 3.
pause
exit /b 1

:success
echo.
echo =====================================
echo   Compilation Complete!
echo =====================================
echo.
echo To use AIMLAB-VR Data Streamer:
echo.
echo 1. C++ Side:
echo    - Run: aimlab_streamer.exe (or aimlab_streamer_fixed.exe)
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
echo Troubleshooting:
echo - If compilation fails, try the simplified version
echo - Make sure Windows Firewall allows UDP ports 45000-45101
echo - Run both applications on the same network
echo.

pause
