@echo off
REM =====================================
REM UDP Chat Application Compiler
REM Author: Pi Ko (pi.ko@nyu.edu)
REM =====================================

echo.
echo =====================================
echo   UDP Chat Application Compiler
echo   Author: Pi Ko (pi.ko@nyu.edu)
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

REM Compile Node 1
echo [1/2] Compiling udp_chat_node1.cpp...
g++ -o node1.exe udp_chat_node1.cpp -lws2_32 -std=c++11

if %errorlevel% equ 0 (
    echo [SUCCESS] node1.exe compiled successfully
) else (
    echo [ERROR] Failed to compile node1.exe
    pause
    exit /b 1
)

echo.

REM Compile Node 2
echo [2/2] Compiling udp_chat_node2.cpp...
g++ -o node2.exe udp_chat_node2.cpp -lws2_32 -std=c++11

if %errorlevel% equ 0 (
    echo [SUCCESS] node2.exe compiled successfully
) else (
    echo [ERROR] Failed to compile node2.exe
    pause
    exit /b 1
)

echo.
echo =====================================
echo   Compilation Complete!
echo =====================================
echo.
echo To run the chat application:
echo   1. Open two command prompt windows
echo   2. Run node1.exe in the first window
echo   3. Run node2.exe in the second window
echo   4. Wait for auto-discovery and handshake
echo   5. Start chatting!
echo.
echo Type 'quit' in either window to exit
echo.

pause
