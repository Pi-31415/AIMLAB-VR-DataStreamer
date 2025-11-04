# AIMLAB VR Streamer - Build Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.1

**Changelog:**
- v1.1 - 04 November 2025 - Added executable build instructions
  - Configured electron-builder
  - Created installer and portable versions
  - Disabled code signing for development builds
- v1.0 - 04 November 2025 - Initial documentation

---

## ✅ Build Status: SUCCESS

The application has been successfully built into Windows executables!

---

## 📦 Built Executables

Two types of executables have been created:

### 1. Installer Version
- **File**: `AIMLAB VR Streamer Setup 1.0.0.exe`
- **Size**: ~71.16 MB
- **Type**: NSIS Installer
- **Features**:
  - Installation wizard
  - Choose installation directory
  - Desktop shortcut creation
  - Start Menu shortcut creation
  - Uninstaller included
  - Auto-update support (when configured)

### 2. Portable Version
- **File**: `AIMLAB-VR-Streamer-Portable.exe`
- **Size**: ~70.94 MB
- **Type**: Standalone executable
- **Features**:
  - No installation required
  - Run from any location (USB drive, network drive, etc.)
  - No registry modifications
  - Perfect for testing or portable use

---

## 📂 Output Directory

All built files are located in:
```
dist/
├── AIMLAB VR Streamer Setup 1.0.0.exe      (Installer)
├── AIMLAB-VR-Streamer-Portable.exe          (Portable)
├── AIMLAB VR Streamer Setup 1.0.0.exe.blockmap
├── latest.yml                                (Update metadata)
└── win-unpacked/                             (Unpacked files)
    └── AIMLAB VR Streamer.exe
```

---

## 🚀 How to Build

### Method 1: NPM Script (Recommended)
```powershell
npm run build
```

### Method 2: Build Directory Only (Faster, for testing)
```powershell
npm run build:dir
```

### Method 3: Full Distribution
```powershell
npm run dist
```

---

## 🛠️ Build Configuration

### package.json Configuration

The build is configured in `package.json` under the `"build"` section:

```json
{
  "build": {
    "appId": "edu.nyu.aimlab.vrstreamer",
    "productName": "AIMLAB VR Streamer",
    "copyright": "Copyright © 2025 Pi Ko, NYU",
    "win": {
      "target": ["nsis", "portable"],
      "signAndEditExecutable": false,
      "verifyUpdateCodeSignature": false
    },
    "nsis": {
      "oneClick": false,
      "allowToChangeInstallationDirectory": true,
      "createDesktopShortcut": true,
      "createStartMenuShortcut": true
    }
  }
}
```

---

## 📋 Build Requirements

### System Requirements
- **OS**: Windows 10/11 (for building Windows executables)
- **Node.js**: v14 or higher
- **npm**: v6 or higher
- **Disk Space**: ~500 MB free space

### Dependencies
- **electron**: ^27.0.0
- **electron-builder**: ^26.0.12

---

## 🎯 Distribution Options

### For End Users - Installer
**Recommended for most users**

```powershell
# Run the installer
.\dist\AIMLAB VR Streamer Setup 1.0.0.exe
```

**Features:**
- Standard Windows installation experience
- Integrated with Windows Start Menu
- Desktop shortcut option
- Easy uninstallation through Control Panel

### For Testing - Portable
**Best for developers and testing**

```powershell
# Just run the portable exe
.\dist\AIMLAB-VR-Streamer-Portable.exe
```

**Features:**
- No installation needed
- Self-contained
- Can run from USB drive
- Leave no traces on system

---

## 🔧 Advanced Build Options

### Building for Different Architectures

#### x64 (Default)
```powershell
npm run build
```

#### ia32 (32-bit)
```powershell
electron-builder --win --ia32
```

#### Both architectures
```powershell
electron-builder --win --x64 --ia32
```

### Building Different Target Types

#### NSIS Installer Only
```powershell
electron-builder --win --x64 nsis
```

#### Portable Only
```powershell
electron-builder --win --x64 portable
```

#### ZIP Archive
```powershell
electron-builder --win --x64 zip
```

---

## 🔐 Code Signing (Optional)

**Note**: Current build is unsigned for development purposes.

To add code signing:

1. **Obtain a code signing certificate**
   - Purchase from certificate authority (Digicert, Sectigo, etc.)
   - Or use free certificate for testing

2. **Configure signing in package.json**
```json
"win": {
  "certificateFile": "path/to/certificate.pfx",
  "certificatePassword": "your-password",
  "signAndEditExecutable": true
}
```

3. **Environment variables** (more secure)
```powershell
$env:CSC_LINK="path/to/certificate.pfx"
$env:CSC_KEY_PASSWORD="your-password"
npm run build
```

---

## 📊 Build Performance

### Build Time
- **First Build**: ~2-3 minutes (downloads Electron binaries)
- **Subsequent Builds**: ~30-60 seconds (uses cache)

### Build Size
- **Installer**: ~71 MB
- **Portable**: ~71 MB
- **Unpacked**: ~165 MB

### Cache Location
```
C:\Users\<username>\AppData\Local\electron-builder\Cache\
```

---

## 🐛 Troubleshooting

### Issue: Build fails with "Cannot create symbolic link"
**Solution**: This was resolved by disabling code signing
```json
"signAndEditExecutable": false
```

### Issue: "electron-builder not found"
**Solution**: Install dependencies
```powershell
npm install
```

### Issue: Out of disk space
**Solution**: Clean cache
```powershell
Remove-Item -Recurse -Force "$env:LOCALAPPDATA\electron-builder\Cache"
npm run build
```

### Issue: Build is slow
**Solution**: Use directory build for testing
```powershell
npm run build:dir
```

### Issue: Antivirus blocks build
**Solution**: Add project folder to antivirus exclusions temporarily

---

## 🧹 Cleaning Build Files

### Remove build output
```powershell
Remove-Item -Recurse -Force dist
```

### Remove cache
```powershell
Remove-Item -Recurse -Force "$env:LOCALAPPDATA\electron-builder\Cache"
```

### Full clean rebuild
```powershell
Remove-Item -Recurse -Force dist, node_modules
npm install
npm run build
```

---

## 📝 Build Scripts Reference

| Command | Description |
|---------|-------------|
| `npm run build` | Build installer + portable (x64) |
| `npm run build:dir` | Build unpacked directory only |
| `npm run dist` | Build all configured targets |

---

## 🎨 Customizing the Build

### Change App Name
Edit `package.json`:
```json
"build": {
  "productName": "Your App Name"
}
```

### Change App ID
```json
"build": {
  "appId": "com.yourcompany.yourapp"
}
```

### Add Custom Icon
1. Create icon file: `build/icon.ico`
2. Update `package.json`:
```json
"win": {
  "icon": "build/icon.ico"
}
```

### Exclude Files from Build
```json
"build": {
  "files": [
    "main.js",
    "preload.js",
    "renderer.js",
    "index.html",
    "styles.css"
  ]
}
```

---

## 📦 Distribution Checklist

Before distributing to end users:

- [ ] Test the installer on clean Windows machine
- [ ] Test the portable version
- [ ] Verify all features work in built version
- [ ] Check application starts correctly
- [ ] Verify shortcuts are created
- [ ] Test uninstaller (for installer version)
- [ ] Consider code signing for production
- [ ] Create release notes
- [ ] Upload to distribution platform

---

## 🌐 Auto-Updates (Optional)

To enable auto-updates:

1. **Set up update server** (GitHub Releases, custom server, etc.)

2. **Configure in package.json**
```json
"build": {
  "publish": {
    "provider": "github",
    "owner": "your-username",
    "repo": "your-repo"
  }
}
```

3. **Add update check in main.js**
```javascript
const { autoUpdater } = require('electron-updater');
autoUpdater.checkForUpdatesAndNotify();
```

---

## 📞 Build Support

For build issues:
- **Email**: pi.ko@nyu.edu
- **Check**: electron-builder documentation: https://www.electron.build
- **Check**: Electron documentation: https://www.electronjs.org

---

## ✅ Success Indicators

Your build was successful if you see:
```
✓ packaging       platform=win32 arch=x64
✓ building        target=nsis
✓ building        target=portable
```

And both executables exist in the `dist` folder!

---

## 🎉 Build Complete!

**You now have:**
- ✅ Installer version for distribution
- ✅ Portable version for testing
- ✅ Unpacked version for debugging
- ✅ Update metadata files
- ✅ Block map for delta updates

**Ready to distribute!** 🚀

---

*Last Updated: 04 November 2025*

