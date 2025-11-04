# AIMLAB-VR Motor Controller - Windows Native UI
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 02 November 2025  
**Version:** v2.0

---

## 🎯 Overview

A **lightweight Windows Native UI** application for AIMLAB-VR Motor Controller using **Win32 API**. This project demonstrates simple button-based controls and status display with **ZERO external dependencies**!

![Win32](https://img.shields.io/badge/Win32-Native-blue)
![Dependencies](https://img.shields.io/badge/Dependencies-None-green)
![C++](https://img.shields.io/badge/C++-17-orange)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![Size](https://img.shields.io/badge/Binary-~90KB-brightgreen)

---

## ✨ Features

- ✅ **Pure Win32 API** - No external libraries required!
- ✅ **Native Windows look and feel**
- ✅ **Tiny executable** (~90KB)
- ✅ **4 Interactive buttons**: Arduino/Unity refresh, Start/Stop recording
- ✅ **Real-time status display**
- ✅ **Clean, professional interface**
- ✅ **Simple one-command build** 
- ✅ **Professional code** with full documentation

---

## 🚀 Quick Start

### Prerequisites

1. **MinGW-w64** (g++ compiler) - That's it!
2. **No external libraries needed!**

### Build & Run

```powershell
# Navigate to directory
cd MotorController

# Build and run (one command!)
.\build.bat
```

**That's it!** No GLFW, no OpenGL setup, no external dependencies!

The application will compile in seconds and automatically launch.

---

## 📁 Project Structure

```
MotorController/
├── main.cpp                      # Win32 application (~200 lines)
├── build.bat                     # Build script
├── README.md                     # This file
├── main.exe                      # Compiled output (~90KB!)
│
└── [Legacy ImGui files]          # Not used in v2.0
    └── (kept for reference)
```

---

## 📖 Documentation

### Main Files

- **`main.cpp`**: Application code with ImGui setup and rendering loop
- **`build.bat`**: Compilation script for Windows
- **`SETUP_GUIDE.md`**: Comprehensive setup and troubleshooting guide

### Key Functions

```cpp
int main() {
    // 1. Initialize GLFW
    glfwInit();
    
    // 2. Create window
    GLFWwindow* window = glfwCreateWindow(...);
    
    // 3. Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    // 4. Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll events
        glfwPollEvents();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Your UI code here
        ImGui::Begin("Hello, World!");
        ImGui::Text("Hello from Dear ImGui!");
        ImGui::End();
        
        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(...);
        glfwSwapBuffers(window);
    }
    
    // 5. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
```

---

## 🎨 UI Features

The application includes three windows:

### 1. "Hello, World!" Window
- Project information (author, date)
- Interactive counter button
- FPS display
- Demo window toggle
- Background color picker

### 2. "About" Window
- Application version
- ImGui version info
- Backend information
- Close button

### 3. ImGui Demo Window (Optional)
- Comprehensive ImGui widget showcase
- Toggle on/off from main window

---

## 🔧 Building from Source

### Method 1: Using build.bat (Recommended)

```batch
.\build.bat
```

### Method 2: Manual Compilation

```bash
g++ main.cpp ^
    imgui.cpp ^
    imgui_draw.cpp ^
    imgui_tables.cpp ^
    imgui_widgets.cpp ^
    imgui_demo.cpp ^
    backends\imgui_impl_glfw.cpp ^
    backends\imgui_impl_opengl3.cpp ^
    -I. -Ibackends ^
    -lglfw3 -lopengl32 -lgdi32 -limm32 -luser32 -lkernel32 ^
    -o main.exe -std=c++17
```

---

## 📋 Requirements

### Software
- **MinGW-w64**: C++ compiler
  - Download: https://www.mingw-w64.org/
  
- **GLFW3**: Windowing and input library
  - Download: https://www.glfw.org/download.html
  
- **OpenGL**: Graphics API (included with Windows)

### Libraries
- `libglfw3.a` - GLFW static library
- `glfw3.dll` - GLFW dynamic library (optional)

---

## 🐛 Troubleshooting

### Common Issues

#### 1. "cannot find -lglfw3"
**Solution:** Install GLFW3 library in MinGW lib directory
```powershell
copy glfw\lib-mingw-w64\libglfw3.a C:\mingw64\lib\
```

#### 2. "GLFW/glfw3.h: No such file"
**Solution:** Install GLFW headers
```powershell
copy glfw\include\GLFW C:\mingw64\include\GLFW
```

#### 3. "glfw3.dll is missing"
**Solution:** Copy DLL to project directory
```powershell
copy glfw\lib-mingw-w64\glfw3.dll .
```

See **SETUP_GUIDE.md** for detailed troubleshooting.

---

## 📚 Learning Resources

### Dear ImGui
- **Official Repo**: https://github.com/ocornut/imgui
- **Getting Started**: https://github.com/ocornut/imgui/wiki/Getting-Started
- **Examples**: https://github.com/ocornut/imgui/tree/master/examples

### GLFW
- **Documentation**: https://www.glfw.org/documentation.html
- **Tutorial**: https://www.glfw.org/docs/latest/quick.html

### OpenGL
- **Learn OpenGL**: https://learnopengl.com/
- **Reference**: https://www.khronos.org/opengl/

---

## 🔄 Next Steps

### Extend the Application

1. **Add More UI Elements**
   ```cpp
   ImGui::Button("New Button");
   ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);
   ImGui::InputText("Text", buffer, 256);
   ```

2. **Add Custom Windows**
   ```cpp
   ImGui::Begin("My Window");
   // Your widgets here
   ImGui::End();
   ```

3. **Add File I/O**
   ```cpp
   if (ImGui::Button("Save")) {
       // Save data to file
   }
   ```

4. **Add Motor Control Logic**
   - Integrate motor controller API
   - Add motor parameter sliders
   - Display motor status
   - Create control panels

---

## 📝 License

This project uses **Dear ImGui** which is licensed under the MIT License.

See `LICENSE.txt` for Dear ImGui license details.

---

## 🤝 Contributing

This is an educational project for AIMLAB-VR research.

### Code Style
- Use consistent indentation (4 spaces)
- Add comments for complex logic
- Follow C++17 standards
- Keep functions focused and small

---

## 📞 Contact

**Author:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Project:** AIMLAB-VR Motor Controller  
**Institution:** New York University

---

## 🎓 Acknowledgments

- **Dear ImGui** by Omar Cornut
- **GLFW** by the GLFW team
- **OpenGL** by Khronos Group

---

## 📊 Version History

### v1.0 (02 November 2025)
- Initial release
- Basic ImGui setup with GLFW + OpenGL3
- Hello World window with interactive elements
- Build script and documentation
- Setup guide and troubleshooting

---

## 🎯 Future Plans

- [ ] Add motor controller integration
- [ ] Create advanced UI panels
- [ ] Add data visualization
- [ ] Implement configuration saving
- [ ] Add keyboard shortcuts
- [ ] Create custom ImGui themes
- [ ] Add logging system
- [ ] Implement error handling

---

*Last Updated: 02 November 2025*

