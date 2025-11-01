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
│   ├── aimlab_vr_datastreamer_simple.cpp   # Simplified version (recommended)
│   └── aimlab_vr_datastreamer_fixed.cpp    # Fixed version (Windows headers)
│
├── 🎮 Unity C# Scripts
│   ├── AIMLABVRDataStreamer.cs             # Main Unity streamer
│   └── AIMLABVRDataStreamer_Modern.cs      # Modern variant
│
├── 📦 Unity Package
│   └── AIMLAB_Streamer_Unity_Package.unitypackage
│
├── 🗂️ Data Directory
│   └── aimlab_data/                        # Output data files (CSV)
│
└── 🚀 Compiled Executables (after compilation)
    ├── aimlab_streamer.exe                 # Simplified version
    └── aimlab_streamer_fixed.exe           # Fixed version
```

---

## 📋 File Descriptions

### Core Files

#### `compile_aimlab_v2.bat`
**Purpose:** Interactive batch compiler for Windows  
**Features:**
- Checks for g++ compiler availability
- Offers 3 compilation options:
  1. Simplified version (recommended)
  2. Fixed version (with Windows headers)
  3. Both versions
- Auto-creates `aimlab_data/` directory
- Provides detailed usage instructions

**Usage:**
```cmd
compile_aimlab_v2.bat
```

---

### C++ Source Files

#### `aimlab_vr_datastreamer_simple.cpp`
**Purpose:** Simplified C++ data logger with better compatibility  
**Features:**
- Cross-platform socket implementation
- Simplified directory creation
- Better Windows/Linux compatibility
- Recommended for most users

**Compilation:**
```cmd
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer_simple.cpp -lws2_32 -std=c++11
```

#### `aimlab_vr_datastreamer_fixed.cpp`
**Purpose:** Fixed version with proper Windows headers  
**Features:**
- Full Windows API support
- Proper directory attribute checking
- Advanced Windows-specific features

**Compilation:**
```cmd
g++ -o aimlab_streamer_fixed.exe aimlab_vr_datastreamer_fixed.cpp -lws2_32 -std=c++11
```

---

### Unity C# Scripts

#### `AIMLABVRDataStreamer.cs`
**Purpose:** Main Unity integration script  
**Features:**
- Auto-discovery protocol
- UDP communication with C++ node
- File management commands
- Data streaming (Vector3, Quaternion, Transform)
- UI integration support

**Usage:**
1. Copy to Unity Assets folder
2. Attach to GameObject
3. Configure in Inspector
4. Call methods from your scripts

#### `AIMLABVRDataStreamer_Modern.cs`
**Purpose:** Modern variant with enhanced features  
**Note:** Check this file for additional modern C# patterns

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
Follow the prompts to select version.

**Option B: Manual Compilation**
```cmd
# Simplified version
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer_simple.cpp -lws2_32 -std=c++11

# Fixed version
g++ -o aimlab_streamer_fixed.exe aimlab_vr_datastreamer_fixed.cpp -lws2_32 -std=c++11
```

### 2. Unity Setup

1. Copy `AIMLABVRDataStreamer.cs` to Unity Assets
2. Attach to GameObject
3. Configure autoConnect in Inspector

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
- Removed duplicate/obsolete files
- Added v2 batch compiler with version selection
- Separated simplified and fixed C++ versions
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

