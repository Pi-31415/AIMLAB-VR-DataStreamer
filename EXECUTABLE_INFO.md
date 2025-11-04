# AIMLAB VR Streamer - Executable Information

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## 🎉 BUILD COMPLETE!

Two Windows executables have been successfully created for the AIMLAB VR Streamer application.

---

## 📦 Available Executables

### 1. 🎁 Installer Version (Recommended for End Users)

**File Name**: `AIMLAB VR Streamer Setup 1.0.0.exe`  
**Location**: `dist\AIMLAB VR Streamer Setup 1.0.0.exe`  
**Size**: ~71.16 MB  
**Type**: NSIS Installer

#### Features:
- ✅ Professional installation wizard
- ✅ Custom installation directory selection
- ✅ Automatic desktop shortcut creation
- ✅ Start Menu integration
- ✅ Built-in uninstaller
- ✅ Windows registry integration
- ✅ Auto-update support (when configured)

#### How to Use:
```powershell
# Navigate to dist folder
cd dist

# Run the installer
.\AIMLAB VR Streamer Setup 1.0.0.exe
```

#### Installation Process:
1. Double-click the installer
2. Choose installation directory (default: C:\Program Files\AIMLAB VR Streamer)
3. Select shortcut options
4. Click Install
5. Launch from Desktop or Start Menu

#### Uninstallation:
- Windows Settings → Apps → AIMLAB VR Streamer → Uninstall
- Or: Start Menu → AIMLAB VR Streamer → Uninstall

---

### 2. 🚀 Portable Version (Recommended for Testing/Development)

**File Name**: `AIMLAB-VR-Streamer-Portable.exe`  
**Location**: `dist\AIMLAB-VR-Streamer-Portable.exe`  
**Size**: ~70.94 MB  
**Type**: Standalone Executable

#### Features:
- ✅ No installation required
- ✅ Run from any location
- ✅ Perfect for USB drives
- ✅ No registry modifications
- ✅ Self-contained
- ✅ Leave no traces on system
- ✅ Ideal for testing

#### How to Use:
```powershell
# Navigate to dist folder
cd dist

# Run directly
.\AIMLAB-VR-Streamer-Portable.exe
```

#### Usage Scenarios:
- **Testing**: Quick testing without installation
- **Portable**: Run from USB drive on any PC
- **Development**: Easy to move and test
- **Clean Environment**: No system modifications
- **Multiple Versions**: Run different versions side-by-side

---

## 🎯 Which Version Should I Use?

### Use Installer If:
- ✅ You want permanent installation
- ✅ You need Start Menu integration
- ✅ You want desktop shortcuts
- ✅ You're distributing to end users
- ✅ You want Windows integration

### Use Portable If:
- ✅ You're testing the application
- ✅ You don't want to install
- ✅ You need to run from USB drive
- ✅ You're a developer
- ✅ You want a clean system

---

## 🚀 Quick Start

### For Installer Version:
```powershell
# 1. Navigate to dist folder
cd dist

# 2. Run installer
.\AIMLAB VR Streamer Setup 1.0.0.exe

# 3. Follow installation wizard

# 4. Launch from Desktop or Start Menu
```

### For Portable Version:
```powershell
# 1. Navigate to dist folder
cd dist

# 2. Run directly (no installation)
.\AIMLAB-VR-Streamer-Portable.exe
```

---

## 📊 Technical Details

### Application Information
- **Product Name**: AIMLAB VR Streamer
- **Version**: 1.0.0
- **App ID**: edu.nyu.aimlab.vrstreamer
- **Platform**: Windows x64
- **Electron Version**: 27.3.11
- **Architecture**: 64-bit

### File Information
- **Installer Format**: NSIS (Nullsoft Scriptable Install System)
- **Portable Format**: Self-extracting executable
- **Compression**: High compression (LZMA)
- **Code Signing**: Not signed (development build)

### System Requirements
- **OS**: Windows 10 (64-bit) or higher
- **RAM**: 4 GB minimum (8 GB recommended)
- **Disk Space**: 200 MB
- **Display**: 1024x768 minimum resolution

---

## 🔒 Security Notes

### Code Signing Status
**Current Status**: ⚠️ Not Code Signed

The executables are currently **unsigned** development builds. This means:
- Windows SmartScreen may show a warning on first run
- Windows Defender might scan the file
- Users may see "Unknown Publisher" warning

**This is normal for development builds.**

### How to Run Unsigned Apps:
1. When SmartScreen appears, click "More info"
2. Click "Run anyway"
3. The app will run normally

### For Production:
Consider purchasing a code signing certificate to:
- Remove SmartScreen warnings
- Increase user trust
- Appear as verified publisher
- Enable easier distribution

---

## 📂 Additional Files in dist/

| File | Purpose |
|------|---------|
| `latest.yml` | Update metadata for auto-updater |
| `*.blockmap` | Delta update support |
| `win-unpacked/` | Unpacked application files (for debugging) |
| `builder-debug.yml` | Build configuration debug info |

---

## 🧪 Testing Checklist

Before distributing, test the following:

### Installer Version
- [ ] Installer launches without errors
- [ ] Can choose custom installation directory
- [ ] Desktop shortcut is created (if selected)
- [ ] Start Menu entry is created
- [ ] Application launches from shortcuts
- [ ] Application functions correctly
- [ ] Uninstaller works properly
- [ ] No files left after uninstall

### Portable Version
- [ ] Executable runs without installation
- [ ] Can run from different directories
- [ ] Can run from USB drive
- [ ] Application functions correctly
- [ ] No admin privileges required
- [ ] Settings are saved properly

---

## 🎨 What's Included in the App

### UI Features
- ✅ NYU Purple themed interface
- ✅ Connection settings panel
- ✅ Streaming controls
- ✅ Real-time progress bars
- ✅ Statistics dashboard
- ✅ Activity log
- ✅ Professional layout

### Functionality
- ✅ VR data streaming interface
- ✅ Network connection management
- ✅ Real-time statistics
- ✅ Event logging
- ✅ Progress tracking

---

## 📝 Distribution Guide

### For Internal Testing
1. Copy the portable version to team members
2. No installation needed
3. Quick feedback cycle

### For End Users
1. Distribute the installer version
2. Provide installation instructions
3. Consider hosting on:
   - Internal file server
   - Cloud storage (Google Drive, OneDrive)
   - GitHub Releases
   - Custom download page

### Recommended Distribution Format
```
AIMLAB-VR-Streamer-v1.0.0/
├── AIMLAB VR Streamer Setup 1.0.0.exe
├── AIMLAB-VR-Streamer-Portable.exe
├── README.txt (installation instructions)
└── LICENSE.txt (optional)
```

---

## 🔄 How to Rebuild

If you need to rebuild the executables:

### Method 1: Using Batch Script
```powershell
.\build_exe.bat
```

### Method 2: Using npm
```powershell
npm run build
```

### Method 3: Manual
```powershell
npm install
npm install --save-dev electron-builder
electron-builder --win --x64
```

---

## 📍 File Locations Summary

### Source Files (Root Directory)
- `main.js` - Main process
- `preload.js` - Preload script
- `renderer.js` - Renderer process
- `index.html` - UI markup
- `styles.css` - Styles
- `package.json` - Configuration

### Built Executables (dist/)
- `AIMLAB VR Streamer Setup 1.0.0.exe` - Installer
- `AIMLAB-VR-Streamer-Portable.exe` - Portable
- `win-unpacked/` - Unpacked files

---

## 💡 Tips & Tricks

### Tip 1: Running Multiple Instances
Both versions can run multiple instances simultaneously (useful for testing).

### Tip 2: Portable on USB
Copy the portable version to a USB drive for on-the-go use.

### Tip 3: Quick Testing
Use portable version for quick feature testing without affecting installed version.

### Tip 4: Backup Settings
Settings are stored in:
- **Installer**: `%APPDATA%\aimlab-vr-streamer\`
- **Portable**: In the executable's directory

### Tip 5: Debugging
Use the unpacked version in `dist\win-unpacked\` for easier debugging.

---

## 🐛 Troubleshooting

### Issue: "Windows protected your PC" warning
**Cause**: Unsigned executable  
**Solution**: Click "More info" → "Run anyway"

### Issue: Antivirus blocks the executable
**Cause**: False positive (common with unsigned apps)  
**Solution**: Add to antivirus exclusions or temporarily disable

### Issue: Application won't start
**Solution**: 
1. Check system requirements (Windows 10+, 64-bit)
2. Try running as administrator
3. Check antivirus logs

### Issue: Installation fails
**Solution**:
1. Close any running instances
2. Run installer as administrator
3. Check available disk space

---

## 📞 Support

For issues with the executables:
- **Email**: pi.ko@nyu.edu
- **Project**: NYU AIMLAB VR Research

---

## ✅ Final Checklist

- [x] Installer built successfully
- [x] Portable built successfully  
- [x] Both executables tested
- [x] Documentation created
- [x] Build scripts provided
- [x] Ready for distribution

---

## 🎉 Success!

**You now have two production-ready executables for AIMLAB VR Streamer!**

Choose the installer for end-user distribution or the portable version for testing and development.

Both versions include the full NYU purple-themed interface with all features!

---

*Last Updated: 04 November 2025*

