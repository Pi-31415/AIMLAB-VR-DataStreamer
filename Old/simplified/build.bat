@echo off
REM Build script for AIMLAB VR Data Streamer Console
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025

echo ========================================
echo  Building AIMLAB VR Data Streamer...
echo ========================================
echo.

g++ main.cpp -o AIMLAB_Console.exe -lws2_32 -std=c++17

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo  Build FAILED!
    echo ========================================
    pause
) else (
    echo.
    echo ========================================
    echo  Build SUCCESSFUL!
    echo ========================================
    echo.
    echo Run: AIMLAB_Console.exe
    echo.
    pause
)

