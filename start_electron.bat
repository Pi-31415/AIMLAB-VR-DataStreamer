@echo off
REM =====================================================
REM AIMLAB VR Streamer - Quick Start Script
REM 
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025
REM Version: v1.0
REM 
REM Description:
REM Quick start batch script to launch the Electron app
REM =====================================================

echo.
echo ========================================
echo   AIMLAB VR Streamer - Electron App
echo   Author: Pi Ko (pi.ko@nyu.edu)
echo ========================================
echo.

REM Check if node_modules exists
if not exist "node_modules\" (
    echo [INFO] Installing dependencies...
    call npm install
    echo.
)

echo [INFO] Starting Electron application...
echo.
call npm start

