# AIMLAB VR Data Streamer - Simple Windows Build Script (PowerShell)

Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "  AIMLAB VR Data Streamer - Build & Run " -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

# Compile the C++ application
Write-Host "Step 1: Compiling C++ application..." -ForegroundColor Yellow
Write-Host "Running: g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32" -ForegroundColor Gray
Write-Host ""

$result = g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32 2>&1 | Out-String

if ($LASTEXITCODE -eq 0) {
    Write-Host "[SUCCESS] C++ application compiled: aimlab_network.exe" -ForegroundColor Green
} else {
    Write-Host "[ERROR] Compilation failed!" -ForegroundColor Red
    Write-Host $result
    Write-Host ""
    Write-Host "Make sure you have MinGW-w64 installed and in your PATH" -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host ""
Write-Host "Step 2: Choose what to run:" -ForegroundColor Yellow
Write-Host "  1 - Run C++ version" -ForegroundColor White
Write-Host "  2 - Run Python version" -ForegroundColor White
Write-Host "  3 - Run both (opens new windows)" -ForegroundColor White
Write-Host "  4 - Just compile (exit)" -ForegroundColor White
Write-Host ""

$choice = Read-Host "Enter choice (1-4)"

if ($choice -eq "1") {
    Write-Host ""
    Write-Host "Starting C++ application..." -ForegroundColor Green
    Write-Host "Commands: send <message>, peers, quit" -ForegroundColor Yellow
    Write-Host "-----------------------------------------" -ForegroundColor Gray
    .\aimlab_network.exe
}
elseif ($choice -eq "2") {
    Write-Host ""
    Write-Host "Starting Python application..." -ForegroundColor Green
    Write-Host "Commands: send <message>, peers, simulate, quit" -ForegroundColor Yellow
    Write-Host "-----------------------------------------" -ForegroundColor Gray
    python aimlab_network_python.py
}
elseif ($choice -eq "3") {
    Write-Host ""
    Write-Host "Starting both applications in new windows..." -ForegroundColor Green
    
    # Start C++ in new window
    Start-Process cmd -ArgumentList "/k", "aimlab_network.exe"
    
    # Wait a bit
    Start-Sleep -Seconds 2
    
    # Start Python in new window
    Start-Process cmd -ArgumentList "/k", "python", "aimlab_network_python.py"
    
    Write-Host "Both applications started!" -ForegroundColor Green
    Write-Host "They should auto-discover and connect." -ForegroundColor Yellow
    Write-Host ""
    pause
}
elseif ($choice -eq "4") {
    Write-Host ""
    Write-Host "Build complete. You can run:" -ForegroundColor Green
    Write-Host "  .\aimlab_network.exe" -ForegroundColor White
    Write-Host "  python aimlab_network_python.py" -ForegroundColor White
}
else {
    Write-Host "Invalid choice!" -ForegroundColor Red
}

Write-Host ""
Write-Host "Done!" -ForegroundColor Green