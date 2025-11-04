# Quick Start Guide - AIMLAB-VR Motor Controller
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v4.0 (SDL2 + ImGui)

---

## ⚡ 5-Minute Setup

### Step 1: Check Prerequisites ✓
```powershell
# Check if g++ is installed
g++ --version
```

**Expected output:** `g++ (x86_64-posix-seh-rev0, Built by MinGW-W64 project)...`

**If not found:** Download MinGW-w64 from https://www.mingw-w64.org/

---

### Step 2: Download Dependencies 📦
```powershell
cd "C-Sharp-Package - GUI\MotorController"

# Download SDL2 (takes ~30 seconds)
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1

# Download ImGui backends (takes ~10 seconds)
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
```

**What you should see:**
```
[SUCCESS] Downloaded SDL2 successfully!
[SUCCESS] All backend files downloaded successfully!
```

---

### Step 3: Build Application 🔨
```powershell
.\build.bat
```

**Expected output:**
```
[SUCCESS] Build completed successfully!
Output: main.exe
```

---

### Step 4: Upload Arduino Firmware 🤖

Open Arduino IDE and upload this code:

```cpp
/**
 * AIMLAB-VR Vibration Motor Controller Firmware
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 November 2025
 */

const int MOTOR_PIN = 9;  // Change to your motor pin

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  
  delay(1000);
  Serial.println("Vibration Motor Controller Ready");
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    if (cmd == '1') {
      // Activate vibration for 500ms
      digitalWrite(MOTOR_PIN, HIGH);
      delay(500);
      digitalWrite(MOTOR_PIN, LOW);
    }
  }
}
```

---

### Step 5: Run Application 🚀
```powershell
.\main.exe
```

**What you should see:**  
A window titled "AIMLAB-VR Motor Controller - SDL2 + ImGui" with:
- Connect button
- Test Vibration button
- Status indicator (initially red)

---

### Step 6: Connect to Arduino 🔌

1. **Plug in Arduino** to USB port
2. **Click** "Connect to / Refresh Vibration Motor" button
3. **Wait** ~3-5 seconds for scanning
4. **Status should turn GREEN** with COM port displayed

**Troubleshooting:**
- If status stays red: Check Arduino USB connection
- If no response: Verify firmware was uploaded correctly
- If wrong COM port: Close other serial monitors (Arduino IDE, PuTTY, etc.)

---

### Step 7: Test Vibration 🎉

1. **Click** "Test Vibration" button
2. **Motor should activate** for 0.5 seconds
3. **Repeat** as many times as you want!

---

## 🎯 That's It!

You now have a fully functional vibration motor controller with a modern GUI!

---

## 📁 File Structure After Setup

```
MotorController/
├── main.exe              ← Your application
├── SDL2.dll              ← Runtime library (auto-copied)
├── main.cpp              ← Source code
├── build.bat             ← Build script
│
├── SDL2/                 ← SDL2 development files
├── backends/             ← ImGui backends
│   ├── imgui_impl_sdl2.cpp
│   ├── imgui_impl_sdl2.h
│   ├── imgui_impl_sdlrenderer2.cpp
│   └── imgui_impl_sdlrenderer2.h
│
└── [ImGui core files]
```

---

## 🔄 Rebuilding After Changes

If you modify `main.cpp`:

```powershell
.\build.bat
```

That's it! Dependencies are already downloaded.

---

## 🐛 Common Issues & Solutions

### Issue 1: "g++ is not recognized"
**Solution:**
```powershell
# Add MinGW to PATH
$env:PATH += ";C:\mingw64\bin"  # Adjust path as needed
```

### Issue 2: "SDL2 not found"
**Solution:**
```powershell
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
```

### Issue 3: "Cannot connect to Arduino"
**Solutions:**
- Close Arduino IDE Serial Monitor
- Unplug and replug Arduino
- Check COM port in Device Manager
- Verify firmware is uploaded

### Issue 4: "Application doesn't start"
**Solution:**
```powershell
# Ensure SDL2.dll is present
copy SDL2\x86_64-w64-mingw32\bin\SDL2.dll .
```

---

## 🎓 Next Steps

### Customize the GUI
Edit `main.cpp` and modify the ImGui code:

```cpp
// Change button size
ImGui::Button("My Button", ImVec2(300, 50));

// Add new text
ImGui::Text("Hello World!");

// Add slider
static float value = 0.0f;
ImGui::SliderFloat("Intensity", &value, 0.0f, 100.0f);
```

### Add Features
Some ideas:
- Multiple motor support
- Vibration intensity control
- Pattern editor
- Data logging
- Real-time graphs

### Port to Linux/Mac
SDL2 + ImGui is cross-platform!
- Replace Windows serial code with termios (Linux/Mac)
- Update build script for target platform
- Everything else should work!

---

## 📞 Need Help?

**Author:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Documentation:** See README.md for detailed information  
**Migration Guide:** See SDL2_MIGRATION_GUIDE.md  

---

## 🎉 Congratulations!

You've successfully set up the AIMLAB-VR Motor Controller with a modern SDL2 + ImGui interface!

**Enjoy building your VR motor control experiments!** 🚀

---

## 📋 Quick Reference Commands

```powershell
# Setup (one-time)
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1

# Build
.\build.bat

# Run
.\main.exe

# Clean build (if needed)
del main.exe
.\build.bat
```

---

**Last Updated:** 04 November 2025  
**Version:** v4.0 (SDL2 + ImGui Software Renderer)

