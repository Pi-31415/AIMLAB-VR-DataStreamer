@echo off
REM =====================================================
REM AIMLAB VR Streamer - Build Executable Script
REM 
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025
REM Version: v1.0
REM 
REM Description:
REM Builds Windows executables (installer + portable)
REM =====================================================

echo.
echo ========================================
echo   AIMLAB VR Streamer - Build Script
echo   Author: Pi Ko (pi.ko@nyu.edu)
echo ========================================
echo.

REM Check if node_modules exists
if not exist "node_modules\" (
    echo [INFO] Installing dependencies...
    call npm install
    echo.
)

REM Check if electron-builder is installed
if not exist "node_modules\electron-builder\" (
    echo [INFO] Installing electron-builder...
    call npm install --save-dev electron-builder
    echo.
)

echo [INFO] Building Windows executables...
echo [INFO] This will create:
echo        - Installer: dist\AIMLAB VR Streamer Setup 1.0.0.exe
echo        - Portable: dist\AIMLAB-VR-Streamer-Portable.exe
echo.
echo [INFO] Please wait, this may take a few minutes...
echo.

call npm run build

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Built files are in the 'dist' folder:
    echo.
    dir /B dist\*.exe
    echo.
    echo To run the installer:
    echo   .\dist\AIMLAB VR Streamer Setup 1.0.0.exe
    echo.
    echo To run the portable version:
    echo   .\dist\AIMLAB-VR-Streamer-Portable.exe
    echo.
    echo ========================================
) else (
    echo.
    echo ========================================
    echo   BUILD FAILED!
    echo ========================================
    echo.
    echo Please check the error messages above.
    echo.
)

pause

