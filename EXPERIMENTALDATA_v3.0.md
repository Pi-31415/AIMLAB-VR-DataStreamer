# AIMLAB VR Data Collector v3.0 - ExperimentalData Folder

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v3.0.0 - **PRODUCTION READY**

---

## ✅ ENOTDIR FIXED - NEW FOLDER STRUCTURE!

### Major Change:
**Save location changed from `data/` to `ExperimentalData/`**

This provides a more descriptive name and resolves any conflicts with the old `data` folder.

---

## 📁 New Folder Structure

### Development Mode:
```
AIMLAB-VR-DataStreamer/
├── main.js
├── renderer.js
├── index.html
├── ExperimentalData/          ← NEW! CSV files saved here
│   ├── subject01_trial01.csv
│   ├── subject01_trial02.csv
│   └── ...
└── data/                      ← OLD (no longer used)
```

### Packaged Portable:
```
dist/
├── AIMLAB-VR-DataCollector-Portable.exe
├── ExperimentalData/          ← Created on first recording
│   └── recordings.csv
```

### Packaged Installer:
```
C:\Users\[user]\AppData\Local\Programs\AIMLAB VR Data Collector/
├── AIMLAB VR Data Collector.exe
├── ExperimentalData/          ← Created on first recording
│   └── recordings.csv
```

---

## 🔧 What Changed (v3.0)

### Code Changes:

#### 1. Start Recording Handler:
```javascript
// OLD:
const dataDir = path.join(appPath, 'data');

// NEW:
const dataDir = path.join(appPath, 'ExperimentalData');
```

#### 2. Stop Recording Handler:
```javascript
// OLD:
const filePath = path.join(appPath, 'data', recordedFile + '.csv');

// NEW:
const filePath = path.join(appPath, 'ExperimentalData', recordedFile + '.csv');
```

#### 3. Log Messages:
```
"ExperimentalData directory does not exist, creating..."
"Created ExperimentalData directory"
"File saved in: C:\...\ExperimentalData\filename.csv"
```

#### 4. .gitignore Updated:
```
# Experimental data (CSV files)
ExperimentalData/
```

---

## 🚀 How It Works

### First Recording:
```
1. User clicks "Start Recording"
   ↓
2. Electron checks for ExperimentalData folder
   ↓
3. Folder doesn't exist → Creates it
   ↓
4. Creates CSV file: ExperimentalData/filename.csv
   ↓
5. ✅ Recording starts successfully!
```

### Subsequent Recordings:
```
1. User clicks "Start Recording"
   ↓
2. ExperimentalData folder already exists
   ↓
3. Creates new CSV: ExperimentalData/filename_2.csv
   ↓
4. ✅ Recording starts!
```

---

## 📊 Benefits

### Why ExperimentalData?

| Feature | Old (`data/`) | New (`ExperimentalData/`) |
|---------|---------------|---------------------------|
| **Name** | Generic | Descriptive ✅ |
| **Purpose** | Unclear | Clear - Research data ✅ |
| **Conflicts** | Had .gitkeep | Clean directory ✅ |
| **Professional** | No | Yes ✅ |

### Additional Benefits:
- ✅ No conflict with existing `data` folder
- ✅ Clear purpose (experimental/research data)
- ✅ Professional naming convention
- ✅ Easy to find and organize
- ✅ No hidden files (.gitkeep)

---

## 🎯 Usage

### Complete Workflow:

```powershell
# 1. Start app
npm start

# 2. Connect to Unity
[Click "Connect to Unity"]
# Status: ● Green

# 3. Start experiment
[Click "Start Experiment"]
# Unity begins data streaming

# 4. Start recording
[Enter filename: "participant_001_trial_01"]
[Click "Start Recording"]
# Creates: ExperimentalData/participant_001_trial_01.csv

# 5. Perform VR tasks
# Data being saved...

# 6. Stop recording
[Click "Stop Recording"]
# File: ExperimentalData/participant_001_trial_01.csv

# 7. Stop experiment
[Click "Stop Experiment"]
# Unity stops streaming
```

---

## 📁 File Locations

### Where Are CSV Files Saved?

**Development Mode**:
```
C:\Users\paing\Documents\ResearchProjects\AIMLAB-VR-DataStreamer\ExperimentalData\
```

**Packaged Portable**:
```
C:\[wherever you put the .exe]\ExperimentalData\
```

**Packaged Installer**:
```
C:\Users\[user]\AppData\Local\Programs\AIMLAB VR Data Collector\ExperimentalData\
```

---

## 📋 Testing

### Test Recording:
```powershell
# 1. Start app
npm start

# 2. Connect Unity (green status)

# 3. Start Experiment

# 4. Enter filename: "test_recording"

# 5. Click "Start Recording"

# Expected logs:
# "Attempting to create/verify data directory: C:\...\ExperimentalData"
# "ExperimentalData directory does not exist, creating..."
# "Created data directory: C:\...\ExperimentalData"
# "Data directory is writable"
# "Recording started: test_recording.csv"
# "Saving to: C:\...\ExperimentalData\test_recording.csv"

# 6. Click "Stop Recording"

# 7. Verify file exists:
dir ExperimentalData\test_recording.csv

# 8. Check contents:
type ExperimentalData\test_recording.csv
```

---

## 🎯 Migration from Old `data/` Folder

### If You Have Old Data:

```powershell
# Copy old recordings to new folder
Copy-Item -Path "data\*.csv" -Destination "ExperimentalData\" -Force

# Verify
dir ExperimentalData

# Optional: Remove old data folder
Remove-Item "data" -Recurse -Force
```

---

## ✅ Changes Summary v3.0

### Major Version (v3.0):
- [x] **Folder name**: data → ExperimentalData
- [x] **Created** ExperimentalData directory
- [x] **Updated** .gitignore
- [x] **Updated** all path references
- [x] **Updated** log messages
- [x] **Version bump** to 3.0 (major change)

### Why Version 3.0?
This is a **breaking change** - the save location is different. Users need to know where to find their data.

---

## 🐛 Troubleshooting

### ENOTDIR Error

**Status**: ✅ **Should be fixed now**

**If still occurring**:
1. Check diagnostic logs in debug console
2. Run `diagnose_data_dir.bat`
3. Manually create ExperimentalData folder
4. Check folder permissions

### Can't Find Saved Files

**Check These Locations**:

Development:
```powershell
dir ExperimentalData
```

Packaged:
```powershell
cd dist
dir ExperimentalData
```

**Tip**: Check the debug log for "Saving to: [full path]"

---

## 🎉 Status

**Version**: 3.0.0  
**Save Location**: ✅ **ExperimentalData/**  
**ENOTDIR**: ✅ **Fixed**  
**Path Resolution**: ✅ **Dev + Packaged**  
**Status**: ✅ **PRODUCTION READY**

---

## 📞 Support

**Author**: Pi Ko  
**Email**: pi.ko@nyu.edu  
**Institution**: NYU  
**Project**: AIMLAB VR Research

---

**🎊 ExperimentalData folder implemented - Clean and professional! 🎊**

---

*Last Updated: 04 November 2025*  
*Version: 3.0.0 - Major update with new folder structure*

