################################################################################
# SDL2 Downloader Script for Windows
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 November 2025
# Version: v1.0
#
# Description:
#   Automatically downloads SDL2 development libraries for MinGW-w64
#   and extracts them to the MotorController directory
#
# Usage:
#   powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
################################################################################

$ErrorActionPreference = "Stop"

# Configuration
$SDL2_VERSION = "2.28.5"
$SDL2_URL = "https://github.com/libsdl-org/SDL/releases/download/release-$SDL2_VERSION/SDL2-devel-$SDL2_VERSION-mingw.zip"
$SDL2_ZIP = "SDL2-mingw.zip"
$SDL2_DIR = "SDL2"

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  SDL2 Development Library Downloader" -ForegroundColor Cyan
Write-Host "  AIMLAB-VR Motor Controller (ImGui + SDL2 Renderer)" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# Check if SDL2 already exists
if (Test-Path $SDL2_DIR) {
    Write-Host "[INFO] SDL2 directory already exists." -ForegroundColor Yellow
    $response = Read-Host "Do you want to re-download? (y/n)"
    if ($response -ne "y") {
        Write-Host "[INFO] Skipping download." -ForegroundColor Green
        exit 0
    }
    Remove-Item -Recurse -Force $SDL2_DIR
}

# Download SDL2
Write-Host "[INFO] Downloading SDL2 $SDL2_VERSION from GitHub..." -ForegroundColor Green
try {
    Invoke-WebRequest -Uri $SDL2_URL -OutFile $SDL2_ZIP -UseBasicParsing
    Write-Host "[SUCCESS] Downloaded SDL2 successfully!" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] Failed to download SDL2: $_" -ForegroundColor Red
    exit 1
}

# Extract SDL2
Write-Host "[INFO] Extracting SDL2..." -ForegroundColor Green
try {
    Expand-Archive -Path $SDL2_ZIP -DestinationPath "." -Force
    
    # Find the extracted directory (typically SDL2-2.28.5)
    $extractedDir = Get-ChildItem -Directory | Where-Object { $_.Name -like "SDL2-*" } | Select-Object -First 1
    
    if ($extractedDir) {
        # Rename to SDL2 for consistency
        Rename-Item $extractedDir.Name $SDL2_DIR
        Write-Host "[SUCCESS] Extracted SDL2 to $SDL2_DIR/" -ForegroundColor Green
    } else {
        Write-Host "[ERROR] Could not find extracted SDL2 directory" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "[ERROR] Failed to extract SDL2: $_" -ForegroundColor Red
    exit 1
}

# Cleanup
Write-Host "[INFO] Cleaning up..." -ForegroundColor Green
Remove-Item $SDL2_ZIP -Force

# Verify installation
$requiredDirs = @(
    "$SDL2_DIR\x86_64-w64-mingw32\include\SDL2",
    "$SDL2_DIR\x86_64-w64-mingw32\lib",
    "$SDL2_DIR\x86_64-w64-mingw32\bin"
)

$allExist = $true
foreach ($dir in $requiredDirs) {
    if (-not (Test-Path $dir)) {
        Write-Host "[ERROR] Missing directory: $dir" -ForegroundColor Red
        $allExist = $false
    }
}

if ($allExist) {
    Write-Host ""
    Write-Host "============================================================" -ForegroundColor Green
    Write-Host "  SDL2 Installation Complete!" -ForegroundColor Green
    Write-Host "============================================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "SDL2 Location: $SDL2_DIR/" -ForegroundColor Cyan
    Write-Host "Include Path: $SDL2_DIR/x86_64-w64-mingw32/include/SDL2/" -ForegroundColor Cyan
    Write-Host "Library Path: $SDL2_DIR/x86_64-w64-mingw32/lib/" -ForegroundColor Cyan
    Write-Host "Binary Path: $SDL2_DIR/x86_64-w64-mingw32/bin/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "You can now run build.bat to compile the application." -ForegroundColor Yellow
} else {
    Write-Host "[ERROR] SDL2 installation incomplete!" -ForegroundColor Red
    exit 1
}

