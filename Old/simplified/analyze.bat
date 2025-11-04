@echo off
REM VR Data Analysis Launcher
REM Author: Pi Ko (pi.ko@nyu.edu)
REM Date: 04 November 2025

echo ========================================
echo  AIMLAB VR Data Analysis
echo ========================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: Python is not installed or not in PATH
    echo Please install Python 3.7 or later
    echo Download from: https://www.python.org/downloads/
    pause
    exit /b 1
)

REM Check if file argument provided
if "%~1"=="" (
    echo Usage: analyze.bat ^<filename.csv^>
    echo Example: analyze.bat session_01.csv
    echo.
    echo Available CSV files:
    dir /b *.csv 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo No CSV files found in current directory
    )
    echo.
    pause
    exit /b 1
)

REM Check if file exists
if not exist "%~1" (
    echo Error: File not found: %~1
    echo.
    pause
    exit /b 1
)

REM Check if requirements are installed
python -c "import pandas, matplotlib, numpy" >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Installing required packages...
    python -m pip install -r requirements.txt
    echo.
)

REM Run analysis
echo Analyzing %~1...
echo.
python analyze_vr_data.py "%~1"

echo.
pause

