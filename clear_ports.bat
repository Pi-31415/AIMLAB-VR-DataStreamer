@echo off
REM =====================================================
REM AIMLAB VR Data Collector - Port Cleanup Utility
REM 
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025
REM Version: v1.0
REM 
REM Description:
REM Clears Unity and Electron processes that may be
REM holding ports 45000, 45001, and 45101.
REM Useful when getting EADDRINUSE errors.
REM =====================================================

echo.
echo ========================================
echo   AIMLAB VR - Port Cleanup Utility
echo ========================================
echo.

echo [INFO] Checking for processes using VR ports...
echo.

REM Check port usage
echo Ports in use:
netstat -ano | findstr ":45000"
netstat -ano | findstr ":45001"
netstat -ano | findstr ":45101"
echo.

echo [INFO] Closing Electron processes...
taskkill /F /IM electron.exe 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Electron processes closed
) else (
    echo [INFO] No Electron processes running
)

echo.
echo [INFO] Closing Unity processes...
taskkill /F /IM Unity.exe 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Unity processes closed
) else (
    echo [INFO] No Unity processes running
)

echo.
echo [INFO] Waiting for ports to clear...
timeout /t 3 /nobreak >nul

echo.
echo [INFO] Checking ports again...
netstat -ano | findstr ":45000"
netstat -ano | findstr ":45001"
netstat -ano | findstr ":45101"

echo.
echo ========================================
echo   Port cleanup complete!
echo ========================================
echo.
echo You can now:
echo   1. Start Unity VR application
echo   2. Start Electron app (npm start)
echo.
echo If ports are still in use, restart your computer.
echo.

pause

