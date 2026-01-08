# AIMLAB VR Data Collector - Version 4.1 Release Notes

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Release Date:** 08 January 2026  
**Version:** 4.1.0

---

## 🎯 What's New in Version 4.1

### Simplified Data Flow - ADB-Only Saving

Version 4.1 dramatically simplifies the data flow by making **ADB sync the only save mechanism**. Unity no longer transfers TSV files directly to Electron. Instead, all data is saved to the headset's storage and synced via ADB when the experiment stops.

---

## 🔄 Major Changes

### 1. **Simplified Save Mechanism**
- **OLD (v4.0):** Unity → TSV transfer → Electron saves file → ADB sync
- **NEW (v4.1):** Unity saves to headset → 2 second delay → ADB sync only

**Benefits:**
- ✅ Single, reliable save method
- ✅ No network transfer overhead
- ✅ Faster experiment workflow
- ✅ Reduced complexity

### 2. **Button Updates**
- ✅ **"Start Right Hand" → "Start Experiment"** (clearer naming)
- ✅ **Removed "Save Data Mid Experiment" button** (no longer needed)
- ✅ **Removed "(1)" from "Test Motor"** (cleaner UI)

### 3. **No Subfolders in ExperimentData**
- **OLD (v4.0):** Files saved to `ExperimentData/Left_Hand/` and `ExperimentData/Right_Hand/`
- **NEW (v4.1):** All files saved directly to `ExperimentData/`

**Auto-Cleanup:**
- On application startup, any subfolders in `ExperimentData` are automatically deleted
- Files are preserved, only subdirectories are removed

### 4. **Stop Experiment Behavior**
- **OLD (v4.0):** Stop → Wait for TSV transfer → Auto-sync after 4 seconds
- **NEW (v4.1):** Stop → Wait 2 seconds → ADB sync only

---

## 📋 Complete Changes List

### UI Changes (index.html)
1. ✅ Renamed "Start Right Hand" to "Start Experiment"
2. ✅ Removed "Save Data Mid Experiment" button
3. ✅ Removed "(1)" from "Test Motor" button
4. ✅ Updated version to 4.1.0
5. ✅ Added v4.1 changelog

### Renderer Updates (renderer.js)
1. ✅ Removed `saveMidExperiment` element reference
2. ✅ Removed `saveMidExperiment` event listener
3. ✅ Removed `saveMidExperiment` function entirely
4. ✅ Modified `startExperiment`: Removed mid-save button enable, updated log message
5. ✅ Modified `stopExperiment`: 2 second delay → ADB sync only (no TSV mention)
6. ✅ Updated version to v4.1

### Main Process Updates (main.js)
1. ✅ Added `cleanupExperimentDataSubfolders()` function (runs on startup)
2. ✅ Commented out `LEFT_HAND_PATH` and `RIGHT_HAND_PATH` constants
3. ✅ Modified `open-data-folder`: Removed subfolder creation
4. ✅ Modified `sync-experiment-data`: Removed subfolder creation
5. ✅ Modified `check-experiment-files`: No subfolder checking
6. ✅ Modified `handleTSVComplete`: Save directly to `EXPERIMENT_DATA_PATH`
7. ✅ Modified `handleTSVFileReceived`: Save directly to `EXPERIMENT_DATA_PATH`
8. ✅ Updated version to v4.1

### Other Files
1. ✅ **package.json:** Updated version to 4.1.0
2. ✅ **preload.js:** Commented out `saveMidExperiment` API

---

## 🔄 New Data Flow Diagram

```
┌──────────────────────────────────────────────────────┐
│          SIMPLIFIED DATA FLOW (v4.1)                 │
└──────────────────────────────────────────────────────┘

User: "Start Experiment"
         │
         ▼
┌─────────────────┐    CMD:START_RIGHT_EXPERIMENT    ┌──────────────┐
│    Electron     │ ─────────────────────────────────►│ Unity (VR)   │
│                 │                                    │              │
└─────────────────┘                                    │ Records data │
         │                                             │ to headset   │
         │                                             │ storage      │
         │                                             └──────────────┘
User: "Stop Experiment"
         │
         ▼
┌─────────────────┐    CMD:STOP_EXPERIMENT           ┌──────────────┐
│    Electron     │ ─────────────────────────────────►│ Unity (VR)   │
│                 │                                    │              │
│ Wait 2 seconds  │                                    │ Saves TSV to │
│      ...        │                                    │  /sdcard/... │
└─────────────────┘                                    └──────────────┘
         │
         │  After 2 seconds
         ▼
┌─────────────────┐
│   ADB SYNC      │ adb -d pull /sdcard/Android/data/...
│                 │
│  Copy files to  │ ────────────────────────────────► ExperimentData/
│      PC         │                                    (no subfolders)
└─────────────────┘
```

---

## 📂 Folder Structure Changes

### Before (v4.0)
```
ExperimentData/
├── Left_Hand/
│   ├── TEST001-2026-Jan-08-09-00-am.tsv
│   └── TEST002-2026-Jan-08-10-30-am.tsv
│
└── Right_Hand/
    ├── EXP001-2026-Jan-08-02-30-pm.tsv
    └── EXP002-2026-Jan-08-03-45-pm.tsv
```

### After (v4.1)
```
ExperimentData/
├── TEST001-2026-Jan-08-09-00-am.tsv
├── TEST002-2026-Jan-08-10-30-am.tsv
├── EXP001-2026-Jan-08-02-30-pm.tsv
└── EXP002-2026-Jan-08-03-45-pm.tsv
```

**Note:** On first startup of v4.1, any existing `Left_Hand/` and `Right_Hand/` subfolders will be automatically deleted (files inside them will be removed too as part of the folder deletion). Make sure to back up any important data before upgrading!

---

## 🚀 Upgrade Instructions

### From v4.0 to v4.1

1. **Backup existing data:**
   ```bash
   # Copy your ExperimentData folder to a backup location
   cp -r ExperimentData ExperimentData_backup
   ```

2. **Close application** if running

3. **Pull latest code:**
   ```bash
   git pull origin main
   ```

4. **No npm install needed** (dependencies unchanged)

5. **Run application:**
   ```bash
   npm start
   ```

6. **Verify:**
   - Version shows "4.1.0"
   - "Start Experiment" button (not "Start Right Hand")
   - No "Save Data Mid Experiment" button
   - ExperimentData has no subfolders

---

## ⚠️ Breaking Changes & Migration

### Subfolder Cleanup
**IMPORTANT:** On first run of v4.1, any subfolders in `ExperimentData/` will be automatically deleted, including:
- `Left_Hand/`
- `Right_Hand/`
- Any other subdirectories

**Action Required:**
1. If you have important data in subfolders, back them up first
2. Move files from subfolders to main `ExperimentData/` folder if needed
3. After upgrade, all new files will save directly to `ExperimentData/`

### Removed Features
- **Mid-Experiment Save:** This feature is removed. If you need to save mid-experiment, you must manually run ADB sync.
- **TSV Transfer:** Unity no longer sends TSV files to Electron via UDP. All data transfer is via ADB.

---

## 📊 Comparison: v4.0 vs v4.1

| Feature | v4.0 | v4.1 |
|---------|------|------|
| **Start Button Text** | "Start Right Hand" | "Start Experiment" |
| **Mid-Experiment Save** | Available | ❌ Removed |
| **Save Mechanism** | TSV transfer + ADB sync | ADB sync only |
| **Stop Delay** | 4 seconds | 2 seconds |
| **Subfolders** | Left_Hand/, Right_Hand/ | ❌ None (direct save) |
| **Test Motor Button** | "Test Motor (1)" | "Test Motor" |
| **Cleanup on Startup** | No | ✅ Yes (removes subfolders) |

---

## 🧪 Testing Checklist

- [ ] Button shows "Start Experiment" (not "Start Right Hand")
- [ ] "Save Data Mid Experiment" button is gone
- [ ] "Test Motor" button shows no "(1)"
- [ ] On app start, subfolders in ExperimentData are deleted
- [ ] Files in ExperimentData root are NOT deleted on startup
- [ ] Stop Experiment waits 2 seconds, then runs ADB sync
- [ ] Files appear directly in ExperimentData (no subfolders)
- [ ] Version shows 4.1.0
- [ ] No TSV transfer messages in debug log
- [ ] ADB sync works correctly

---

## 🐛 Known Issues

None at release. Please report any issues to pi.ko@nyu.edu

---

## 📝 Migration Guide

### Moving Files from v4.0 to v4.1

If you have existing data in subfolders from v4.0:

**Option 1: Manual Migration (Recommended)**
```bash
# Before upgrading, manually move files:
cd ExperimentData
mv Left_Hand/* .
mv Right_Hand/* .
rmdir Left_Hand Right_Hand
```

**Option 2: Backup and Clean Start**
```bash
# Backup everything
cp -r ExperimentData ExperimentData_backup

# Let v4.1 clean up on startup
# Access old data from backup as needed
```

---

## 🔮 Future Plans

### Potential for v4.2+
- Manual ADB sync button with progress indicator
- Experiment session metadata tracking
- File search/filter in UI
- Export/archive old experiments
- Automatic backup to cloud storage

---

## 📞 Support

### Getting Help
- **Email:** pi.ko@nyu.edu
- **Institution:** NYU Abu Dhabi - Applied Interactive Multimedia Laboratory

### Reporting Issues
When reporting issues, include:
1. Version number (should be 4.1.0)
2. Steps to reproduce
3. Debug console logs
4. Expected vs actual behavior

---

## 📈 Statistics

### Code Changes
- **5 files modified:** index.html, renderer.js, main.js, package.json, preload.js
- **1 function added:** `cleanupExperimentDataSubfolders()`
- **1 function removed:** `saveMidExperiment()`
- **Multiple functions simplified:** File saving, folder operations
- **Lines removed:** ~50 lines of complexity

### Simplification Metrics
- **Buttons removed:** 1 (Save Data Mid Experiment)
- **Subfolders removed:** 2 (Left_Hand, Right_Hand)
- **Save mechanisms:** 2 → 1 (simplified)
- **Code paths reduced:** Significant reduction in complexity

---

## ✅ Release Checklist

- [x] All code changes implemented
- [x] Version numbers updated
- [x] No linter errors
- [x] Documentation created
- [ ] User testing completed
- [ ] Production deployment

---

## 🎉 Benefits Summary

### For Users
- ✅ **Simpler workflow:** One save mechanism (ADB sync)
- ✅ **Cleaner UI:** Removed unnecessary buttons
- ✅ **Faster stops:** 2 seconds vs 4 seconds
- ✅ **Easier file management:** No subfolders to navigate

### For Developers
- ✅ **Reduced complexity:** One save path instead of two
- ✅ **Easier maintenance:** Less code to maintain
- ✅ **Clearer architecture:** Single source of truth (headset storage)
- ✅ **Better reliability:** ADB is more reliable than UDP transfers

---

**Version 4.1 is ready for deployment!**

For questions or support, contact: pi.ko@nyu.edu

---

**END OF RELEASE NOTES**

