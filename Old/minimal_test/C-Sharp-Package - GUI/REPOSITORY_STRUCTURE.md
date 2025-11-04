# AIMLAB-VR-Data Streamer - Repository Structure
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 02 November 2025  
**Version:** v2.0

---

## 📁 Repository Structure

```
C-Sharp-Package - GUI/
│
├── 📄 README_AIMLAB.md              # Main documentation
├── 📄 REPOSITORY_STRUCTURE.md       # This file - repository guide
├── 📄 .gitignore                    # Git ignore rules
│
├── 🔧 compile_aimlab_v2.bat         # v2 Compiler (RECOMMENDED)
│
├── 💻 C++ Source Files
│   └── aimlab_vr_datastreamer.cpp          # Main C++ streamer (v2.0 - Fixed version)
│
├── 🎮 Unity C# Scripts
│   ├── AIMLABVRDataStreamer.cs             # Main Unity streamer (v2.0 - Modern with TextMeshPro)
│   └── AIMLABVRDataStreamer_Legacy.cs      # Legacy version (for older Unity)
│
├── 📦 Unity Package
│   └── AIMLAB_Streamer_Unity_Package.unitypackage
│
├── 🗂️ Data Directory
│   └── aimlab_data/                        # Output data files (CSV)
│
└── 🚀 Compiled Executable (after compilation)
    └── aimlab_streamer.exe                 # Main executable
```

---

## 📋 File Descriptions

### Core Files

#### `compile_aimlab_v2.bat`
**Purpose:** Batch compiler for Windows  
**Features:**
- Checks for g++ compiler availability
- Compiles the main C++ data streamer
- Auto-creates `aimlab_data/` directory
- Provides detailed usage instructions

**Usage:**
```cmd
compile_aimlab_v2.bat
```

**Output:** `aimlab_streamer.exe`

---

### C++ Source Files

#### `aimlab_vr_datastreamer.cpp`
**Purpose:** Main C++ data logger with proper Windows headers (v2.0)  
**Features:**
- Full Windows API support
- Proper directory attribute checking
- Cross-platform compatibility (Windows/Linux)
- Advanced Windows-specific features
- Auto-discovery protocol
- Real-time data streaming

**Compilation:**
```cmd
# Windows
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11

# Linux
g++ -o aimlab_streamer aimlab_vr_datastreamer.cpp -pthread -std=c++11
```

---

### Unity C# Scripts

#### `AIMLABVRDataStreamer.cs`
**Purpose:** Main Unity integration script (v2.0 - Modern Version)  
**Features:**
- Auto-discovery protocol
- UDP communication with C++ node
- File management commands
- Data streaming (Vector3, Quaternion, Transform)
- TextMeshPro UI support
- Unity Input System integration (optional)
- XR/VR support

**Requirements:**
- TextMeshPro package
- Input System package (optional)
- XR Plugin Management (for VR, optional)

**Usage:**
1. Copy to Unity Assets folder
2. Attach to GameObject
3. Configure in Inspector
4. Call methods from your scripts

#### `AIMLABVRDataStreamer_Legacy.cs`
**Purpose:** Legacy Unity integration script (v1.0)  
**Features:**
- Same core functionality as v2.0
- Uses standard Unity UI instead of TextMeshPro
- Compatible with older Unity versions
- No special package requirements

**When to Use:**
- Older Unity projects without TextMeshPro
- Projects that need to maintain compatibility
- Simple projects without modern UI needs

---

### Package Files

#### `AIMLAB_Streamer_Unity_Package.unitypackage`
**Purpose:** Pre-packaged Unity assets  
**Contents:** Unity scripts and example scenes  
**Import:** Assets → Import Package → Custom Package

---

### Documentation

#### `README_AIMLAB.md`
**Purpose:** Comprehensive documentation  
**Sections:**
- Installation instructions
- Usage examples
- API reference
- Troubleshooting
- Network configuration
- Version history

#### `REPOSITORY_STRUCTURE.md`
**Purpose:** This file - repository organization guide

#### `.gitignore`
**Purpose:** Git ignore rules  
**Ignores:**
- Compiled executables (*.exe)
- Object files (*.o, *.obj)
- Data files (*.csv in aimlab_data/)
- IDE files (.vscode, .vs)
- OS files (.DS_Store, Thumbs.db)

---

## 🚀 Quick Start Guide

### 1. Compilation

**Option A: Using the v2 Batch Compiler (Recommended)**
```cmd
compile_aimlab_v2.bat
```

**Option B: Manual Compilation**
```cmd
# Windows
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11

# Linux
g++ -o aimlab_streamer aimlab_vr_datastreamer.cpp -pthread -std=c++11
```

### 2. Unity Setup

1. Copy `AIMLABVRDataStreamer.cs` to Unity Assets
2. Ensure TextMeshPro package is installed
3. Attach script to GameObject
4. Configure autoConnect in Inspector
5. (Optional) For older Unity, use `AIMLABVRDataStreamer_Legacy.cs`

### 3. Run

1. Start C++ executable: `./aimlab_streamer.exe`
2. Play Unity scene
3. Applications auto-connect
4. Use commands or UI to control file operations

---

## 🧹 Cleaned Files (Removed in v2.0)

The following files were removed during repository cleanup:

- ❌ `aimlab_vr_datastreamer.cpp` (duplicate of fixed version)
- ❌ `compile_aimlab.bat` (replaced by v2)
- ❌ `csharp3.cs` (temporary test file)
- ❌ `aimlab_data/Hi` (test file)
- ❌ `aimlab_data/Hi2` (test file)

---

## 📊 Data Output

### Directory: `aimlab_data/`
**Purpose:** Stores all CSV data files  
**Format:** CSV with columns: Timestamp, Type, Data  
**Naming Convention:**
- Auto-generated: `aimlab_session_YYYYMMDD_HHMMSS_N.csv`
- Custom: User-specified name

**Example Data:**
```csv
Timestamp,Type,Data
1698765432000,HeadPosition,1.234,0.567,2.890
1698765432010,HeadRotation,0.000,0.707,0.000,0.707
1698765432020,EventType,TriggerPressed
```

---

## 🔧 Version Compatibility

### v2.0 Improvements
- ✅ Cleaned duplicate files
- ✅ Organized structure
- ✅ Interactive compiler
- ✅ Version selection
- ✅ Better documentation
- ✅ Git ignore rules

### v1.0 Features (Preserved)
- ✅ Auto-discovery
- ✅ File management
- ✅ Real-time streaming
- ✅ Unity integration

---

## 🛠️ Development Notes

### Adding New Features

1. **C++ Side:** Edit `aimlab_vr_datastreamer_simple.cpp`
2. **Unity Side:** Edit `AIMLABVRDataStreamer.cs`
3. **Recompile:** Run `compile_aimlab_v2.bat`
4. **Test:** Run both applications and verify communication

### Network Ports

- **Discovery:** 45000-45009 (UDP broadcast)
- **C++ Data Port:** 45100
- **Unity Data Port:** 45101

### Firewall Configuration

Allow UDP traffic on ports 45000-45101

---

## 📝 Changelog

### v2.0 (02 November 2025)
- Made fixed C++ version the default
- Made Modern C# (TextMeshPro) the default
- Simplified compiler (single version build)
- Legacy versions preserved for compatibility
- Removed duplicate/obsolete files
- Cleaned up data directory
- Updated documentation
- Added .gitignore
- Added REPOSITORY_STRUCTURE.md

### v1.0 (2025)
- Initial release
- Auto-discovery protocol
- File management
- Real-time streaming
- Unity integration

---

## 📞 Support

**Author:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Project:** AIMLAB VR Research  

---

## 📄 License

Educational and research use  
Created for AIMLAB VR research projects at NYU

---

*Last Updated: 02 November 2025*

