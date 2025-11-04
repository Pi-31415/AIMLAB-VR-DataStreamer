################################################################################
# Dear ImGui SDL2 Backend Downloader Script
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 November 2025
# Version: v1.0
#
# Description:
#   Downloads official Dear ImGui SDL2 backend files from GitHub repository
#
# Usage:
#   powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
################################################################################

$ErrorActionPreference = "Stop"

# Configuration
# Note: Using 'master' branch for latest stable backends
# For specific version, use format like 'v1.89.9'
$IMGUI_BRANCH = "master"
$BASE_URL = "https://raw.githubusercontent.com/ocornut/imgui/$IMGUI_BRANCH/backends"

$FILES = @(
    "imgui_impl_sdl2.h",
    "imgui_impl_sdl2.cpp",
    "imgui_impl_sdlrenderer2.h",
    "imgui_impl_sdlrenderer2.cpp"
)

$BACKENDS_DIR = "backends"

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Dear ImGui SDL2 Backend Downloader" -ForegroundColor Cyan
Write-Host "  AIMLAB-VR Motor Controller" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# Create backends directory if it doesn't exist
if (-not (Test-Path $BACKENDS_DIR)) {
    New-Item -ItemType Directory -Path $BACKENDS_DIR | Out-Null
    Write-Host "[INFO] Created backends directory" -ForegroundColor Green
}

# Download each file
$successCount = 0
foreach ($file in $FILES) {
    $url = "$BASE_URL/$file"
    $output = "$BACKENDS_DIR\$file"
    
    Write-Host "[INFO] Downloading $file..." -ForegroundColor Yellow
    
    try {
        Invoke-WebRequest -Uri $url -OutFile $output -UseBasicParsing
        Write-Host "[SUCCESS] Downloaded $file" -ForegroundColor Green
        $successCount++
    } catch {
        Write-Host "[ERROR] Failed to download $file : $_" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Download Complete: $successCount/$($FILES.Count) files" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

if ($successCount -eq $FILES.Count) {
    Write-Host "[SUCCESS] All backend files downloaded successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "Backend files location: $BACKENDS_DIR/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "You can now proceed to download SDL2 and build the application." -ForegroundColor Yellow
} else {
    Write-Host "[WARNING] Some files failed to download. Check errors above." -ForegroundColor Red
}

