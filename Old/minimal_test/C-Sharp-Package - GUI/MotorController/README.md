# AIMLAB-VR Motor Controller - SDL2 + ImGui Software Renderer
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v4.0

---

## 🎯 Overview

A **modern, cross-platform GUI** for AIMLAB-VR Motor Controller using **Dear ImGui + SDL2 Software Renderer**. This implementation uses SDL2's built-in accelerated 2D renderer with automatic fallback to software rendering - **NO OpenGL required**!

![SDL2](https://img.shields.io/badge/SDL2-2.28.5-blue)
![ImGui](https://img.shields.io/badge/Dear_ImGui-1.92.5-purple)
![Dependencies](https://img.shields.io/badge/No_OpenGL-green)
![C++](https://img.shields.io/badge/C++-17-orange)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![Size](https://img.shields.io/badge/Binary-~450KB-brightgreen)

---

## ✨ Features

### Core Functionality
- ✅ **COM Port Scanning** - Automatic detection of Arduino devices (COM1-COM40)
- ✅ **Serial Handshake** - Reliable communication protocol with Arduino
- ✅ **Vibration Testing** - One-click motor test functionality
- ✅ **Connection Status** - Real-time visual feedback with color indicators
- ✅ **Non-blocking UI** - Threaded serial operations for smooth interface

### Technical Features
- ✅ **SDL2 Software Renderer** - No OpenGL dependency!
- ✅ **Dear ImGui Integration** - Modern, beautiful interface
- ✅ **Cross-platform Ready** - SDL2 enables easy porting to Linux/Mac
- ✅ **Small Binary** - Self-contained executable (~450KB + SDL2.dll)
- ✅ **Professional Code** - Full documentation and error handling

---

## 🚀 Quick Start

### Prerequisites

1. **MinGW-w64** (g++ compiler)
   - Download: https://www.mingw-w64.org/
   - Make sure `g++` is in your PATH

2. **PowerShell** (for automated setup)
   - Built into Windows 10/11

### Installation (Automated)

```powershell
# Navigate to directory
cd "C-Sharp-Package - GUI\MotorController"

# Step 1: Download SDL2 libraries
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1

# Step 2: Download Dear ImGui SDL2 backends
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1

# Step 3: Build the application
.\build.bat
```

**That's it!** The application will compile and be ready to run.

---

## 📁 Project Structure

```
MotorController/
│
├── main.cpp                          # Main application (SDL2 + ImGui)
├── main_win32_backup.cpp             # Previous Win32 version (backup)
│
├── imgui/                            # Dear ImGui core files
│   ├── imgui.cpp, imgui.h
│   ├── imgui_draw.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui_tables.cpp
│   └── imgui_demo.cpp
│
├── backends/                         # ImGui backend implementations
│   ├── imgui_impl_sdl2.cpp          # SDL2 platform backend
│   ├── imgui_impl_sdl2.h
│   ├── imgui_impl_sdlrenderer2.cpp  # SDL2 renderer backend
│   └── imgui_impl_sdlrenderer2.h
│
├── SDL2/                            # SDL2 development libraries
│   └── x86_64-w64-mingw32/
│       ├── include/
│       ├── lib/
│       └── bin/
│
├── build.bat                        # Build script
├── download_sdl2.ps1                # SDL2 auto-downloader
├── download_imgui_sdl2_backends.ps1 # Backend auto-downloader
│
├── main.exe                         # Compiled executable
├── SDL2.dll                         # SDL2 runtime library
│
└── Documentation/
    ├── README.md                    # This file
    ├── SETUP_GUIDE.md              # Detailed setup instructions
    └── SERIAL_PROTOCOL.md          # Arduino communication protocol
```

---

## 🔧 Building from Source

### Method 1: Using build.bat (Recommended)

```batch
.\build.bat
```

The build script will:
1. Check for SDL2 installation
2. Check for ImGui backends
3. Compile all source files
4. Link with SDL2 and Windows libraries
5. Copy SDL2.dll to executable directory
6. Display build summary

### Method 2: Manual Compilation

```bash
g++ main.cpp ^
    imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp ^
    backends\imgui_impl_sdl2.cpp backends\imgui_impl_sdlrenderer2.cpp ^
    -I. -Ibackends -ISDL2\x86_64-w64-mingw32\include\SDL2 ^
    -LSDL2\x86_64-w64-mingw32\lib ^
    -lmingw32 -lSDL2main -lSDL2 -lgdi32 ^
    -o main.exe -std=c++17 -mwindows
```

---

## 📋 Dependencies

### Build-time Dependencies
- **MinGW-w64**: C++ compiler toolchain
- **SDL2 Development Libraries**: Headers and static libraries
- **Dear ImGui**: Included in project (no separate installation needed)

### Runtime Dependencies
- **SDL2.dll**: Automatically copied by build script
- **Windows GDI32**: System library (always available)

### Library Versions
- SDL2: 2.28.5 (MinGW)
- Dear ImGui: 1.92.5 WIP
- C++ Standard: C++17

---

## 💡 How It Works

### Architecture Overview

```
┌─────────────────────────────────────────────┐
│           User Interface (ImGui)            │
│  - Buttons, Status Display, Text            │
└───────────────┬─────────────────────────────┘
                │
┌───────────────▼─────────────────────────────┐
│       SDL2 Software Renderer                │
│  - 2D Accelerated Rendering                 │
│  - Automatic fallback to software           │
└───────────────┬─────────────────────────────┘
                │
┌───────────────▼─────────────────────────────┐
│     Serial Communication Thread             │
│  - COM Port Scanning                        │
│  - Arduino Handshake                        │
│  - Non-blocking I/O                         │
└─────────────────────────────────────────────┘
```

### Key Components

#### 1. Serial Communication
- **Port Scanning**: Iterates through COM1-COM40, attempts to open each port
- **Handshake Protocol**: 
  1. Open serial port (triggers Arduino reset)
  2. Wait 2 seconds for Arduino bootloader
  3. Send "HELLO\r\n" command
  4. Wait for "Vibration Motor Controller Ready" response
  5. Connection established on success

#### 2. GUI System
- **ImGui Context**: Manages UI state and rendering
- **SDL2 Platform Backend**: Handles window, input, clipboard
- **SDL2 Renderer Backend**: Renders ImGui draw data using SDL2 2D API
- **Custom Styling**: Professional light theme with rounded corners

#### 3. Threading Model
- **Main Thread**: UI rendering and event processing
- **Background Threads**: Serial communication (connect, test vibration)
- **Atomic Variables**: Thread-safe state management

---

## 🎨 UI Features

### Main Window
- **Title Bar**: Application name and version
- **Author Info**: Contact information
- **Connect Button**: Initiates COM port scan and Arduino handshake
- **Test Vibration Button**: Sends vibration command to Arduino
- **Status Display**: Shows connection state with color-coded indicator
  - 🔴 Red: Not connected
  - 🟢 Green: Connected (displays COM port)
- **Instructions**: User guidance text

### Visual Elements
- Modern rounded corners
- Smooth hover effects
- Color-coded status indicators
- Professional color scheme
- Responsive button states

---

## 🔬 Arduino Communication Protocol

### Serial Settings
- **Baud Rate**: 9600
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Flow Control**: None

### Command Protocol

| Command | Description | Response |
|---------|-------------|----------|
| `HELLO\r\n` | Handshake request | `Vibration Motor Controller Ready\n` |
| `1\n` | Trigger vibration | (Motor activates for set duration) |

### Example Arduino Firmware

```cpp
// AIMLAB-VR Vibration Motor Controller Firmware
// Author: Pi Ko (pi.ko@nyu.edu)

const int MOTOR_PIN = 9;

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

## 🐛 Troubleshooting

### Common Issues

#### 1. "SDL2 not found"
**Solution:** Run the SDL2 downloader script
```powershell
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
```

#### 2. "SDL2 ImGui backends not found"
**Solution:** Run the backend downloader script
```powershell
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
```

#### 3. "g++ is not recognized"
**Solution:** Install MinGW-w64 and add to PATH
```powershell
# Check if g++ is available
g++ --version

# If not found, install MinGW-w64 and add to PATH
```

#### 4. "Application crashes on startup"
**Solution:** Ensure SDL2.dll is in same directory as main.exe
```powershell
copy SDL2\x86_64-w64-mingw32\bin\SDL2.dll .
```

#### 5. "Cannot connect to Arduino"
**Solution:** 
- Check Arduino is plugged in and recognized by Windows
- Verify COM port in Device Manager
- Ensure correct firmware is uploaded to Arduino
- Try closing other serial monitor applications

#### 6. "Compilation fails with undefined reference"
**Solution:** Check library linking order in build command
- Ensure `-lmingw32` comes before `-lSDL2main`
- Verify SDL2 library path is correct

---

## 📚 Learning Resources

### SDL2
- **Official Website**: https://www.libsdl.org/
- **Documentation**: https://wiki.libsdl.org/
- **Download**: https://github.com/libsdl-org/SDL/releases

### Dear ImGui
- **Official Repo**: https://github.com/ocornut/imgui
- **Getting Started**: https://github.com/ocornut/imgui/wiki/Getting-Started
- **Examples**: https://github.com/ocornut/imgui/tree/master/examples
- **SDL2 Renderer Example**: https://github.com/ocornut/imgui/tree/master/examples/example_sdl2_sdlrenderer2

### C++ Serial Communication
- **Windows Serial**: https://docs.microsoft.com/en-us/windows/win32/devio/communications-resources
- **CreateFile API**: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea

---

## 🔄 Version History

### v4.0 (04 November 2025) - SDL2 + ImGui Software Renderer
- **NEW**: Migrated to Dear ImGui + SDL2 software renderer
- **NEW**: No OpenGL dependency - uses SDL2's built-in 2D renderer
- **NEW**: Automated SDL2 and backend download scripts
- **NEW**: Modern ImGui interface with custom styling
- **NEW**: Improved build system with dependency checking
- **IMPROVED**: Better error handling and user feedback
- **IMPROVED**: Enhanced documentation and setup guides

### v3.0 (02 November 2025) - Win32 Native
- Full serial communication with Arduino motor controller
- COM port auto-detection
- Handshake protocol implementation
- Connection status indicators

### v2.0 (02 November 2025) - Win32 API
- Switched to Windows Native Controls
- Pure Win32 implementation
- Zero external dependencies (except MinGW)

### v1.0 (02 November 2025) - ImGui + GLFW + OpenGL3
- Initial ImGui implementation
- GLFW window management
- OpenGL3 rendering backend

---

## 🎯 Future Enhancements

### Planned Features
- [ ] **Linux/Mac Support** - Cross-platform builds using SDL2
- [ ] **Multiple Motor Control** - Support for multiple vibration motors
- [ ] **Pattern Editor** - Create custom vibration patterns
- [ ] **Data Logging** - Record vibration events to file
- [ ] **Configuration Save/Load** - Persistent settings
- [ ] **Custom Themes** - Dark mode and custom color schemes
- [ ] **Multi-language Support** - Internationalization
- [ ] **Advanced Diagnostics** - Motor health monitoring

### Possible Improvements
- [ ] Add vibration intensity slider
- [ ] Implement vibration pattern presets
- [ ] Add COM port manual selection
- [ ] Include battery level monitoring (if applicable)
- [ ] Create installer package
- [ ] Add automatic firmware update capability

---

## 📝 Technical Details

### Why SDL2 Software Renderer?

#### Advantages
1. **No OpenGL Requirement**: Works on systems without GPU drivers
2. **Automatic Fallback**: SDL2 falls back to software rendering if needed
3. **Simpler Setup**: Fewer dependencies than OpenGL
4. **Better Compatibility**: Works on older systems
5. **Smaller Binary**: No OpenGL libraries needed
6. **Easy Debugging**: Simpler rendering pipeline

#### Performance
- **GUI Rendering**: More than sufficient for UI applications
- **Frame Rate**: 60 FPS easily achievable
- **CPU Usage**: Minimal (< 5% on modern CPUs)
- **Memory**: Low footprint (~10-20 MB)

### Code Quality
- **Documentation**: Full Doxygen-style comments
- **Error Handling**: Comprehensive error checking
- **Thread Safety**: Atomic variables for shared state
- **Resource Management**: Proper cleanup on exit
- **Modern C++**: C++17 features and best practices

---

## 🤝 Contributing

This is an educational project for AIMLAB-VR research.

### Code Style Guidelines
- Use consistent indentation (4 spaces)
- Add Doxygen comments for all functions
- Follow C++17 standards
- Keep functions focused and small
- Use meaningful variable names

### Reporting Issues
If you encounter issues:
1. Check the troubleshooting section
2. Verify all dependencies are installed
3. Test with the example Arduino firmware
4. Document steps to reproduce the issue

---

## 📞 Contact

**Author:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Project:** AIMLAB-VR Motor Controller  
**Institution:** New York University  
**Version:** v4.0 (SDL2 + ImGui Software Renderer)

---

## 🎓 Acknowledgments

- **Dear ImGui** by Omar Cornut - Excellent immediate mode GUI library
- **SDL2** by the SDL team - Robust cross-platform media layer
- **MinGW-w64** - Windows port of GNU Compiler Collection
- **Arduino** - Accessible hardware prototyping platform

---

## 📄 License

This project uses **Dear ImGui** which is licensed under the MIT License.

**SDL2** is licensed under the zlib license.

See `LICENSE.txt` for detailed license information.

---

## 📊 Comparison with Previous Versions

| Feature | v1.0 (GLFW+OpenGL) | v2.0 (Win32) | v4.0 (SDL2+ImGui) |
|---------|-------------------|--------------|-------------------|
| GUI Framework | ImGui | Native Win32 | ImGui |
| Rendering | OpenGL 3 | GDI32 | SDL2 Renderer |
| Dependencies | GLFW, OpenGL | None | SDL2 |
| Binary Size | ~1.2 MB | ~90 KB | ~450 KB |
| Cross-platform | Yes | No | Yes |
| Modern UI | Yes | Basic | Yes |
| OpenGL Required | Yes | No | No |
| Ease of Setup | Medium | Easy | Easy |
| Customizability | High | Low | High |

---

## 🚀 Getting Started Checklist

- [ ] Install MinGW-w64
- [ ] Download SDL2 libraries (`.\download_sdl2.ps1`)
- [ ] Download ImGui backends (`.\download_imgui_sdl2_backends.ps1`)
- [ ] Build application (`.\build.bat`)
- [ ] Upload firmware to Arduino
- [ ] Connect Arduino to PC
- [ ] Run application (`.\main.exe`)
- [ ] Click "Connect" button
- [ ] Test vibration

---

*Last Updated: 04 November 2025*

**Happy Coding! 🎉**
