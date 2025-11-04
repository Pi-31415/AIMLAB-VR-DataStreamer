@echo off
REM ============================================================================
REM Build Script for AIMLAB VR Data Streamer v6.2
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025
REM Version: v6.2
REM
REM Description:
REM   Build script for v6.2 with PROGRESS INDICATORS and enhanced debugging
REM   Creates AIMLAB_VR_DataStreamer_v6.2.exe
REM
REM NEW IN v6.2:
REM   - Progress bars during Arduino COM port scanning
REM   - Progress bars during Unity VR discovery
REM   - Real-time status messages
REM   - 30-second timeout for manual refresh
REM   - Enhanced timestamped debug logging
REM   - Auto-reset progress indicators
REM
REM Requirements:
REM   - MinGW-w64 (g++ compiler)
REM   - SDL2 development libraries (run download_sdl2.ps1 first)
REM   - Dear ImGui SDL2 backends (run download_imgui_sdl2_backends.ps1 first)
REM   - Winsock2 (included in Windows)
REM
REM Usage:
REM   .\build_v6.2.bat
REM ============================================================================

echo ============================================================
echo   AIMLAB VR Data Streamer v6.2 Build System
echo   WITH PROGRESS INDICATORS
echo ============================================================
echo.

REM Check if SDL2 exists
if not exist "SDL2\" (
    echo [ERROR] SDL2 not found!
    echo.
    echo Please run the following command first:
    echo   powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
    echo.
    pause
    exit /b 1
)

REM Check if SDL2 backends exist
if not exist "backends\imgui_impl_sdl2.cpp" (
    echo [ERROR] SDL2 ImGui backends not found!
    echo.
    echo Please run the following command first:
    echo   powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
    echo.
    pause
    exit /b 1
)

REM Set SDL2 paths
set SDL2_INCLUDE=SDL2\x86_64-w64-mingw32\include\SDL2
set SDL2_LIB=SDL2\x86_64-w64-mingw32\lib
set SDL2_BIN=SDL2\x86_64-w64-mingw32\bin

echo [INFO] SDL2 Include: %SDL2_INCLUDE%
echo [INFO] SDL2 Library: %SDL2_LIB%
echo [INFO] SDL2 Binary: %SDL2_BIN%
echo.

REM Compile command for v6.2
echo [INFO] Starting compilation of v6.2...
echo [INFO] Source: main_v6.2.cpp
echo [INFO] Output: AIMLAB_VR_DataStreamer_v6.2.exe
echo.

g++ main_v6.2.cpp ^
    imgui.cpp ^
    imgui_draw.cpp ^
    imgui_widgets.cpp ^
    imgui_tables.cpp ^
    backends\imgui_impl_sdl2.cpp ^
    backends\imgui_impl_sdlrenderer2.cpp ^
    -I. ^
    -Ibackends ^
    -I%SDL2_INCLUDE% ^
    -L%SDL2_LIB% ^
    -lmingw32 ^
    -lSDL2main ^
    -lSDL2 ^
    -lgdi32 ^
    -lws2_32 ^
    -mwindows ^
    -std=c++17 ^
    -o AIMLAB_VR_DataStreamer_v6.2.exe

REM Check compilation result
if %errorlevel% neq 0 (
    echo.
    echo ============================================================
    echo [ERROR] Compilation failed!
    echo ============================================================
    echo.
    echo Common issues:
    echo   1. MinGW-w64 not in PATH
    echo   2. SDL2 not downloaded properly
    echo   3. ImGui backend files missing
    echo.
    echo Try running setup scripts:
    echo   powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
    echo   powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
    echo.
    pause
    exit /b 1
)

REM Copy SDL2.dll to executable directory
echo.
echo [INFO] Copying SDL2.dll to executable directory...
copy /Y "%SDL2_BIN%\SDL2.dll" . >nul 2>&1

if %errorlevel% neq 0 (
    echo [WARNING] Could not copy SDL2.dll - you may need to do this manually
    echo [WARNING] Copy from: %SDL2_BIN%\SDL2.dll
) else (
    echo [SUCCESS] SDL2.dll copied successfully
)

echo.
echo ============================================================
echo [SUCCESS] v6.2 Build completed successfully!
echo ============================================================
echo.
echo Output: AIMLAB_VR_DataStreamer_v6.2.exe
echo Size: 
dir AIMLAB_VR_DataStreamer_v6.2.exe | findstr "AIMLAB_VR_DataStreamer_v6.2.exe"
echo.
echo Dependencies:
echo   - SDL2.dll (copied to current directory)
echo   - Windows GDI32 (system library)
echo   - Windows Winsock2 (system library)
echo.
echo To run: .\AIMLAB_VR_DataStreamer_v6.2.exe
echo.
echo ============================================================
echo NEW IN v6.2 - PROGRESS INDICATORS:
echo ============================================================
echo   - Arduino COM port scanning progress bar
echo   - Unity VR discovery progress bar with countdown
echo   - Real-time status messages:
echo     * "Checking COM3... [5/12]"
echo     * "Listening for VR headset... (15s remaining)"
echo   - 30-second timeout for manual refresh operations
echo   - Enhanced debug console with timestamps
echo   - Auto-reset of progress bars after 2 seconds
echo   - Thread-safe logging throughout
echo.
echo ============================================================
echo FEATURES:
echo ============================================================
echo   [√] Unity VR headset data streaming
echo   [√] Arduino vibration motor control
echo   [√] CSV data logging with auto-numbering
echo   [√] Auto-discovery on startup (20 seconds)
echo   [√] Manual refresh with progress indicators (30 seconds)
echo   [√] NYU purple-themed professional UI
echo   [√] Debug console with timestamped logs
echo   [√] Real-time connection status
echo   [√] Progress bars and status messages
echo   [√] Non-blocking threaded operations
echo.

REM Offer to run the executable
set /p RUN="Would you like to run v6.2 now? (y/n): "
if /i "%RUN%"=="y" (
    echo.
    echo [INFO] Starting AIMLAB VR Data Streamer v6.2...
    echo [INFO] Watch for progress bars during connection attempts!
    echo.
    start AIMLAB_VR_DataStreamer_v6.2.exe
)

echo.
echo ============================================================
echo COMPARISON: v6.1 vs v6.2
echo ============================================================
echo.
echo v6.1 (stable):          v6.2 (with progress):
echo   - Basic status        - Progress bars
echo   - No feedback         - Status messages
echo   - 20s timeout         - 30s timeout (manual)
echo   - Simple logging      - Timestamped logs
echo.
echo Both versions are fully functional!
echo v6.1: Minimal, stable, proven
echo v6.2: Enhanced UX with visual feedback
echo.
echo To build v6.1: .\build.bat
echo To build v6.2: .\build_v6.2.bat (this script)
echo.

pause

