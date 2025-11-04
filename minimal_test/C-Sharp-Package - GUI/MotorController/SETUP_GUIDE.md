# Dear ImGui Hello World - Setup Guide
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 02 November 2025  
**Version:** 1.0

---

## 📋 Table of Contents

1. [Requirements](#requirements)
2. [GLFW3 Setup](#glfw3-setup)
3. [Building the Project](#building-the-project)
4. [Troubleshooting](#troubleshooting)
5. [Project Structure](#project-structure)

---

## 🔧 Requirements

### 1. MinGW-w64 (g++ compiler)

**Check if installed:**
```powershell
g++ --version
```

**If not installed:**
1. Download from: https://www.mingw-w64.org/
2. Or use: https://github.com/niXman/mingw-builds-binaries/releases
3. Add MinGW `bin` directory to system PATH
4. Verify installation: `g++ --version`

---

## 📦 GLFW3 Setup

GLFW is required for window management and OpenGL context creation.

### Option 1: Download Prebuilt Binaries (Recommended)

#### Step 1: Download GLFW
1. Visit: https://www.glfw.org/download.html
2. Download: **Windows pre-compiled binaries**
   - File: `glfw-3.x.x.bin.WIN64.zip` or `glfw-3.x.x.bin.WIN32.zip`

#### Step 2: Extract Files
Extract the downloaded ZIP file. You'll see this structure:
```
glfw-3.x.x.bin.WIN64/
├── include/
│   └── GLFW/
│       ├── glfw3.h
│       └── glfw3native.h
├── lib-mingw-w64/    (for MinGW 64-bit)
│   ├── libglfw3.a
│   └── glfw3.dll
└── lib-mingw/        (for MinGW 32-bit)
    ├── libglfw3.a
    └── glfw3.dll
```

#### Step 3: Install GLFW for MinGW

**Method A: Quick Install (Project-Local)**

Copy files to your project directory:
```powershell
# Navigate to MotorController directory
cd "C:\Users\paing\Documents\ResearchProjects\AIMLAB-VR-DataStreamer\minimal_test\C-Sharp-Package - GUI\MotorController"

# Copy DLL to project root
copy "path\to\glfw\lib-mingw-w64\glfw3.dll" .

# No need to copy .a file for this method
```

Then modify `build.bat` to include the library path:
```batch
g++ ... -L"path\to\glfw\lib-mingw-w64" -lglfw3 ...
```

**Method B: System-Wide Install (Recommended)**

1. **Copy Headers:**
   ```powershell
   # Find your MinGW include directory
   # Example: C:\mingw64\include\
   
   # Copy GLFW folder to MinGW include
   copy "path\to\glfw\include\GLFW" "C:\mingw64\include\GLFW" /Y
   ```

2. **Copy Library:**
   ```powershell
   # Find your MinGW lib directory
   # Example: C:\mingw64\lib\
   
   # Copy libglfw3.a
   copy "path\to\glfw\lib-mingw-w64\libglfw3.a" "C:\mingw64\lib\" /Y
   ```

3. **Copy DLL (Optional but Recommended):**
   ```powershell
   # Option 1: Copy to project directory
   copy "path\to\glfw\lib-mingw-w64\glfw3.dll" . /Y
   
   # Option 2: Copy to Windows System directory (requires admin)
   copy "path\to\glfw\lib-mingw-w64\glfw3.dll" "C:\Windows\System32\" /Y
   
   # Option 3: Add GLFW bin to PATH
   # Add path\to\glfw\lib-mingw-w64 to system PATH
   ```

### Option 2: Build GLFW from Source

If you prefer to build from source:

```powershell
# Clone GLFW repository
git clone https://github.com/glfw/glfw.git
cd glfw

# Build with CMake
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make

# Install (copies to MinGW directories)
mingw32-make install
```

---

## 🚀 Building the Project

### Step 1: Verify Setup

Check that everything is in place:
```powershell
# Check g++
g++ --version

# Check GLFW headers (if system-wide install)
ls C:\mingw64\include\GLFW\glfw3.h

# Check GLFW library (if system-wide install)
ls C:\mingw64\lib\libglfw3.a
```

### Step 2: Build

Navigate to the MotorController directory:
```powershell
cd "C:\Users\paing\Documents\ResearchProjects\AIMLAB-VR-DataStreamer\minimal_test\C-Sharp-Package - GUI\MotorController"
```

Run the build script:
```powershell
.\build.bat
```

### Step 3: Run

If build is successful, the program will automatically run.

Or run manually:
```powershell
.\main.exe
```

---

## 🐛 Troubleshooting

### Error: "cannot find -lglfw3"

**Cause:** Linker can't find GLFW library

**Solutions:**
1. Verify `libglfw3.a` is in MinGW lib directory
2. Add library path to build command:
   ```batch
   g++ ... -L"C:\path\to\glfw\lib-mingw-w64" -lglfw3 ...
   ```
3. Use absolute path:
   ```batch
   g++ ... "C:\path\to\glfw\lib-mingw-w64\libglfw3.a" ...
   ```

### Error: "GLFW/glfw3.h: No such file or directory"

**Cause:** Compiler can't find GLFW headers

**Solutions:**
1. Verify GLFW headers are in MinGW include directory
2. Add include path to build command:
   ```batch
   g++ ... -I"C:\path\to\glfw\include" ...
   ```

### Error: "The program can't start because glfw3.dll is missing"

**Cause:** Runtime can't find GLFW DLL

**Solutions:**
1. Copy `glfw3.dll` to the same directory as `main.exe`
2. Add DLL location to PATH
3. Copy DLL to `C:\Windows\System32`

### Error: Undefined reference to OpenGL functions

**Cause:** OpenGL library not linked

**Solution:** Ensure `-lopengl32` is in the linker flags (it should be in `build.bat`)

### Build Warning: "ISO C++ forbids converting..."

**Cause:** C++ standard version issue

**Solution:** Use `-std=c++17` or `-std=c++11` (already in `build.bat`)

---

## 📁 Project Structure

```
MotorController/
│
├── Core ImGui Files
│   ├── imgui.cpp
│   ├── imgui.h
│   ├── imgui_draw.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui_tables.cpp
│   ├── imgui_demo.cpp
│   ├── imconfig.h
│   ├── imgui_internal.h
│   ├── imstb_rectpack.h
│   ├── imstb_textedit.h
│   └── imstb_truetype.h
│
├── Backend Files
│   └── backends/
│       ├── imgui_impl_glfw.cpp
│       ├── imgui_impl_glfw.h
│       ├── imgui_impl_opengl3.cpp
│       └── imgui_impl_opengl3.h
│
├── Application Files
│   ├── main.cpp              # Your application code
│   └── build.bat             # Build script
│
├── Documentation
│   ├── SETUP_GUIDE.md        # This file
│   └── LICENSE.txt           # ImGui license
│
└── Build Output
    └── main.exe              # Compiled executable (after build)
```

---

## 🎯 Quick Start Checklist

- [ ] MinGW-w64 installed and in PATH
- [ ] GLFW3 headers in MinGW include directory
- [ ] GLFW3 library (libglfw3.a) in MinGW lib directory
- [ ] GLFW3 DLL (glfw3.dll) accessible (project dir or PATH)
- [ ] All ImGui files present
- [ ] Backend files downloaded
- [ ] Run `build.bat`
- [ ] Success! `main.exe` runs

---

## 📚 Additional Resources

### Official Documentation
- **Dear ImGui:** https://github.com/ocornut/imgui
- **GLFW:** https://www.glfw.org/documentation.html
- **MinGW-w64:** https://www.mingw-w64.org/

### Tutorials
- ImGui Getting Started: https://github.com/ocornut/imgui/wiki/Getting-Started
- GLFW Tutorial: https://www.glfw.org/docs/latest/quick.html

### Example Projects
- ImGui Examples: https://github.com/ocornut/imgui/tree/master/examples
- GLFW Examples: https://github.com/glfw/glfw/tree/master/examples

---

## 🔄 Alternative: CMake Build (Advanced)

If you prefer CMake over the batch script, create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(ImGuiHelloWorld)

set(CMAKE_CXX_STANDARD 17)

# Find GLFW
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# ImGui sources
set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp
)

# Create executable
add_executable(main main.cpp ${IMGUI_SOURCES})

# Include directories
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Link libraries
target_link_libraries(main PRIVATE
    glfw
    ${OPENGL_LIBRARIES}
    gdi32
    imm32
    user32
    kernel32
)
```

Build with CMake:
```powershell
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

---

## ✅ Verification

After successful build, you should see:

1. **Console Output:**
   ```
   ===========================================
     Dear ImGui Hello World - Motor Controller
     Author: Pi Ko (pi.ko@nyu.edu)
     Date: 02 November 2025
   ===========================================
   
   [INFO] Initializing GLFW...
   [INFO] Creating window...
   [INFO] Initializing Dear ImGui...
   [SUCCESS] Application initialized successfully!
   ```

2. **Window Display:**
   - Main window with title "Dear ImGui - Hello World (Motor Controller)"
   - "Hello, World!" panel with project information
   - "About" panel with version information
   - Optional ImGui Demo window

3. **File Created:**
   - `main.exe` in the MotorController directory

---

## 🆘 Need Help?

If you encounter issues:

1. Check this guide's troubleshooting section
2. Verify all requirements are met
3. Check compiler and linker output for specific errors
4. Ensure GLFW version is compatible (3.3+ recommended)
5. Try the Quick Install method if system-wide install fails

**Contact:**  
Pi Ko - pi.ko@nyu.edu

---

*Last Updated: 02 November 2025*

