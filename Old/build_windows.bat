@echo off
REM AIMLAB VR Data Streamer - Windows Build Script
REM This script compiles and optionally runs the C++ implementation

echo =========================================
echo  AIMLAB VR Data Streamer - Windows Build
echo =========================================
echo.

REM Check for g++ compiler
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW-w64 or use Visual Studio.
    echo.
    echo Download MinGW-w64 from: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

echo Found g++ compiler
echo.

REM Compile the C++ application
echo Compiling C++ application...
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32 2>build_errors.txt

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Compilation failed!
    echo Check build_errors.txt for details.
    echo.
    type build_errors.txt
    pause
    exit /b 1
) else (
    echo SUCCESS: Compilation completed!
    if exist build_errors.txt del build_errors.txt
)

echo.
echo Build successful: aimlab_network.exe
echo.

REM Ask user if they want to run the application
set /p run_app="Do you want to run the application now? (y/n): "

if /i "%run_app%"=="y" (
    echo.
    echo Starting AIMLAB VR Data Streamer...
    echo Press Ctrl+C to stop
    echo.
    aimlab_network.exe
) else (
    echo.
    echo You can run the application later with: aimlab_network.exe
)

pause
