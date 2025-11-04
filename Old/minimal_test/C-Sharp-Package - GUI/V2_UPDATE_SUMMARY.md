# v2.0 Update Summary - Default Versions
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 02 November 2025  
**Update:** Made fixed C++ and Modern C# the default versions

---

## 🎯 Objective

Update the repository to make the fixed C++ version and Modern C# (TextMeshPro) version the default, while preserving legacy versions for compatibility.

---

## ✅ Changes Completed

### 1. C++ Changes

#### Removed:
- ❌ `aimlab_vr_datastreamer_simple.cpp` (simplified version deleted)

#### Renamed:
- 📝 `aimlab_vr_datastreamer_fixed.cpp` → `aimlab_vr_datastreamer.cpp`

#### Updated:
- ✅ Added version v2.0 and date (02 November 2025)
- ✅ Added comprehensive changelog
- ✅ Updated banner to show v2.0

**New Header:**
```cpp
/**
 * @file aimlab_vr_datastreamer.cpp
 * @brief AIMLAB-VR-Data Streamer - C++ Node
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 02 November 2025
 * @version v2.0
 * 
 * Changelog:
 * - v2.0 (02 November 2025): Made fixed version the default, proper Windows headers
 * - v1.0 (2025): Initial release
 */
```

---

### 2. C# Changes

#### Renamed:
- 📝 `AIMLABVRDataStreamer.cs` → `AIMLABVRDataStreamer_Legacy.cs`
- 📝 `AIMLABVRDataStreamer_Modern.cs` → `AIMLABVRDataStreamer.cs`

#### Updated:
**Main File** (`AIMLABVRDataStreamer.cs`):
- ✅ Now the Modern version with TextMeshPro
- ✅ Version v2.0 with date
- ✅ Added comprehensive changelog
- ✅ Updated requirements documentation

**Legacy File** (`AIMLABVRDataStreamer_Legacy.cs`):
- ✅ Marked as v1.0 (Legacy)
- ✅ Added note directing users to v2.0
- ✅ Clear documentation about when to use

---

### 3. Compiler Updates

#### `compile_aimlab_v2.bat`

**Before:**
- Interactive menu with 3 options
- Compiled simplified, fixed, or both versions
- Required user to choose version

**After:**
- ✅ Streamlined single version compilation
- ✅ Compiles `aimlab_vr_datastreamer.cpp` → `aimlab_streamer.exe`
- ✅ Updated instructions for modern defaults
- ✅ Mentions TextMeshPro requirements
- ✅ References legacy version for older Unity

**Command:**
```batch
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11
```

---

### 4. Documentation Updates

#### `README_AIMLAB.md`
- ✅ Updated changelog to reflect v2.0 changes
- ✅ Simplified compilation instructions (single version)
- ✅ Added TextMeshPro requirements for Unity
- ✅ Documented legacy version availability
- ✅ Updated version history

#### `REPOSITORY_STRUCTURE.md`
- ✅ Updated file tree to show new structure
- ✅ Removed references to simplified/fixed versions
- ✅ Added Modern/Legacy C# documentation
- ✅ Updated Quick Start guide
- ✅ Added TextMeshPro setup steps
- ✅ Updated changelog

#### `.gitignore`
- ✅ No changes needed (already properly configured)

---

## 📊 Repository Structure Comparison

### Before v2.0 Update:
```
├── aimlab_vr_datastreamer_simple.cpp    ❌ Removed
├── aimlab_vr_datastreamer_fixed.cpp     📝 Renamed
├── AIMLABVRDataStreamer.cs              📝 Renamed to Legacy
└── AIMLABVRDataStreamer_Modern.cs       📝 Renamed to main
```

### After v2.0 Update:
```
├── aimlab_vr_datastreamer.cpp           ✨ Main version (was fixed)
├── AIMLABVRDataStreamer.cs              ✨ Main version (was Modern)
└── AIMLABVRDataStreamer_Legacy.cs       💾 Legacy version (was main)
```

---

## 🎯 Default Versions Now

| Component | Default Version | Features |
|-----------|----------------|----------|
| **C++** | Fixed version (v2.0) | Proper Windows headers, full API support |
| **C#** | Modern version (v2.0) | TextMeshPro, Input System, XR support |
| **Compiler** | Single build | Streamlined compilation process |

---

## 💾 Legacy Support

Legacy versions are preserved for compatibility:

### `AIMLABVRDataStreamer_Legacy.cs`
- **Version:** v1.0
- **Purpose:** Older Unity projects
- **Features:** Standard UI, no TextMeshPro requirement
- **When to Use:**
  - Projects without TextMeshPro
  - Older Unity versions
  - Simple projects without modern UI

---

## 📦 Final File List

```
Repository Root:
├── .gitignore                              # Git ignore rules
├── aimlab_streamer.exe                     # Compiled executable (preserved)
├── aimlab_streamer_fixed.exe               # Old executable (will be obsolete)
├── AIMLAB_Streamer_Unity_Package.unitypackage
├── aimlab_vr_datastreamer.cpp              ✨ MAIN C++ (v2.0)
├── AIMLABVRDataStreamer.cs                 ✨ MAIN C# (v2.0)
├── AIMLABVRDataStreamer_Legacy.cs          💾 Legacy C# (v1.0)
├── CLEANUP_SUMMARY.md                      # Previous cleanup doc
├── compile_aimlab_v2.bat                   ✅ Updated compiler
├── README_AIMLAB.md                        ✅ Updated docs
├── REPOSITORY_STRUCTURE.md                 ✅ Updated docs
└── V2_UPDATE_SUMMARY.md                    # This file

Data Directory:
└── aimlab_data/                            # Empty, ready for use
```

---

## 🚀 Usage Instructions (Updated)

### For New Projects:

#### 1. Compile C++:
```cmd
compile_aimlab_v2.bat
```

#### 2. Setup Unity:
1. Install TextMeshPro package
2. Copy `AIMLABVRDataStreamer.cs` to Assets
3. Attach to GameObject
4. Configure settings

#### 3. Run:
```cmd
./aimlab_streamer.exe
```

### For Legacy Projects:

Use `AIMLABVRDataStreamer_Legacy.cs` instead of the main version if:
- Project doesn't have TextMeshPro
- Using older Unity version
- Don't want to add new dependencies

---

## 🔧 Technical Details

### C++ Version (v2.0)
**Features:**
- WIN32_LEAN_AND_MEAN
- Proper Windows.h header order
- Full Windows API support
- Cross-platform (Windows/Linux)
- Directory attribute checking
- Advanced file operations

**Compilation:**
```cpp
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <direct.h>
#else
    // Linux headers
#endif
```

### C# Version (v2.0)
**Features:**
- TextMeshPro UI components
- Unity Input System support (optional)
- XR Plugin Management support (optional)
- Modern async patterns
- Enhanced VR controller support

**Requirements:**
```csharp
using TMPro;  // TextMeshPro
using UnityEngine.InputSystem;  // Optional
using UnityEngine.XR;  // Optional
```

---

## ✨ Benefits of v2.0 Defaults

### 1. **Simplified Setup**
- Single C++ version to compile
- No confusion about which version to use
- Clearer documentation

### 2. **Better Features**
- Proper Windows API integration
- Modern Unity UI with TextMeshPro
- Enhanced VR support

### 3. **Maintained Compatibility**
- Legacy versions preserved
- Backward compatibility ensured
- Easy migration path

### 4. **Professional Quality**
- Industry-standard headers
- Modern C# patterns
- Best practices implemented

---

## 📝 Migration Guide

### From v1.0 to v2.0:

#### If Using Simplified C++:
```bash
# Old:
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer_simple.cpp -lws2_32 -std=c++11

# New:
g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11
```

#### If Using Old C#:
1. Install TextMeshPro in Unity
2. Replace old script with `AIMLABVRDataStreamer.cs`
3. Update UI references to TextMeshProUGUI
4. **OR** Use `AIMLABVRDataStreamer_Legacy.cs` if not ready to migrate

---

## 🎓 Version Summary

| Version | C++ File | C# File | Compiler Options |
|---------|----------|---------|------------------|
| v1.0 | Multiple versions | Single version | 3 options |
| v2.0 | Single version (fixed) | Modern + Legacy | Single build |

---

## ⚡ Performance & Quality

### C++ Improvements:
- ✅ Proper header inclusion order
- ✅ Better Windows API integration
- ✅ Enhanced error handling
- ✅ Professional code structure

### C# Improvements:
- ✅ TextMeshPro for better UI rendering
- ✅ Input System for modern VR devices
- ✅ XR support for latest Unity
- ✅ Enhanced modularity

---

## 🔍 Verification

### Verify Installation:

1. **Check Files Exist:**
```cmd
dir aimlab_vr_datastreamer.cpp
dir AIMLABVRDataStreamer.cs
dir AIMLABVRDataStreamer_Legacy.cs
```

2. **Compile Test:**
```cmd
compile_aimlab_v2.bat
```

3. **Run Test:**
```cmd
aimlab_streamer.exe
```
Should show: `Version: v2.0`

---

## 📞 Support

**Author:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Project:** AIMLAB VR Research at NYU

---

## 🎉 Summary

### What Changed:
- ✅ Fixed C++ version is now the default
- ✅ Modern C# with TextMeshPro is now the default
- ✅ Simplified compiler (single version)
- ✅ Legacy support maintained
- ✅ All documentation updated

### Files Modified: 7
1. `aimlab_vr_datastreamer.cpp` (renamed + updated)
2. `AIMLABVRDataStreamer.cs` (renamed + updated)
3. `AIMLABVRDataStreamer_Legacy.cs` (renamed + updated)
4. `compile_aimlab_v2.bat` (simplified)
5. `README_AIMLAB.md` (updated)
6. `REPOSITORY_STRUCTURE.md` (updated)
7. `.gitignore` (verified)

### Files Deleted: 1
1. `aimlab_vr_datastreamer_simple.cpp`

### Files Created: 1
1. `V2_UPDATE_SUMMARY.md` (this file)

---

**Update Status:** ✅ Complete  
**Date Completed:** 02 November 2025  
**Ready for Use:** Yes

---

*End of v2.0 Update Summary*

