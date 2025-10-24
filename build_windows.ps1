# AIMLAB VR Data Streamer - Windows Build Script (PowerShell)
# This script compiles and optionally runs both C++ and Python implementations

Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "  AIMLAB VR Data Streamer - Windows Build" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

# Function to check if a command exists
function Test-CommandExists {
    param($command)
    $null = Get-Command $command -ErrorAction SilentlyContinue
    return $?
}

# Check prerequisites
Write-Host "Checking prerequisites..." -ForegroundColor Yellow

$hasGpp = Test-CommandExists "g++"
$hasPython = Test-CommandExists "python"

if (-not $hasGpp) {
    Write-Host "ERROR: g++ compiler not found!" -ForegroundColor Red
    Write-Host "Please install MinGW-w64 from: https://www.mingw-w64.org/downloads/" -ForegroundColor Yellow
    Write-Host "Or use Visual Studio with C++ support" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}
Write-Host "✓ Found g++ compiler" -ForegroundColor Green

if (-not $hasPython) {
    Write-Host "WARNING: Python not found!" -ForegroundColor Yellow
    Write-Host "Python is required for the Python implementation" -ForegroundColor Yellow
    Write-Host "Download from: https://www.python.org/downloads/windows/" -ForegroundColor Yellow
} else {
    Write-Host "✓ Found Python" -ForegroundColor Green
}

Write-Host ""

# Compile C++ application
Write-Host "Compiling C++ application..." -ForegroundColor Cyan
$buildOutput = g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32 2>&1

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Compilation failed!" -ForegroundColor Red
    Write-Host $buildOutput -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "✓ C++ application compiled successfully: aimlab_network.exe" -ForegroundColor Green
Write-Host ""

# Menu for user
Write-Host "What would you like to do?" -ForegroundColor Cyan
Write-Host "1. Run C++ implementation only"
Write-Host "2. Run Python implementation only"
Write-Host "3. Run both (in separate windows)"
Write-Host "4. Run automated test suite"
Write-Host "5. Exit"
Write-Host ""

$choice = Read-Host "Enter your choice (1-5)"

switch ($choice) {
    "1" {
        Write-Host "Starting C++ implementation..." -ForegroundColor Green
        Write-Host "Commands: 'send <message>', 'peers', 'quit'" -ForegroundColor Yellow
        Write-Host ""
        & .\aimlab_network.exe
    }
    "2" {
        if ($hasPython) {
            Write-Host "Starting Python implementation..." -ForegroundColor Green
            Write-Host "Commands: 'send <message>', 'peers', 'simulate', 'quit'" -ForegroundColor Yellow
            Write-Host ""
            python aimlab_network_python.py
        } else {
            Write-Host "Python is not installed!" -ForegroundColor Red
        }
    }
    "3" {
        Write-Host "Starting both implementations..." -ForegroundColor Green
        
        # Start C++ in new window
        Start-Process powershell -ArgumentList "-NoExit", "-Command", "& '.\aimlab_network.exe'"
        
        # Wait a moment
        Start-Sleep -Seconds 2
        
        # Start Python in new window if available
        if ($hasPython) {
            Start-Process powershell -ArgumentList "-NoExit", "-Command", "python aimlab_network_python.py"
            Write-Host "✓ Both implementations started in separate windows" -ForegroundColor Green
            Write-Host "They should auto-discover and connect!" -ForegroundColor Yellow
        } else {
            Write-Host "Started C++ only (Python not available)" -ForegroundColor Yellow
        }
    }
    "4" {
        if ($hasPython) {
            Write-Host "Starting one instance for testing..." -ForegroundColor Green
            Start-Process powershell -ArgumentList "-Command", "& '.\aimlab_network.exe'"
            
            Start-Sleep -Seconds 3
            
            Write-Host "Running test suite..." -ForegroundColor Cyan
            python test_communication.py
        } else {
            Write-Host "Python is required for the test suite!" -ForegroundColor Red
        }
    }
    "5" {
        Write-Host "Exiting..." -ForegroundColor Yellow
        exit 0
    }
    default {
        Write-Host "Invalid choice!" -ForegroundColor Red
    }
}

Write-Host ""
Read-Host "Press Enter to exit"
