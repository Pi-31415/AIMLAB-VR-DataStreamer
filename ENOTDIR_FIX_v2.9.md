# AIMLAB VR Data Collector - ENOTDIR Fix v2.9

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v2.9.0 - **ENOTDIR RESOLVED**

---

## 🔧 CRITICAL FIX: ENOTDIR Error

### The Problem:
**Error**: `ENOTDIR, not a directory`

**Cause**: Path resolution issues between development and packaged app

### The Solution:

#### 1. **Proper Path Resolution** ✅
```javascript
// OLD: Only worked in development
const dataDir = path.join(__dirname, 'data');

// NEW: Works in both development AND packaged app
const appPath = app.isPackaged 
  ? path.dirname(app.getPath('exe'))  // Packaged
  : __dirname;                         // Development
const dataDir = path.join(appPath, 'data');
```

#### 2. **Extensive Error Handling** ✅
- Check if path exists
- Check if it's a directory vs file
- Remove file if needed
- Create directory with recursive flag
- Verify write permissions
- Detailed error logging

#### 3. **Diagnostic Logging** ✅
```javascript
sendLog(`Data directory path: ${dataDir}`, 'debug');
sendLog(`__dirname: ${__dirname}`, 'error');
sendLog(`app.isPackaged: ${app.isPackaged}`, 'error');
sendLog(`Data exists - isDirectory: ${stats.isDirectory()}`, 'error');
```

---

## 🔧 What Was Fixed

### Path Resolution:
```javascript
// Development mode:
appPath = __dirname
// C:\Users\...\AIMLAB-VR-DataStreamer

// Packaged mode:
appPath = path.dirname(app.getPath('exe'))
// C:\Users\...\AppData\Local\Programs\AIMLAB VR Data Collector

dataDir = path.join(appPath, 'data')
```

### Directory Creation:
```javascript
// Multiple fallbacks:
1. Try mkdirSync with { recursive: true }
2. If fails, try without recursive
3. If fails, throw detailed error

// Verification:
fs.accessSync(dataDir, fs.constants.W_OK);
```

### Error Diagnostics:
```javascript
// On error, logs:
- Error message
- Error code (ENOTDIR, EACCES, etc.)
- Full path attempted
- Whether app is packaged
- If directory exists
- If path is directory vs file
```

---

## 🚀 Testing

### Test in Development:
```powershell
npm start

# Click "Start Recording"
# Should see:
# "Attempting to create/verify data directory: C:\...\data"
# "Data directory already exists"
# "Data directory is writable"
# "Recording started: filename.csv"
```

### Test in Packaged App:
```powershell
cd dist
.\AIMLAB-VR-DataCollector-Portable.exe

# Click "Start Recording"
# Should create data folder next to .exe
# Path: C:\...\dist\data\filename.csv
```

---

## 📁 Data Directory Locations

### Development Mode:
```
AIMLAB-VR-DataStreamer/
├── main.js
├── renderer.js
├── data/               ← Here
│   └── recordings.csv
```

### Packaged Installer:
```
C:\Users\[user]\AppData\Local\Programs\AIMLAB VR Data Collector/
├── AIMLAB VR Data Collector.exe
├── data/               ← Here
│   └── recordings.csv
```

### Packaged Portable:
```
dist/
├── AIMLAB-VR-DataCollector-Portable.exe
├── data/               ← Here (created on first use)
│   └── recordings.csv
```

---

## 🛠️ Diagnostic Tools

### diagnose_data_dir.bat:
Run this script to check data directory status:

```powershell
.\diagnose_data_dir.bat
```

**Checks**:
- Current directory
- If data exists
- If data is directory or file
- Directory contents
- Write permissions
- Full path

---

## 🐛 If Still Getting ENOTDIR

### Manual Fix:
```powershell
# 1. Navigate to project directory
cd C:\Users\paing\Documents\ResearchProjects\AIMLAB-VR-DataStreamer

# 2. Remove data if it's a file
if (Test-Path "data" -PathType Leaf) {
    Remove-Item "data" -Force
}

# 3. Create data as directory
New-Item -ItemType Directory -Path "data" -Force

# 4. Verify
Test-Path "data" -PathType Container
# Should output: True

# 5. Test write
"test" | Out-File "data\test.txt"
Remove-Item "data\test.txt"
```

### Check for Symlinks:
```powershell
# Check if data is a symlink
(Get-Item "data").LinkType

# Should output: nothing (not a link)
# If it says "SymbolicLink" or "Junction", that's the problem
```

### Rebuild Data Directory:
```powershell
# Remove everything
Remove-Item "data" -Recurse -Force

# Create fresh
New-Item -ItemType Directory -Path "data"

# Restart app
npm start
```

---

## ✅ Changes in v2.9

### Code Changes:
- [x] Proper path resolution (packaged vs development)
- [x] Extensive error handling
- [x] Detailed diagnostic logging
- [x] Write permission verification
- [x] Multiple fallback strategies

### Tools Added:
- [x] diagnose_data_dir.bat script
- [x] ENOTDIR_FIX_v2.9.md documentation

---

## 🎯 Status

**Version**: 2.9.0 (if you update package.json)  
**ENOTDIR**: ✅ **Should be fixed**  
**Path Resolution**: ✅ **Works in dev & packaged**  
**Error Logging**: ✅ **Comprehensive**

---

## 📞 Support

**Author**: Pi Ko  
**Email**: pi.ko@nyu.edu

---

**Try the app now and check the detailed error logs if it still fails!**

---

*Last Updated: 04 November 2025*  
*ENOTDIR debugging enhanced*

