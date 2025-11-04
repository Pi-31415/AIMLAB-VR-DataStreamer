@echo off
REM =====================================
REM UDP Chat Application Compiler v2
REM Author: Pi Ko (pi.ko@nyu.edu)
REM =====================================

echo.
echo =====================================
echo   UDP Chat Application Compiler v2
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

echo Compiling improved v2 versions (recommended)...
echo.

REM Compile Node 1 v2
echo [1/2] Compiling udp_chat_node1_v2.cpp...
g++ -o node1_v2.exe udp_chat_node1_v2.cpp -lws2_32 -std=c++11

if %errorlevel% equ 0 (
    echo [SUCCESS] node1_v2.exe compiled successfully
) else (
    echo [ERROR] Failed to compile node1_v2.exe
    pause
    exit /b 1
)

echo.

REM Compile Node 2 v2
echo [2/2] Compiling udp_chat_node2_v2.cpp...
g++ -o node2_v2.exe udp_chat_node2_v2.cpp -lws2_32 -std=c++11

if %errorlevel% equ 0 (
    echo [SUCCESS] node2_v2.exe compiled successfully
) else (
    echo [ERROR] Failed to compile node2_v2.exe
    pause
    exit /b 1
)

echo.
echo =====================================
echo   Compilation Complete!
echo =====================================
echo.
echo The v2 versions have improved port handling that:
echo   - Automatically finds available ports
echo   - Broadcasts to multiple ports for discovery
echo   - Works even if default ports are in use
echo   - Provides better error messages
echo.
echo To run the chat application:
echo   1. Open two command prompt windows
echo   2. Run node1_v2.exe in the first window
echo   3. Run node2_v2.exe in the second window
echo   4. Wait for auto-discovery and handshake
echo   5. Start chatting!
echo.
echo Type 'quit' in either window to exit
echo.
echo If you still have issues, check:
echo   - Windows Firewall (allow UDP ports 50001-50010)
echo   - Antivirus software blocking UDP broadcasts
echo   - Run as Administrator if needed
echo.

pause
