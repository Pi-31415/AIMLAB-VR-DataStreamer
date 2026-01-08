# AIMLAB VR Data Collector - Version 4.2 Implementation Summary

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 08 January 2026  
**Version:** 4.2.0

---

## ✅ Implementation Complete!

All changes for version 4.2 have been successfully implemented with **NO LINTER ERRORS**.

---

## 🎯 Key Changes

### **1. ADB-Only Saving**
- ✅ TSV transfers from Unity NO LONGER save files locally
- ✅ TSV transfers now TRIGGER ADB sync instead
- ✅ All data saving exclusively through ADB sync

### **2. Duplicate File Handling**
- ✅ ADB sync pulls to temporary folder first (`temp_sync/`)
- ✅ Files checked for duplicates before moving to `ExperimentData/`
- ✅ Duplicate files automatically renamed with timestamp
- ✅ Format: `[filename]-YYYY-Mon-DD-HH-MM-am/pm.extension`

### **3. Enhanced ADB Sync Process**
- ✅ Pulls to `temp_sync/` folder first
- ✅ Checks each file for duplicates
- ✅ Renames new files if duplicate exists
- ✅ Moves files to `ExperimentData/`
- ✅ Cleans up `temp_sync/` folder

---

## 📋 Files Modified

| File | Changes | Status |
|------|---------|--------|
| **main.js** | • Updated version to v4.2<br>• Replaced ADB sync with temp folder logic<br>• Added `moveFilesWithDuplicateHandling()`<br>• Added `moveFileWithDuplicateCheck()`<br>• Modified `handleTSVComplete()` to trigger ADB sync<br>• Modified `handleTSVFileReceived()` to trigger ADB sync | ✅ Complete |
| **renderer.js** | • Updated version to v4.2<br>• Modified TSV saved message handler<br>• Changed to show "Synced via ADB" | ✅ Complete |
| **package.json** | • Updated version to 4.2.0 | ✅ Complete |
| **index.html** | • Updated version to 4.2.0<br>• Added v4.2 changelog | ✅ Complete |

---

## 🔄 New Data Flow (v4.2)

```
┌──────────────────────────────────────────────────────────────┐
│                   TWO TRIGGERS FOR ADB SYNC                  │
└──────────────────────────────────────────────────────────────┘

Trigger 1: Stop Experiment (User clicks)
    │
    ├─ Send CMD:STOP_EXPERIMENT to Unity
    ├─ Wait 2 seconds
    └─ Trigger ADB Sync
        │
        ▼

Trigger 2: TSV Transfer Complete (Unity sends)
    │
    ├─ Receive TSV_COMPLETE message
    ├─ DO NOT save TSV data locally ❌
    ├─ Wait 2 seconds
    └─ Trigger ADB Sync
        │
        ▼

┌──────────────────────────────────────────────────────────────┐
│                      ADB SYNC PROCESS                        │
├──────────────────────────────────────────────────────────────┤
│ 1. Create temp_sync/ folder                                  │
│ 2. adb -d pull /sdcard/.../HandMovement/. → temp_sync/       │
│ 3. For each file in temp_sync/:                              │
│    • Check if file exists in ExperimentData/                 │
│    • If NO duplicate: Copy as-is                             │
│    • If DUPLICATE: Append timestamp to NEW file              │
│      Example: data.tsv → data-2026-Jan-08-12-30-pm.tsv       │
│ 4. Clean up temp_sync/ folder                                │
└──────────────────────────────────────────────────────────────┘
            │
            ▼
┌──────────────────────────────────────────────────────────────┐
│              ExperimentData/ (flat structure)                │
├──────────────────────────────────────────────────────────────┤
│ • experiment1.tsv                                            │
│ • experiment1-2026-Jan-08-12-30-pm.tsv  ← duplicate renamed  │
│ • experiment2.tsv                                            │
│ • experiment2-2026-Jan-08-01-45-pm.tsv  ← duplicate renamed  │
└──────────────────────────────────────────────────────────────┘
```

---

## 📂 Duplicate File Naming Examples

| Scenario | Original File | Already Exists? | Final Filename |
|----------|--------------|-----------------|----------------|
| First save | `EXP001.tsv` | No | `EXP001.tsv` |
| Second save | `EXP001.tsv` | Yes | `EXP001-2026-Jan-08-12-30-pm.tsv` |
| Third save | `EXP001.tsv` | Yes (2 exist) | `EXP001-2026-Jan-08-12-31-pm.tsv` |
| Same minute | `EXP001.tsv` | Yes | `EXP001-2026-Jan-08-12-30-pm-45.tsv` |

---

## 🔧 Technical Implementation Details

### **New Helper Functions**

#### 1. `moveFilesWithDuplicateHandling(sourceDir, destDir)`
- Recursively processes all files from temp folder
- Flattens subdirectories (saves all files to root)
- Calls `moveFileWithDuplicateCheck()` for each file
- Returns count of moved files

#### 2. `moveFileWithDuplicateCheck(sourceFile, destDir, fileName)`
- Checks if file already exists in destination
- If exists: Appends timestamp to NEW filename
- If still exists (rare): Adds seconds
- Copies file to destination
- Logs duplicate detection

### **Modified Functions**

#### 1. `sync-experiment-data` IPC Handler
**OLD (v4.1):**
```javascript
adb pull → EXPERIMENT_DATA_PATH directly
```

**NEW (v4.2):**
```javascript
adb pull → temp_sync/
→ moveFilesWithDuplicateHandling()
→ Clean up temp_sync/
```

#### 2. `handleTSVComplete(message)`
**OLD (v4.1):**
```javascript
Reassemble TSV chunks → Save to ExperimentData → Notify renderer
```

**NEW (v4.2):**
```javascript
Receive TSV complete → Log message → Trigger ADB sync after 2s
```

#### 3. `handleTSVFileReceived(message)`
**OLD (v4.1):**
```javascript
Parse TSV content → Save to ExperimentData → Notify renderer
```

**NEW (v4.2):**
```javascript
Receive TSV file → Log message → Trigger ADB sync after 2s
```

---

## ⚙️ Configuration

### Paths Used
- **APP_PATH:** Application root directory
- **EXPERIMENT_DATA_PATH:** `APP_PATH/ExperimentData/`
- **temp_sync:** `APP_PATH/temp_sync/` (created/deleted automatically)

### ADB Source Path
```
/sdcard/Android/data/com.AimLab.NHPT/files/HandMovement/.
```

---

## ✅ Quality Checks

- ✅ No linter errors in any file
- ✅ All functions properly defined
- ✅ Error handling in place
- ✅ Cleanup logic for temp folder
- ✅ Logging for all operations
- ✅ Version numbers updated consistently

---

## 🧪 Testing Checklist

### Basic Functionality
- [ ] Application starts without errors
- [ ] Version shows "4.2.0"
- [ ] Connect to Unity works
- [ ] Start Experiment works

### ADB Sync on Stop
- [ ] Stop Experiment waits 2 seconds
- [ ] ADB sync triggers automatically
- [ ] Files appear in ExperimentData/
- [ ] No temp_sync folder remains

### TSV Transfer Trigger
- [ ] Unity sends TSV data
- [ ] No local TSV file created
- [ ] ADB sync triggers after 2 seconds
- [ ] Success message shows "Synced via ADB"

### Duplicate Handling
- [ ] First file: `experiment.tsv`
- [ ] Second file: `experiment-2026-Jan-08-12-30-pm.tsv`
- [ ] Both files exist in ExperimentData/
- [ ] Original file NOT overwritten
- [ ] New file has timestamp appended

### Error Cases
- [ ] ADB not connected → Error message shown
- [ ] Headset not connected → Error message shown
- [ ] temp_sync cleanup happens even on error

---

## 🎯 Benefits of v4.2

### For Users
- ✅ **No data loss:** Duplicates automatically handled
- ✅ **Clear history:** All saves preserved with timestamps
- ✅ **Reliable saving:** One consistent method (ADB)
- ✅ **No confusion:** TSV transfers don't create local files

### For System
- ✅ **Single source of truth:** Headset storage
- ✅ **Conflict prevention:** Automatic timestamp appending
- ✅ **Clean implementation:** Temp folder prevents overwrites
- ✅ **Better logging:** Clear messages about what's happening

---

## 📊 Comparison: v4.1 vs v4.2

| Feature | v4.1 | v4.2 |
|---------|------|------|
| **TSV Transfer Action** | ❌ Save locally | ✅ Trigger ADB sync |
| **Stop Experiment Action** | ✅ ADB sync | ✅ ADB sync |
| **Duplicate Handling** | ❌ None | ✅ Automatic timestamp |
| **Save Location** | Direct to ExperimentData | Via temp_sync first |
| **Overwrite Protection** | ❌ None | ✅ Full protection |
| **Temp Folder** | ❌ Not used | ✅ temp_sync/ |

---

## 🐛 Known Limitations

### Current Limitations
1. **Timestamp Resolution:** Minute-level by default (adds seconds if needed)
2. **Temp Folder:** Created in app root (requires write permissions)
3. **ADB Dependency:** Requires ADB to be configured and working
4. **No Progress Bar:** User sees "Syncing..." but no percentage

### Not Issues (By Design)
- ✅ TSV data is ignored (not saved locally)
- ✅ Only NEW files get timestamp (original preserved)
- ✅ temp_sync folder is temporary (cleaned up automatically)

---

## 🔮 Future Enhancements (v4.3+)

### Potential Improvements
1. **Progress Indicator:** Show file copy progress
2. **Batch Operations:** Handle multiple files more efficiently
3. **Smart Timestamps:** Only add if truly needed
4. **Compression:** Compress old files automatically
5. **Cloud Backup:** Auto-backup to cloud storage

---

## 📝 Migration Notes

### From v4.1 to v4.2
- ✅ **No data migration needed**
- ✅ **Fully backward compatible**
- ✅ **Existing files unchanged**
- ✅ **Just update and run**

### Important Notes
- First run creates temp_sync folder (automatically)
- TSV transfers will now trigger ADB sync
- Duplicate files get timestamped (not overwritten)
- All existing files remain untouched

---

## 📞 Support

### Getting Help
- **Email:** pi.ko@nyu.edu
- **Institution:** NYU Abu Dhabi - AIMLAB

### Reporting Issues
Include:
1. Version number (4.2.0)
2. Debug console logs
3. Steps to reproduce
4. Expected vs actual behavior

---

## 🎉 Release Status

**Version 4.2.0 is READY for testing and deployment!**

All code changes implemented ✅  
No linter errors ✅  
Documentation complete ✅  
Ready for user testing ✅

---

**END OF SUMMARY**

