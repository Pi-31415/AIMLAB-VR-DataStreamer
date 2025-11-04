@echo off
REM ================================================
REM AIMLAB-VR Motor Controller - Build Script
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 02 November 2025
REM Version: v2.0 (Win32 Native)
REM ================================================

setlocal

echo.
echo ================================================
echo   AIMLAB-VR Motor Controller - Build Script
echo   Author: Pi Ko (pi.ko@nyu.edu)
echo   Version: v2.0 (Windows Native UI)
echo ================================================
echo.

REM Check if g++ is available
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] g++ compiler not found!
    echo Please install MinGW-w64 and add to PATH
    echo Download from: https://www.mingw-w64.org/
    echo.
    pause
    exit /b 1
)

echo [INFO] Found g++ compiler
echo.

REM Clean previous build
if exist main.exe (
    echo [INFO] Removing previous build...
    del main.exe
)

REM Compile command - Win32 Native (No external libraries needed!)
echo [INFO] Compiling Motor Controller (Win32 Native UI)...
echo.

g++ main.cpp ^
    -lgdi32 ^
    -luser32 ^
    -o main.exe ^
    -mwindows ^
    -std=c++17

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed!
    echo.
    echo Compilation error occurred. Check the output above.
    echo.
    pause
    exit /b %errorlevel%
)

echo.
echo ================================================
echo   Build Successful!
echo ================================================
echo.
echo [SUCCESS] Compiled with zero external dependencies!
echo [INFO] Executable: main.exe
echo [INFO] Size: 
dir main.exe | find "main.exe"
echo.
echo [INFO] Running program...
echo.

REM Run the program
start main.exe

echo [INFO] Application launched
echo.
pause

endlocal

