@echo off
REM ============================================================================
REM Build Script for AIMLAB-VR Motor Controller (SDL2 + ImGui)
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025
REM Version: v5.0
REM
REM Description:
REM   Automated build script for compiling the AIMLAB-VR Motor Controller
REM   using Dear ImGui + SDL2 software renderer (no OpenGL required)
REM
REM Requirements:
REM   - MinGW-w64 (g++ compiler)
REM   - SDL2 development libraries (run download_sdl2.ps1 first)
REM   - Dear ImGui SDL2 backends (run download_imgui_sdl2_backends.ps1 first)
REM
REM Usage:
REM   .\build.bat
REM ============================================================================

echo ============================================================
echo   AIMLAB-VR Motor Controller Build System
echo   Dear ImGui + SDL2 Software Renderer
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

REM Compile command
echo [INFO] Starting compilation...
echo.

g++ main.cpp ^
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
    -mwindows ^
    -std=c++17 ^
    -o main.exe

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
echo [SUCCESS] Build completed successfully!
echo ============================================================
echo.
echo Output: main.exe
echo Size: 
dir main.exe | findstr "main.exe"
echo.
echo Dependencies:
echo   - SDL2.dll (copied to current directory)
echo   - Windows GDI32 (system library)
echo.
echo To run: .\main.exe
echo.
echo Features:
echo   - Dear ImGui modern GUI
echo   - SDL2 software renderer (no OpenGL)
echo   - Arduino serial communication
echo   - Non-blocking UI operations
echo.

REM Offer to run the executable
set /p RUN="Would you like to run the application now? (y/n): "
if /i "%RUN%"=="y" (
    echo.
    echo [INFO] Starting application...
    start main.exe
)

echo.
pause
