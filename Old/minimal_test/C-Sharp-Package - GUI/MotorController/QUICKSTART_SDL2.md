# AIMLAB-VR Motor Controller - Quick Start Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v5.0

---

## ⚡ Get Running in 5 Minutes

This guide will get you from zero to a working motor controller GUI in under 5 minutes.

---

## 📋 Prerequisites Checklist

Before you start, make sure you have:

- ✅ Windows 10 or 11 (64-bit)
- ✅ MinGW-w64 installed and in PATH
- ✅ Arduino with motor controller firmware
- ✅ USB cable to connect Arduino

### Check MinGW Installation

Open PowerShell and run:
```powershell
g++ --version
```

**Expected output:**
```
g++ (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 13.1.0
```

**If not installed:** Download from [winlibs.com](https://winlibs.com/) or [mingw-w64.org](https://www.mingw-w64.org/)

---

## 🚀 Step-by-Step Setup

### Step 1: Open PowerShell

```powershell
# Navigate to the MotorController directory
cd "C:\Users\YourUsername\...\C-Sharp-Package - GUI\MotorController"
```

### Step 2: Download SDL2

```powershell
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
```

**What this does:**
- Downloads SDL2 development libraries (v2.28.5)
- Extracts to `SDL2/` directory
- Takes about 30-60 seconds

**Expected output:**
```
Downloading SDL2...
Extracting SDL2...
[SUCCESS] SDL2 installed successfully!
```

### Step 3: Download ImGui Backends

```powershell
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
```

**What this does:**
- Downloads SDL2 rendering backends for ImGui
- Places in `backends/` directory
- Takes about 10-20 seconds

**Expected output:**
```
Downloading ImGui SDL2 backends...
[SUCCESS] Backends installed successfully!
```

### Step 4: Build the Application

```batch
.\build.bat
```

**What this does:**
- Compiles all source files
- Links against SDL2
- Copies SDL2.dll
- Creates `main.exe`
- Takes about 5-10 seconds

**Expected output:**
```
============================================================
  AIMLAB-VR Motor Controller Build System
============================================================
[INFO] Starting compilation...
[SUCCESS] Build completed successfully!

Output: main.exe
Would you like to run the application now? (y/n):
```

Type `y` and press Enter to run immediately, or `n` to run manually later.

### Step 5: Connect Arduino and Test

1. **Plug in your Arduino** with the motor controller
2. **Run the application** (if not already running):
   ```batch
   .\main.exe
   ```
3. **Click "Connect to / Refresh Vibration Motor"**
4. Wait for green status indicator
5. **Click "Test Vibration"** to verify

---

## 🎯 Visual Guide

### What You Should See

#### 1. After Building Successfully

```
MotorController/
├─ main.exe          ← Your compiled application (new!)
├─ SDL2.dll          ← Runtime library (new!)
├─ SDL2/             ← Development files
├─ backends/         ← ImGui backends
└─ imgui.cpp, ...    ← Source files
```

#### 2. When Application Starts

```
┌─────────────────────────────────────────────────┐
│ AIMLAB-VR Motor Controller                      │
├─────────────────────────────────────────────────┤
│ Author: Pi Ko (pi.ko@nyu.edu)                   │
│ Version: v5.0 - SDL2 + ImGui Software Renderer  │
├─────────────────────────────────────────────────┤
│  [Connect to / Refresh Vibration Motor]         │
│  [Test Vibration]                               │
├─────────────────────────────────────────────────┤
│ Status: NOT CONNECTED                       ●   │
│                                                  │
│ Instructions: Click 'Connect' to scan for       │
│ Arduino...                                       │
└─────────────────────────────────────────────────┘
```

#### 3. After Successful Connection

```
┌─────────────────────────────────────────────────┐
│ Status: CONNECTED [COM3]                    ●   │
│                                            ↑    │
│                                      Green circle
└─────────────────────────────────────────────────┘
```

---

## ❓ Common Issues

### Issue 1: "g++ is not recognized"

**Problem:** MinGW not in PATH

**Solution:**
1. Find MinGW installation (e.g., `C:\mingw64\`)
2. Add `C:\mingw64\bin` to system PATH
3. Restart PowerShell
4. Verify: `g++ --version`

**Step-by-step PATH setup:**
1. Press `Win + X`, select "System"
2. Click "Advanced system settings"
3. Click "Environment Variables"
4. Under "System variables", find "Path", click "Edit"
5. Click "New", add `C:\mingw64\bin`
6. Click "OK" on all dialogs
7. Restart PowerShell

### Issue 2: "SDL2 not found" during build

**Problem:** SDL2 not downloaded or extraction failed

**Solution:**
```powershell
# Re-run the download script
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1

# Verify SDL2 directory exists
Test-Path SDL2
# Should output: True
```

### Issue 3: "Cannot load SDL2.dll"

**Problem:** SDL2.dll not in same directory as main.exe

**Solution:**
```powershell
# Copy SDL2.dll manually
copy SDL2\x86_64-w64-mingw32\bin\SDL2.dll .

# Verify it's there
dir SDL2.dll
```

### Issue 4: No Arduino Found

**Problem:** Arduino not detected or on different COM port

**Diagnosis:**
1. Open Device Manager (`Win + X`, Device Manager)
2. Expand "Ports (COM & LPT)"
3. Look for "Arduino" or "USB Serial Device"
4. Note the COM number (e.g., COM3)

**Solutions:**
- Install Arduino drivers
- Try different USB port
- Close Arduino IDE (especially Serial Monitor!)
- Check USB cable (must support data, not just power)
- Verify Arduino has correct firmware

### Issue 5: Compilation Errors

**Problem:** Missing files or incompatible versions

**Solution:**
```powershell
# Verify all required files exist
Test-Path imgui.cpp
Test-Path backends\imgui_impl_sdl2.cpp
Test-Path backends\imgui_impl_sdlrenderer2.cpp

# If any return False, re-download ImGui backends
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
```

---

## 🔍 Troubleshooting Commands

### Check Your Setup

```powershell
# 1. Check MinGW
g++ --version

# 2. Check SDL2 installation
Test-Path SDL2\x86_64-w64-mingw32\lib\libSDL2.a

# 3. Check ImGui backends
Test-Path backends\imgui_impl_sdl2.cpp
Test-Path backends\imgui_impl_sdlrenderer2.cpp

# 4. List COM ports (requires Arduino CLI or similar)
mode  # Shows available COM ports

# 5. Check if main.exe was built
Test-Path main.exe
```

### Clean Build

If you encounter persistent issues, try a clean build:

```powershell
# Remove old build artifacts
Remove-Item main.exe -ErrorAction SilentlyContinue
Remove-Item SDL2.dll -ErrorAction SilentlyContinue

# Rebuild
.\build.bat
```

---

## 📝 Arduino Firmware Quick Upload

If your Arduino doesn't have the correct firmware:

### Arduino Code (Copy & Paste)

```cpp
// AIMLAB-VR Vibration Motor Controller
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 04 November 2025

const int MOTOR_PIN = 9;  // PWM pin for motor

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  
  // Wait for serial to be ready
  delay(1000);
  
  // Send ready message
  Serial.println("Vibration Motor Controller Ready");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();  // Remove whitespace
    
    if (cmd == "HELLO") {
      // Respond to handshake
      Serial.println("Vibration Motor Controller Ready");
    }
    else if (cmd == "1") {
      // Activate motor
      digitalWrite(MOTOR_PIN, HIGH);
      delay(500);
      digitalWrite(MOTOR_PIN, LOW);
      Serial.println("OK");
    }
  }
}
```

### Upload Steps

1. Open Arduino IDE
2. Copy the code above
3. Paste into a new sketch
4. Select correct board (Tools → Board → Arduino Uno/Nano/etc.)
5. Select correct port (Tools → Port → COMx)
6. Click Upload button (→)
7. Wait for "Done uploading"
8. **Close Serial Monitor if open!**

---

## ✅ Success Checklist

You're ready to use the application when:

- [ ] `g++ --version` works
- [ ] `SDL2/` directory exists with files
- [ ] `backends/` has `imgui_impl_sdl2.cpp` and `imgui_impl_sdlrenderer2.cpp`
- [ ] `build.bat` completes without errors
- [ ] `main.exe` exists
- [ ] `SDL2.dll` is next to `main.exe`
- [ ] Arduino is connected and shows up in Device Manager
- [ ] Arduino has correct firmware uploaded
- [ ] Arduino IDE Serial Monitor is CLOSED
- [ ] Application shows green "CONNECTED" status

---

## 🎮 Using the Application

### Normal Workflow

1. **Start Application**
   ```batch
   .\main.exe
   ```

2. **Connect to Arduino**
   - Click "Connect to / Refresh Vibration Motor"
   - Wait 1-3 seconds for scan
   - Status turns green when connected

3. **Test Motor**
   - Click "Test Vibration"
   - Motor should activate for ~500ms
   - Repeat as needed

4. **Disconnect**
   - Simply close the application window
   - Serial port automatically released

### Tips

- **Multiple attempts:** If first connection fails, click "Connect" again
- **USB issues:** Try unplugging and replugging Arduino
- **Port changes:** If Arduino COM port changes, just click "Connect" again
- **Responsiveness:** UI remains responsive during all operations

---

## 📊 Expected Performance

### Timing Reference

| Operation | Expected Time |
|-----------|---------------|
| Download SDL2 | 30-60 seconds |
| Download backends | 10-20 seconds |
| Compilation | 5-10 seconds |
| Application startup | <1 second |
| COM port scan | 1-3 seconds |
| Arduino handshake | 2-4 seconds |
| Test vibration | <100 ms |

If operations take significantly longer, check:
- Internet connection (for downloads)
- Antivirus (may slow compilation)
- USB connection (for Arduino communication)

---

## 🆘 Still Having Problems?

### Try These Steps

1. **Restart everything**
   ```powershell
   # Close all applications
   # Unplug Arduino
   # Restart PowerShell
   # Plug in Arduino
   # Try again
   ```

2. **Verify files manually**
   ```powershell
   # List all files
   dir -Recurse -Name
   
   # Should include:
   # - main.cpp
   # - imgui.cpp, imgui_draw.cpp, imgui_widgets.cpp, imgui_tables.cpp
   # - backends\imgui_impl_sdl2.cpp
   # - backends\imgui_impl_sdlrenderer2.cpp
   # - SDL2\x86_64-w64-mingw32\lib\libSDL2.a
   ```

3. **Check for antivirus interference**
   - Temporarily disable antivirus
   - Retry build
   - Re-enable antivirus

4. **Use verbose output**
   ```batch
   REM Edit build.bat, change:
   g++ main.cpp ... -o main.exe
   REM To:
   g++ main.cpp ... -o main.exe -v
   REM This shows detailed compilation info
   ```

### Get Help

If you're still stuck:

1. **Check full documentation**
   - [SDL2_REIMPLEMENTATION_COMPLETE.md](SDL2_REIMPLEMENTATION_COMPLETE.md)
   - [README_SDL2.md](README_SDL2.md)

2. **Collect information**
   - Windows version: `winver`
   - MinGW version: `g++ --version`
   - SDL2 version: Check `SDL2\README.txt`
   - Error messages (full text)
   - Steps that led to error

3. **Contact**
   - Email: pi.ko@nyu.edu
   - Include all information from step 2

---

## 🎉 You're All Set!

If you've made it here and everything works, congratulations! You now have a modern, portable, OpenGL-free motor controller GUI.

### Next Steps

- **Experiment** with different motor patterns
- **Modify** the Arduino code for custom behavior
- **Integrate** with your VR application
- **Share** your improvements

### Learn More

- [SDL2 Documentation](https://wiki.libsdl.org/)
- [Dear ImGui Documentation](https://github.com/ocornut/imgui)
- [Arduino Reference](https://www.arduino.cc/reference/en/)

---

**Happy Controlling! 🎮⚡**

---

**Last Updated:** 04 November 2025  
**Guide Version:** v5.0  
**Author:** Pi Ko (pi.ko@nyu.edu)

