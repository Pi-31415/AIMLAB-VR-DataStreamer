# Build Instructions

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 21 October 2025  
**Version:** v1.0

## Prerequisites

### Required Tools
- CMake 3.15 or later
- C++17 compatible compiler:
  - Windows: Visual Studio 2017 or later, or MinGW-w64
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+ or Clang 5+

### Optional Dependencies
- For real VR device support (future):
  - OpenVR SDK
  - Oculus SDK
  - OpenXR SDK

## Build Instructions

### Windows

#### Using Visual Studio

1. Open PowerShell and navigate to the project directory:
```powershell
cd path\to\AIMLAB-VR-DataStreamer
```

2. Create build directory:
```powershell
mkdir build
cd build
```

3. Generate Visual Studio solution:
```powershell
cmake ..
```

4. Build the project:
```powershell
cmake --build . --config Release
```

5. Executables will be in `build/bin/Release/`

#### Using MinGW

1. Generate Makefiles:
```powershell
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
```

2. Build:
```powershell
mingw32-make
```

### Linux

1. Install dependencies:
```bash
sudo apt-get install build-essential cmake
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

3. Executables will be in `build/bin/`

### macOS

1. Install Xcode Command Line Tools:
```bash
xcode-select --install
```

2. Install CMake (if not already installed):
```bash
brew install cmake
```

3. Build the project:
```bash
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Build Options

### CMake Options

- `BUILD_EXAMPLES` (default: ON): Build example applications
- `BUILD_TESTS` (default: ON): Build test applications
- `BUILD_SHARED_LIBS` (default: ON): Build shared libraries instead of static

Example:
```bash
cmake -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF ..
```

## Running Examples

### Basic Streaming Example

Windows:
```powershell
.\build\bin\Release\basic_streaming_example.exe
```

Linux/macOS:
```bash
./build/bin/basic_streaming_example
```

### Advanced Streaming Example

Windows:
```powershell
.\build\bin\Release\advanced_streaming_example.exe
```

Linux/macOS:
```bash
./build/bin/advanced_streaming_example
```

### Network Receiver Example

Windows:
```powershell
.\build\bin\Release\network_receiver_example.exe [port]
```

Linux/macOS:
```bash
./build/bin/network_receiver_example [port]
```

## Running Tests

### Unit Tests

```bash
# DataPacket tests
./build/bin/test_datapacket

# Logger tests
./build/bin/test_logger

# Integration tests
./build/bin/test_integration
```

## Installation

To install the library system-wide:

### Linux/macOS
```bash
cd build
sudo make install
```

### Windows
```powershell
cmake --install . --config Release
```

Default installation paths:
- Headers: `/usr/local/include/aimlab-vr-datastreamer/`
- Libraries: `/usr/local/lib/`
- Binaries: `/usr/local/bin/`

## Troubleshooting

### Windows: WinSock Error

If you get linker errors about WinSock:
- Make sure you're linking `ws2_32.lib`
- This is automatically handled by CMake

### Linux: Missing pthread

If you get pthread errors:
- Install: `sudo apt-get install libpthread-stubs0-dev`
- CMake automatically links pthread

### macOS: Permission Denied

If you get permission errors:
- Run: `sudo chown -R $USER build/`

### CMake Can't Find Compiler

Set compiler explicitly:
```bash
cmake -DCMAKE_CXX_COMPILER=/path/to/g++ ..
```

## Clean Build

To perform a clean build:

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
```

## IDE Integration

### Visual Studio Code

1. Install CMake Tools extension
2. Open project folder
3. Select compiler kit
4. Build using CMake Tools

### CLion

1. Open project folder
2. CLion will automatically detect CMake
3. Build using built-in tools

### Visual Studio

1. Open folder as CMake project
2. VS will automatically configure
3. Build using VS build tools

## Cross-Platform Notes

The codebase is designed to be platform-agnostic. The CMake build system automatically handles platform-specific configurations.

## Next Steps

After building, see:
- `API_REFERENCE.md` for API documentation
- `USAGE_GUIDE.md` for usage instructions
- Examples in `examples/` directory

---

For issues or questions, contact Pi Ko (pi.ko@nyu.edu)

