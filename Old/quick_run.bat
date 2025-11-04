@echo off
REM Simple one-click build and run for AIMLAB VR Data Streamer

echo.
echo ==================================
echo  AIMLAB VR - Quick Compile and Run
echo ==================================
echo.

REM Compile
echo Compiling...
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Compilation failed!
    echo Make sure MinGW-w64 is installed.
    echo.
    pause
    exit /b 1
)

echo SUCCESS: Compiled aimlab_network.exe
echo.
echo Starting application...
echo.
echo Commands: 
echo   send [message] - Send to all peers
echo   peers         - List connected peers  
echo   quit          - Exit
echo.
echo ======================================
echo.

aimlab_network.exe

pause