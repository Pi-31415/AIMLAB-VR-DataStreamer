@echo off
REM Run both C++ and Python implementations for testing

echo.
echo ==========================================
echo  AIMLAB VR - Run Both Implementations
echo ==========================================
echo.

REM First compile the C++ version
echo Step 1: Compiling C++ version...
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32

if %errorlevel% neq 0 (
    echo.
    echo ERROR: C++ compilation failed!
    echo Please install MinGW-w64 first.
    pause
    exit /b 1
)

echo SUCCESS: C++ compiled
echo.

REM Start C++ in new window
echo Step 2: Starting C++ node...
start "AIMLAB C++ Node" cmd /k aimlab_network.exe

REM Wait a moment
timeout /t 2 /nobreak >nul

REM Start Python in new window
echo Step 3: Starting Python node...
start "AIMLAB Python Node" cmd /k python aimlab_network_python.py

echo.
echo ==========================================
echo Both nodes started in separate windows!
echo They should auto-discover and connect.
echo.
echo In each window, you can type:
echo   send [message] - to send messages
echo   peers         - to list connections
echo   quit          - to exit
echo ==========================================
echo.

pause