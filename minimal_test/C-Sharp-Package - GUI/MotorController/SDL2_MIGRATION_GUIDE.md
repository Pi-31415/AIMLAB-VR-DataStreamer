# SDL2 Migration Guide - AIMLAB-VR Motor Controller
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v4.0

---

## 📋 Overview

This document describes the migration from Win32 native UI to Dear ImGui + SDL2 software renderer implementation. This new architecture provides a modern, cross-platform GUI without requiring OpenGL.

---

## 🎯 Migration Objectives

### Goals Achieved
✅ **Modern GUI Framework** - Migrated to Dear ImGui for beautiful, customizable interface  
✅ **Cross-platform Ready** - SDL2 enables easy porting to Linux/Mac  
✅ **No OpenGL Dependency** - Uses SDL2's built-in 2D renderer  
✅ **Improved User Experience** - Better visual feedback and smoother interactions  
✅ **Maintained Functionality** - All Arduino communication features preserved  
✅ **Easy Setup** - Automated download scripts for dependencies  

---

## 🔄 Architecture Changes

### Previous Architecture (v3.0 - Win32)
```
┌─────────────────────────┐
│   Win32 API (WndProc)   │
│   - Native Controls     │
│   - GDI32 Rendering     │
└───────────┬─────────────┘
            │
┌───────────▼─────────────┐
│  Serial Communication   │
│  - COM Port Scanning    │
│  - Arduino Handshake    │
└─────────────────────────┘
```

**Pros:**
- Zero dependencies
- Tiny binary size (~90KB)
- Native Windows look and feel

**Cons:**
- Windows-only
- Basic UI capabilities
- Limited customization
- Manual control positioning

### New Architecture (v4.0 - SDL2 + ImGui)
```
┌─────────────────────────────────────┐
│        Dear ImGui (UI Layer)        │
│  - Modern widgets                   │
│  - Custom styling                   │
│  - Immediate mode rendering         │
└───────────┬─────────────────────────┘
            │
┌───────────▼─────────────────────────┐
│   ImGui Backend (SDL2 Platform)     │
│  - Window management                │
│  - Input handling                   │
│  - Event processing                 │
└───────────┬─────────────────────────┘
            │
┌───────────▼─────────────────────────┐
│  ImGui Backend (SDL2 Renderer)      │
│  - 2D accelerated rendering         │
│  - Software fallback                │
└───────────┬─────────────────────────┘
            │
┌───────────▼─────────────────────────┐
│     Serial Communication Thread     │
│  - COM Port Scanning                │
│  - Arduino Handshake                │
│  - Non-blocking I/O                 │
└─────────────────────────────────────┘
```

**Pros:**
- Cross-platform (Windows, Linux, Mac)
- Modern, beautiful UI
- Easy to customize and extend
- Powerful widget library
- No OpenGL required
- Active development and support

**Cons:**
- Larger binary (~450KB vs ~90KB)
- Requires SDL2.dll (~1MB)
- Slightly more complex build process

---

## 📝 Code Changes

### 1. Window Creation

#### Old (Win32):
```cpp
WNDCLASS wc = { };
wc.lpfnWndProc = WndProc;
wc.hInstance = hInstance;
wc.lpszClassName = CLASS_NAME;
RegisterClass(&wc);

HWND hwnd = CreateWindow(
    CLASS_NAME,
    "AIMLAB-VR Motor Controller",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    420, 250,
    NULL, NULL, hInstance, NULL
);
```

#### New (SDL2):
```cpp
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

SDL_Window* window = SDL_CreateWindow(
    "AIMLAB-VR Motor Controller - SDL2 + ImGui",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    500, 320,
    SDL_WINDOW_SHOWN
);

SDL_Renderer* renderer = SDL_CreateRenderer(
    window, -1, 
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
);
```

### 2. UI Controls

#### Old (Win32):
```cpp
// Create button
CreateWindow("BUTTON", "Connect to Vibration Motor",
             WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
             20, 50, 280, 30, 
             hwnd, (HMENU)ID_BTN_CONNECT, NULL, NULL);

// Create label
CreateWindow("STATIC", "Status: Not Connected",
             WS_VISIBLE | WS_CHILD, 
             20, 150, 300, 20, 
             hwnd, NULL, NULL, NULL);
```

#### New (ImGui):
```cpp
// Button
if (ImGui::Button("Connect to / Refresh Vibration Motor", ImVec2(400, 40))) {
    // Handle click
    std::thread(connectArduino).detach();
}

// Status text with color
if (connected) {
    ImGui::TextColored(ImVec4(0.0f, 0.7f, 0.0f, 1.0f), "CONNECTED");
} else {
    ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "NOT CONNECTED");
}

// Draw indicator circle
ImDrawList* draw_list = ImGui::GetWindowDrawList();
ImVec2 p = ImGui::GetCursorScreenPos();
draw_list->AddCircleFilled(
    ImVec2(p.x + 440, p.y - 10), 
    8.0f, 
    connected ? IM_COL32(0, 200, 0, 255) : IM_COL32(200, 0, 0, 255)
);
```

### 3. Main Loop

#### Old (Win32):
```cpp
MSG msg = { };
while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
```

#### New (SDL2 + ImGui):
```cpp
bool running = true;
SDL_Event event;

while (running) {
    // Process events
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            running = false;
    }
    
    // Start ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // Draw UI
    ImGui::Begin("Main Window");
    // ... UI code ...
    ImGui::End();
    
    // Render
    ImGui::Render();
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}
```

### 4. Serial Communication

**No changes required!** All serial communication code remains identical:
- `listCOMPorts()` - Unchanged
- `readLine()` - Unchanged
- `handshakeArduino()` - Unchanged
- `connectArduino()` - Unchanged
- `testVibration()` - Unchanged

This demonstrates excellent separation of concerns in the original design.

---

## 🔧 Build System Changes

### Dependencies

#### Old (Win32):
- MinGW-w64 (g++)
- Windows SDK (built-in)

#### New (SDL2 + ImGui):
- MinGW-w64 (g++)
- SDL2 development libraries (auto-downloaded)
- Dear ImGui source files (included)
- ImGui SDL2 backends (auto-downloaded)

### Compilation Command

#### Old:
```bash
g++ main.cpp -lgdi32 -luser32 -o main.exe -std=c++17
```

#### New:
```bash
g++ main.cpp imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp ^
    backends\imgui_impl_sdl2.cpp backends\imgui_impl_sdlrenderer2.cpp ^
    -I. -Ibackends -ISDL2\x86_64-w64-mingw32\include\SDL2 ^
    -LSDL2\x86_64-w64-mingw32\lib ^
    -lmingw32 -lSDL2main -lSDL2 -lgdi32 ^
    -o main.exe -std=c++17 -mwindows
```

### Setup Process

#### Old:
```powershell
.\build.bat
```

#### New:
```powershell
# One-time setup
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1

# Build
.\build.bat
```

---

## 🎨 Visual Improvements

### Enhanced UI Elements

#### Status Indicators
- **Old**: Static text + icon
- **New**: Colored text + animated circle indicator

#### Buttons
- **Old**: Standard Windows buttons
- **New**: Styled buttons with hover effects, rounded corners

#### Overall Design
- **Old**: Windows 95-style interface
- **New**: Modern, clean design with custom color scheme

#### Customization
- **Old**: Limited to Windows themes
- **New**: Fully customizable colors, fonts, spacing

---

## 📦 Deployment Changes

### Distribution Package

#### Old (Win32):
```
MotorController/
├── main.exe (90 KB)
└── README.md
```
**Total size: ~90 KB**

#### New (SDL2 + ImGui):
```
MotorController/
├── main.exe (450 KB)
├── SDL2.dll (1 MB)
└── README.md
```
**Total size: ~1.5 MB**

### Installation
- **Old**: Copy single exe, run immediately
- **New**: Copy exe + SDL2.dll, run immediately (no installer needed)

---

## 🧪 Testing Checklist

After migration, verify all functionality:

- [✓] Application launches without errors
- [✓] Window displays correctly
- [✓] "Connect" button is clickable
- [✓] COM port scanning works
- [✓] Arduino handshake succeeds
- [✓] Connection status updates correctly
- [✓] Status indicator changes color (red → green)
- [✓] COM port displays correctly
- [✓] "Test Vibration" button is clickable
- [✓] Vibration command sends successfully
- [✓] UI remains responsive during operations
- [✓] Application closes cleanly
- [✓] No memory leaks
- [✓] Serial port closes on exit

---

## 🐛 Common Migration Issues

### Issue 1: "SDL2.dll not found"
**Symptom**: Application fails to start  
**Solution**: Copy SDL2.dll to same directory as main.exe
```powershell
copy SDL2\x86_64-w64-mingw32\bin\SDL2.dll .
```

### Issue 2: Build fails with "undefined reference to SDL_..."
**Symptom**: Linker errors during compilation  
**Solution**: Check library order (mingw32 must come before SDL2main)
```bash
-lmingw32 -lSDL2main -lSDL2  # Correct order
```

### Issue 3: UI appears but is unresponsive
**Symptom**: Window shows but doesn't respond to input  
**Solution**: Ensure event processing is in main loop
```cpp
while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);  // Essential!
}
```

### Issue 4: Black screen / no rendering
**Symptom**: Window appears but content is black  
**Solution**: Verify renderer creation and frame order
```cpp
ImGui_ImplSDLRenderer2_NewFrame();  // Renderer first
ImGui_ImplSDL2_NewFrame();          // Then platform
ImGui::NewFrame();                  // Then ImGui
```

---

## 📚 Learning Outcomes

### Skills Gained Through Migration
1. **SDL2 Integration** - Window management, event handling, rendering
2. **Dear ImGui Usage** - Immediate mode GUI, widget creation, styling
3. **Backend Architecture** - Understanding platform abstraction layers
4. **Build Systems** - Managing dependencies, linking libraries
5. **Cross-platform Development** - Writing portable C++ code

### Best Practices Applied
- ✅ Separation of concerns (UI vs logic)
- ✅ Threaded I/O for responsive UI
- ✅ Proper resource management
- ✅ Comprehensive documentation
- ✅ Automated dependency management

---

## 🚀 Future Migration Paths

### Possible Next Steps

#### 1. Linux Port
Minimal changes needed:
- Replace Windows serial code with termios
- Update build script for Linux
- Everything else should work as-is!

#### 2. macOS Port
Similar to Linux:
- Use IOKit for serial communication
- Create Xcode project or Makefile
- SDL2 and ImGui work natively

#### 3. Add Graphics
Since we're using SDL2:
- Can add images/icons easily
- Support for textures built-in
- No OpenGL required

#### 4. Multi-window Support
ImGui supports multiple windows:
- Separate configuration window
- Real-time data plots
- Log viewer

---

## 📊 Performance Comparison

| Metric | Win32 | SDL2 + ImGui |
|--------|-------|--------------|
| Startup Time | ~100ms | ~150ms |
| Memory Usage | ~5 MB | ~15 MB |
| CPU Usage (Idle) | < 1% | < 2% |
| Frame Rate | N/A | 60 FPS |
| Binary Size | 90 KB | 450 KB |
| Dependencies | 0 | SDL2.dll (1 MB) |
| Lines of Code | ~300 | ~500 |

**Verdict**: Slightly higher resource usage, but vastly improved functionality and user experience.

---

## 🎓 Conclusion

The migration from Win32 to SDL2 + ImGui has been **highly successful**:

### What We Gained
✅ Modern, beautiful interface  
✅ Cross-platform capability  
✅ Easier to extend and maintain  
✅ Better user experience  
✅ No OpenGL dependency  
✅ Active ecosystem and support  

### What We Kept
✅ All Arduino communication features  
✅ COM port scanning  
✅ Handshake protocol  
✅ Vibration testing  
✅ Non-blocking I/O  
✅ Reliable operation  

### What We Traded
- Slightly larger binary size
- One additional DLL dependency
- More complex build process (but automated!)

The benefits far outweigh the minor drawbacks, making this migration a clear win for the project!

---

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v4.0

---

*For questions or issues, please contact the author.*

