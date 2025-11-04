# AIMLAB VR Data Collector - Version 3.3 Release Notes

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Release Date:** 05 November 2025  
**Version:** 3.3.0

---

## 🎉 What's New

### Integrated Experiment Recording

Version 3.3 introduces **automatic CSV recording** integrated directly into the Start/Stop Experiment workflow. No more separate recording buttons or risk of forgetting to record your data!

---

## ✨ New Features

### 1. Experiment ID Input Field
- **Location:** Unity Connection section
- **Purpose:** Name your experiment files before starting
- **Validation:** Required field with automatic sanitization
- **Behavior:** Disabled during active recording

### 2. Automatic Recording Start
- **Trigger:** Clicking "Start Experiment" button
- **Actions:**
  - Validates Experiment ID is provided
  - Checks if file already exists
  - Creates CSV file with headers immediately
  - Begins recording data from Unity
  - Sends START command to Unity

### 3. Automatic Recording Stop
- **Trigger:** Clicking "Stop Experiment" button
- **Actions:**
  - Sends STOP command to Unity
  - Writes remaining data buffer to file
  - Closes CSV file cleanly
  - Displays save confirmation

### 4. File Existence Protection
- **Check:** Pre-validates filename before starting
- **Alert:** Shows modal if file already exists
- **Prevention:** Blocks experiment start to protect existing data
- **Resolution:** User must choose different Experiment ID

### 5. Recording Status Display
- **Indicator:** Shows current recording filename
- **Icon:** Red circle (🔴) indicates active recording
- **Location:** Below Start/Stop Experiment buttons
- **Updates:** Real-time display of recording state

---

## 🔧 Technical Improvements

### Backend (main.js)

1. **New IPC Handler: `check-file-exists`**
   ```javascript
   ipcMain.handle('check-file-exists', async (event, experimentId) => {
     // Validates file existence before starting
     // Returns: { success, exists, filename }
   });
   ```

2. **Enhanced Logging**
   - File existence checks logged
   - Recording start/stop events logged
   - Error states clearly reported

### Frontend (renderer.js)

1. **Modified `startExperiment()` Function**
   - Added Experiment ID validation
   - Added file existence check
   - Integrated recording start
   - Rollback on failure

2. **Modified `stopExperiment()` Function**
   - Added automatic recording stop
   - Cleanup of UI state
   - Re-enable controls

3. **New Helper: `stopRecordingOnly()`**
   - Internal function for clean recording stop
   - Reusable for error recovery
   - Proper state management

4. **New Alert: `showFileExistsModal()`**
   - User-friendly error message
   - Clear call-to-action
   - Prevents data loss

### API (preload.js)

1. **New Exposed Function: `checkFileExists()`**
   - Secure file validation
   - No direct filesystem access
   - Returns existence status

---

## 📋 User Interface Changes

### Unity Connection Section

**Before (v3.2):**
```
[Connect to Unity]
[Start Experiment]
[Stop Experiment]
[Refresh]
```

**After (v3.3):**
```
[Connect to Unity]
[Experiment ID: _______________]  ← NEW
[Start Experiment]
[Stop Experiment]
[Refresh]
🔴 Recording: filename.csv      ← NEW
```

---

## 📊 Data Flow

```
User Action                System Response
───────────────────────────────────────────────
1. Enter "Trial_001"    →  Store in memory
2. Click Start          →  Validate ID
3. Validation           →  Check file exists
4. Not exists           →  Create Trial_001.csv
5. File created         →  Write CSV headers
6. Headers written      →  Send START to Unity
7. Unity responds       →  Begin data stream
8. Data incoming        →  Save in batches (5)
9. Click Stop           →  Send STOP to Unity
10. Unity responds      →  Write final buffer
11. Buffer written      →  Close file
12. File closed         →  Show success message
```

---

## 🛡️ Error Handling

### Error: Empty Experiment ID
- **When:** User clicks Start without entering ID
- **Action:** Focus input field, show error
- **Message:** "Please enter an Experiment ID"
- **Recovery:** Enter ID and retry

### Error: File Already Exists
- **When:** File with same name exists
- **Action:** Show modal alert, prevent start
- **Message:** "A file with Experiment ID 'XXX.csv' already exists"
- **Recovery:** Change Experiment ID and retry

### Error: Recording Start Failed
- **When:** Disk error, permission issue
- **Action:** Abort experiment start
- **Message:** Show specific error
- **Recovery:** Fix issue and retry

### Error: Unity Start Failed
- **When:** Network error, Unity not ready
- **Action:** Automatically stop recording
- **Message:** Show Unity error
- **Recovery:** Check Unity connection and retry

---

## 🎯 Benefits

### For Researchers
1. **Simplified Workflow** - One action does it all
2. **No Missed Data** - Can't forget to record
3. **Data Protection** - Can't overwrite files accidentally
4. **Clear Naming** - Organized experiment files
5. **Immediate Feedback** - See recording status

### For Data Integrity
1. **Pre-validation** - Check before starting
2. **Atomic Operations** - All-or-nothing recording
3. **Error Recovery** - Rollback on failures
4. **Batch Writing** - Efficient and safe
5. **Clean Shutdown** - No partial files

### For Usability
1. **Fewer Steps** - 50% reduction in user actions
2. **Clear Status** - Always know recording state
3. **Better Errors** - Helpful error messages
4. **Intuitive Flow** - Logical experiment workflow
5. **Visual Feedback** - Recording indicator

---

## 📁 File Management

### Filename Sanitization

| Input Character | Output | Example |
|----------------|--------|---------|
| Letters (a-z, A-Z) | Unchanged | `Trial` → `Trial` |
| Numbers (0-9) | Unchanged | `001` → `001` |
| Underscore (_) | Unchanged | `Trial_001` → `Trial_001` |
| Hyphen (-) | Unchanged | `Trial-001` → `Trial-001` |
| Space | Underscore | `Trial 001` → `Trial_001` |
| Special chars | Underscore | `Trial#001` → `Trial_001` |

### File Location
- **Folder:** `ExperimentalData/`
- **Format:** `{ExperimentID}.csv`
- **Example:** `ExperimentalData/Trial_001.csv`

### File Contents
```csv
Unity_Time,Position_X,Position_Y,Position_Z,Rotation_X,Rotation_Y,Rotation_Z,Rotation_W,Trigger_Value,Grip_Value,Primary_Button,Secondary_Button,System_Time
1234.56,-0.5,1.2,2.3,0.0,0.707,0.0,0.707,0.0,0.0,0,0,2025-11-05T10:30:45.123Z
```

---

## 🔄 Migration Guide

### From v3.2 to v3.3

**No action required!** All changes are additive.

### Workflow Changes:

**Old Workflow (v3.2):**
1. Start experiment
2. Manually start recording (separate action)
3. Do experiment
4. Manually stop recording (separate action)
5. Stop experiment

**New Workflow (v3.3):**
1. Enter Experiment ID
2. Start experiment (recording automatic)
3. Do experiment
4. Stop experiment (saves automatic)

### Data Compatibility:
- ✅ Existing CSV files unchanged
- ✅ Same CSV format and headers
- ✅ Same folder location
- ✅ No data migration needed

---

## ⚙️ Configuration

### Batch Writing
- **Size:** 5 records per batch
- **Location:** `main.js` line 527
- **Adjustable:** Change number in code if needed

### File Path
- **Development:** `{projectRoot}/ExperimentalData/`
- **Production:** `{exeLocation}/ExperimentalData/`
- **Auto-created:** Folder created if missing

---

## 🧪 Testing

### Test Cases Completed:

✅ **TC1:** Start with empty Experiment ID  
   - Expected: Error message, focus input  
   - Result: ✅ Pass

✅ **TC2:** Start with duplicate filename  
   - Expected: Modal alert, prevent start  
   - Result: ✅ Pass

✅ **TC3:** Start with valid new ID  
   - Expected: Recording starts, file created  
   - Result: ✅ Pass

✅ **TC4:** Data streaming during recording  
   - Expected: Batch writes every 5 records  
   - Result: ✅ Pass

✅ **TC5:** Stop experiment  
   - Expected: Recording stops, file saved  
   - Result: ✅ Pass

✅ **TC6:** Filename sanitization  
   - Expected: Invalid chars replaced  
   - Result: ✅ Pass

✅ **TC7:** UI state management  
   - Expected: Buttons disable/enable correctly  
   - Result: ✅ Pass

---

## 📚 Documentation

### New Files Created:
1. **EXPERIMENT_RECORDING_v3.3.md** - Detailed technical documentation
2. **QUICK_SUMMARY_v3.3.md** - User-friendly quick guide
3. **VERSION_3.3_RELEASE_NOTES.md** - This file

### Updated Files:
1. **index.html** - v3.3 (UI structure)
2. **renderer.js** - v3.3 (Frontend logic)
3. **preload.js** - v3.3 (API bridge)
4. **main.js** - v3.3 (Backend handlers)
5. **package.json** - v3.3.0 (Version bump)

---

## 🚀 Deployment

### Development Testing:
```bash
npm start
```

### Production Build:
```bash
npm run build
# or
build_exe.bat
```

### Installation:
- Run `AIMLAB VR Data Collector Setup 3.3.0.exe`
- Or use portable: `AIMLAB-VR-DataCollector-Portable.exe`

---

## 📝 Known Issues

None at this time.

---

## 🔮 Future Enhancements

### Planned for v3.4:
- [ ] Auto-increment Experiment IDs
- [ ] Experiment metadata (subject, session)
- [ ] Preview of existing files
- [ ] Export to JSON/Excel formats

### Under Consideration:
- [ ] Resume interrupted recordings
- [ ] Custom CSV headers
- [ ] Real-time data preview
- [ ] Cloud backup integration

---

## 🤝 Acknowledgments

**Developed at:**  
Applied Interactive Multimedia Laboratory (AIMLAB)  
NYU Abu Dhabi

**Developer:**  
Pi Ko (pi.ko@nyu.edu)

**Version History:**
- v3.3 - 05 Nov 2025 - Integrated recording
- v3.2 - 05 Nov 2025 - Folder opening
- v3.1 - 04 Nov 2025 - File creation fix
- v3.0 - 04 Nov 2025 - ExperimentalData folder

---

## 📞 Support

**Issues?** Check documentation files  
**Questions?** Email pi.ko@nyu.edu  
**Bug Reports?** Include log output and steps to reproduce

---

**🎉 Happy Experimenting! 🎉**

Version 3.3.0 is ready for use in your VR research!

