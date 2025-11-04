@echo off
REM Setup Verification Script
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025

echo ========================================
echo  AIMLAB VR Setup Verification
echo ========================================
echo.

REM Check for g++ compiler
echo [1/5] Checking for g++ compiler...
g++ --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   [FAIL] g++ not found
    echo   Install MinGW-w64 from: https://sourceforge.net/projects/mingw-w64/
    set ERRORS=1
) else (
    for /f "tokens=*" %%i in ('g++ --version ^| findstr /C:"g++"') do (
        echo   [OK] %%i
    )
)
echo.

REM Check for Python
echo [2/5] Checking for Python...
python --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   [WARN] Python not found
    echo   Install from: https://www.python.org/downloads/
    echo   (Only needed for data analysis)
) else (
    for /f "tokens=*" %%i in ('python --version') do (
        echo   [OK] %%i
    )
    
    REM Check Python packages
    echo   Checking Python packages...
    python -c "import pandas" >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo   [WARN] pandas not installed (run: pip install -r requirements.txt)
    ) else (
        echo   [OK] pandas installed
    )
)
echo.

REM Check for executable
echo [3/5] Checking for AIMLAB_Console.exe...
if exist "AIMLAB_Console.exe" (
    echo   [OK] Executable found
    for %%A in (AIMLAB_Console.exe) do echo   Size: %%~zA bytes
) else (
    echo   [INFO] Executable not built yet
    echo   Run: build.bat
)
echo.

REM Check for source files
echo [4/5] Checking source files...
set ALL_FILES_OK=1

if exist "main.cpp" (
    echo   [OK] main.cpp
) else (
    echo   [FAIL] main.cpp missing
    set ALL_FILES_OK=0
)

if exist "Arduino_Vibration_Controller.ino" (
    echo   [OK] Arduino_Vibration_Controller.ino
) else (
    echo   [FAIL] Arduino_Vibration_Controller.ino missing
    set ALL_FILES_OK=0
)

if exist "Unity_VRDataStreamer.cs" (
    echo   [OK] Unity_VRDataStreamer.cs
) else (
    echo   [FAIL] Unity_VRDataStreamer.cs missing
    set ALL_FILES_OK=0
)

if exist "analyze_vr_data.py" (
    echo   [OK] analyze_vr_data.py
) else (
    echo   [FAIL] analyze_vr_data.py missing
    set ALL_FILES_OK=0
)
echo.

REM Check for documentation
echo [5/5] Checking documentation...
if exist "README.md" (echo   [OK] README.md) else (echo   [FAIL] README.md missing)
if exist "QUICK_START.md" (echo   [OK] QUICK_START.md) else (echo   [FAIL] QUICK_START.md missing)
if exist "SETUP_GUIDE.md" (echo   [OK] SETUP_GUIDE.md) else (echo   [FAIL] SETUP_GUIDE.md missing)
if exist "PROJECT_OVERVIEW.md" (echo   [OK] PROJECT_OVERVIEW.md) else (echo   [FAIL] PROJECT_OVERVIEW.md missing)
echo.

REM Check network ports (optional)
echo [BONUS] Checking network ports...
netstat -an | findstr ":55000" >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo   [INFO] Port 55000 is in use
) else (
    echo   [OK] Port 55000 is available
)

netstat -an | findstr ":55001" >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo   [INFO] Port 55001 is in use
) else (
    echo   [OK] Port 55001 is available
)
echo.

REM Summary
echo ========================================
echo  Verification Summary
echo ========================================
if "%ERRORS%"=="1" (
    echo   Status: ERRORS FOUND
    echo   Please install missing dependencies
) else if "%ALL_FILES_OK%"=="0" (
    echo   Status: FILES MISSING
) else (
    echo   Status: ALL CHECKS PASSED
    echo.
    echo   Ready to build and run!
    echo   Next steps:
    echo   1. Run: build.bat
    echo   2. Run: AIMLAB_Console.exe
)
echo ========================================
echo.
pause

