# ================================================
# GLFW3 Download and Setup Script
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 02 November 2025
# ================================================

Write-Host "`n================================================" -ForegroundColor Cyan
Write-Host "  GLFW3 Download and Setup Script" -ForegroundColor Cyan
Write-Host "  Author: Pi Ko (pi.ko@nyu.edu)" -ForegroundColor Cyan
Write-Host "================================================`n" -ForegroundColor Cyan

# Configuration
$glfwVersion = "3.4"
$glfwUrl = "https://github.com/glfw/glfw/releases/download/$glfwVersion/glfw-$glfwVersion.bin.WIN64.zip"
$downloadPath = "glfw.zip"
$extractPath = "glfw"

# Step 1: Download GLFW
Write-Host "[1/4] Downloading GLFW $glfwVersion..." -ForegroundColor Yellow

try {
    Invoke-WebRequest -Uri $glfwUrl -OutFile $downloadPath -UseBasicParsing
    Write-Host "[SUCCESS] Downloaded GLFW" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] Failed to download GLFW: $_" -ForegroundColor Red
    Write-Host "`nPlease download manually from: https://www.glfw.org/download.html" -ForegroundColor Yellow
    pause
    exit 1
}

# Step 2: Extract
Write-Host "`n[2/4] Extracting GLFW..." -ForegroundColor Yellow

try {
    Expand-Archive -Path $downloadPath -DestinationPath . -Force
    
    # Find the extracted folder
    $glfwFolder = Get-ChildItem -Directory | Where-Object { $_.Name -like "glfw-*" } | Select-Object -First 1
    
    if ($glfwFolder) {
        Write-Host "[SUCCESS] Extracted to: $($glfwFolder.Name)" -ForegroundColor Green
        $extractPath = $glfwFolder.FullName
    } else {
        throw "Could not find extracted GLFW folder"
    }
} catch {
    Write-Host "[ERROR] Failed to extract GLFW: $_" -ForegroundColor Red
    pause
    exit 1
}

# Step 3: Copy DLL to project directory
Write-Host "`n[3/4] Copying GLFW DLL to project..." -ForegroundColor Yellow

try {
    $dllSource = Join-Path $extractPath "lib-mingw-w64\glfw3.dll"
    if (Test-Path $dllSource) {
        Copy-Item $dllSource -Destination "." -Force
        Write-Host "[SUCCESS] Copied glfw3.dll" -ForegroundColor Green
    } else {
        Write-Host "[WARNING] Could not find glfw3.dll at: $dllSource" -ForegroundColor Yellow
    }
} catch {
    Write-Host "[WARNING] Failed to copy DLL: $_" -ForegroundColor Yellow
}

# Step 4: Display installation instructions
Write-Host "`n[4/4] Installation Instructions" -ForegroundColor Yellow
Write-Host "================================================`n" -ForegroundColor Cyan

Write-Host "GLFW has been downloaded and extracted to:" -ForegroundColor White
Write-Host "  $extractPath`n" -ForegroundColor Cyan

Write-Host "To complete the setup, you need to:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. Find your MinGW installation directory" -ForegroundColor White
Write-Host "   Common locations:" -ForegroundColor Gray
Write-Host "   - C:\mingw64\" -ForegroundColor Gray
Write-Host "   - C:\msys64\mingw64\" -ForegroundColor Gray
Write-Host "   - C:\Program Files\mingw-w64\" -ForegroundColor Gray
Write-Host ""

Write-Host "2. Copy GLFW headers:" -ForegroundColor White
$headerSource = Join-Path $extractPath "include\GLFW"
Write-Host "   From: $headerSource" -ForegroundColor Cyan
Write-Host "   To:   C:\mingw64\include\GLFW" -ForegroundColor Cyan
Write-Host ""
Write-Host "   PowerShell command:" -ForegroundColor Gray
Write-Host "   Copy-Item `"$headerSource`" -Destination `"C:\mingw64\include\GLFW`" -Recurse -Force" -ForegroundColor DarkGray
Write-Host ""

Write-Host "3. Copy GLFW library:" -ForegroundColor White
$libSource = Join-Path $extractPath "lib-mingw-w64\libglfw3.a"
Write-Host "   From: $libSource" -ForegroundColor Cyan
Write-Host "   To:   C:\mingw64\lib\libglfw3.a" -ForegroundColor Cyan
Write-Host ""
Write-Host "   PowerShell command:" -ForegroundColor Gray
Write-Host "   Copy-Item `"$libSource`" -Destination `"C:\mingw64\lib\libglfw3.a`" -Force" -ForegroundColor DarkGray
Write-Host ""

Write-Host "4. GLFW DLL (already copied to project directory)" -ForegroundColor White
Write-Host "   Location: $(Get-Location)\glfw3.dll" -ForegroundColor Green
Write-Host ""

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "Quick Setup Script:" -ForegroundColor Yellow
Write-Host "================================================`n" -ForegroundColor Cyan

$quickScript = @"
# Run these commands (adjust MinGW path if different):
Copy-Item "$headerSource" -Destination "C:\mingw64\include\GLFW" -Recurse -Force
Copy-Item "$libSource" -Destination "C:\mingw64\lib\libglfw3.a" -Force
Write-Host "GLFW installed successfully!" -ForegroundColor Green
"@

Write-Host $quickScript -ForegroundColor DarkGray
Write-Host ""

# Option to run quick setup
Write-Host "Would you like to attempt automatic installation to C:\mingw64? (y/n)" -ForegroundColor Yellow
$response = Read-Host

if ($response -eq 'y' -or $response -eq 'Y') {
    Write-Host "`nAttempting automatic installation..." -ForegroundColor Yellow
    
    try {
        # Check if MinGW exists
        if (Test-Path "C:\mingw64") {
            # Copy headers
            $destHeader = "C:\mingw64\include\GLFW"
            Copy-Item $headerSource -Destination $destHeader -Recurse -Force
            Write-Host "[SUCCESS] Copied headers to $destHeader" -ForegroundColor Green
            
            # Copy library
            $destLib = "C:\mingw64\lib\libglfw3.a"
            Copy-Item $libSource -Destination $destLib -Force
            Write-Host "[SUCCESS] Copied library to $destLib" -ForegroundColor Green
            
            Write-Host "`n[SUCCESS] GLFW installed successfully!" -ForegroundColor Green
            Write-Host "You can now run: .\build.bat" -ForegroundColor Cyan
        } else {
            Write-Host "[ERROR] C:\mingw64 not found" -ForegroundColor Red
            Write-Host "Please manually copy files to your MinGW directory" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "[ERROR] Installation failed: $_" -ForegroundColor Red
        Write-Host "Please manually copy files following the instructions above" -ForegroundColor Yellow
    }
} else {
    Write-Host "`nPlease manually install GLFW following the instructions above" -ForegroundColor Yellow
}

# Cleanup
Write-Host "`nCleanup downloaded zip? (y/n)" -ForegroundColor Yellow
$cleanup = Read-Host

if ($cleanup -eq 'y' -or $cleanup -eq 'Y') {
    Remove-Item $downloadPath -Force
    Write-Host "Cleaned up downloaded zip" -ForegroundColor Green
}

Write-Host "`n================================================" -ForegroundColor Cyan
Write-Host "Setup complete!" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Verify GLFW is installed: g++ -lglfw3" -ForegroundColor White
Write-Host "2. Build the project: .\build.bat" -ForegroundColor White
Write-Host "3. See SETUP_GUIDE.md for troubleshooting" -ForegroundColor White
Write-Host ""

pause

