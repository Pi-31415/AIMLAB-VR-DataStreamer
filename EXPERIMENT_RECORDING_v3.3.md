# Experiment Recording Integration - Version 3.3

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025  
**Version:** v3.3.0

## Overview

Integrated automatic CSV recording with the Start/Stop Experiment workflow. Added an Experiment ID textbox for file naming, and recording now automatically starts when you click "Start Experiment" and stops when you click "Stop Experiment". Includes file existence validation to prevent overwriting existing data.

---

## Key Features

### 1. **Experiment ID Textbox**
- New input field in Unity Connection section
- Required before starting an experiment
- Used as the filename for CSV data
- Invalid characters automatically replaced with underscores

### 2. **Automatic Recording**
- **Start Experiment** → Automatically creates CSV file and begins recording
- **Stop Experiment** → Automatically saves and closes CSV file
- No separate recording buttons needed
- Seamless integration with Unity experiment workflow

### 3. **File Existence Validation**
- Pre-checks if file already exists before starting
- Shows modal alert if duplicate filename detected
- Prevents accidental data overwriting
- User must choose a different Experiment ID if file exists

### 4. **Batch Writing**
- Data written in batches of 5 records (as previously implemented)
- Minimizes disk I/O for better performance
- Final batch written when experiment stops

---

## User Workflow

### Complete Experiment Flow:

```
1. Connect to Unity → Click "Connect to Unity"
2. Enter Experiment ID → Type unique identifier (e.g., "Trial_001")
3. Start Experiment → Click "Start Experiment"
   ├─ System checks if file exists
   ├─ If exists: Shows error modal
   ├─ If not: Creates CSV file with headers
   ├─ Sends START command to Unity
   └─ Unity begins streaming data
4. Data Collection → Data automatically saved in batches
5. Stop Experiment → Click "Stop Experiment"
   ├─ Sends STOP command to Unity
   ├─ Writes remaining data buffer
   ├─ Closes CSV file
   └─ Shows save confirmation
6. View Data → Click "Open Experiment Data Folder"
```

---

## UI Changes

### New Elements:

```
┌─────────────────────────────────────────┐
│       Unity Connection Section          │
├─────────────────────────────────────────┤
│  [Connect to Unity]                     │
│  [Experiment ID Input Field] ←── NEW   │
│  [Start Experiment]                     │
│  [Stop Experiment]                      │
│  [Refresh]                              │
│  🔴 Recording: filename.csv ←── NEW    │
└─────────────────────────────────────────┘
```

### Input Field:
- **Placeholder:** "Enter Experiment ID (required)"
- **Type:** Text input
- **Validation:** Required, sanitized automatically
- **State:** Disabled during recording

### Recording Status:
- Shows: `🔴 Recording: ExperimentID.csv`
- Appears when recording starts
- Disappears when recording stops

---

## Implementation Details

### File Existence Check (`main.js`)

```javascript
ipcMain.handle('check-file-exists', async (event, experimentId) => {
  const appPath = app.isPackaged 
    ? path.dirname(app.getPath('exe')) 
    : __dirname;
  const dataDir = path.join(appPath, 'ExperimentalData');
  
  // Clean filename
  const cleanFilename = experimentId
    .replace(/\.csv$/i, '')
    .replace(/[^a-zA-Z0-9_-]/g, '_');
  const filepath = path.join(dataDir, `${cleanFilename}.csv`);
  
  const exists = fs.existsSync(filepath);
  return { success: true, exists: exists, filename: cleanFilename };
});
```

### Start Experiment with Recording (`renderer.js`)

```javascript
async function startExperiment() {
  const experimentId = elements.experimentId.value.trim();
  
  // Validate
  if (!experimentId) {
    addLog('Please enter an Experiment ID', 'error');
    return;
  }
  
  // Check if file exists
  const checkResult = await window.api.checkFileExists(experimentId);
  if (checkResult.exists) {
    showFileExistsModal(experimentId);
    return;
  }
  
  // Start recording
  const recordResult = await window.api.startRecording(experimentId);
  if (!recordResult.success) {
    addLog(`Failed to start recording: ${recordResult.error}`, 'error');
    return;
  }
  
  // Update UI
  isRecording = true;
  elements.recordingStatus.textContent = `🔴 Recording: ${recordResult.filename}.csv`;
  elements.experimentId.disabled = true;
  
  // Send Unity command
  const result = await window.api.startExperiment();
  if (result.success) {
    elements.startExperiment.disabled = true;
    elements.stopExperiment.disabled = false;
  } else {
    // Rollback recording if Unity fails
    await stopRecordingOnly();
  }
}
```

### Stop Experiment with Recording (`renderer.js`)

```javascript
async function stopExperiment() {
  // Stop Unity
  await window.api.stopExperiment();
  
  // Stop recording
  await stopRecordingOnly();
  
  // Reset UI
  elements.startExperiment.disabled = false;
  elements.stopExperiment.disabled = true;
  elements.experimentId.disabled = false;
}

async function stopRecordingOnly() {
  if (!isRecording) return;
  
  const result = await window.api.stopRecording();
  if (result.success) {
    isRecording = false;
    elements.recordingStatus.textContent = '';
    addLog(`Recording saved: ${result.filename}.csv`, 'success');
  }
}
```

---

## Error Handling

### 1. **Missing Experiment ID**
- **Trigger:** Clicking Start without entering ID
- **Action:** Focus input field, show error log
- **Message:** "Please enter an Experiment ID"

### 2. **File Already Exists**
- **Trigger:** Starting with duplicate Experiment ID
- **Action:** Show modal alert, prevent start
- **Message:** "A file with Experiment ID 'XXX.csv' already exists. Please use a different Experiment ID."

### 3. **Recording Start Failure**
- **Trigger:** Disk error, permission issue
- **Action:** Don't start Unity experiment, show error
- **Rollback:** No file created, can retry

### 4. **Unity Start Failure**
- **Trigger:** Unity not connected, network issue
- **Action:** Automatically stop recording
- **Rollback:** File closed cleanly, can retry

---

## File Naming

### Automatic Sanitization:

| Input | Output | Reason |
|-------|--------|--------|
| `Trial_001` | `Trial_001.csv` | Valid, unchanged |
| `Subject #5` | `Subject__5.csv` | # replaced with _ |
| `Test/Data` | `Test_Data.csv` | / replaced with _ |
| `Exp 2024-11-05` | `Exp_2024-11-05.csv` | Space replaced with _ |
| `data.csv` | `data.csv` | .csv extension handled |

**Allowed Characters:** `a-z`, `A-Z`, `0-9`, `_`, `-`  
**Invalid Characters:** Replaced with underscore `_`

---

## Data Flow Diagram

```
┌─────────────┐
│ Enter ID    │
│ "Trial_001" │
└──────┬──────┘
       │
       ▼
┌─────────────────┐
│ Check File      │
│ Exists?         │
└────┬───────┬────┘
     │       │
    NO      YES
     │       │
     ▼       ▼
┌──────┐  ┌──────────┐
│Start │  │Show Modal│
│Record│  │Alert     │
└──┬───┘  └──────────┘
   │
   ▼
┌────────────────┐
│ Send START to  │
│ Unity          │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Unity Streams  │◄──┐
│ Data           │   │
└───────┬────────┘   │
        │            │
        ▼            │
┌────────────────┐   │
│ Save in Batches│   │
│ (5 records)    │───┘
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ User Clicks    │
│ Stop Experiment│
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Send STOP to   │
│ Unity          │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Write Final    │
│ Buffer         │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ Close File     │
│ ✓ Saved        │
└────────────────┘
```

---

## Code Changes Summary

### Files Modified:

1. **index.html** (v3.3)
   - Added `<input id="experimentId">` in Unity section
   - Added `<div id="recordingStatus">` for status display
   - Updated version to 3.3.0

2. **renderer.js** (v3.3)
   - Added `isRecording` state variable back
   - Added `experimentId` and `recordingStatus` DOM references
   - Modified `startExperiment()` with recording integration
   - Modified `stopExperiment()` with recording integration
   - Added `stopRecordingOnly()` helper function
   - Added `showFileExistsModal()` function

3. **preload.js** (v3.3)
   - Added `checkFileExists()` API function

4. **main.js** (v3.3)
   - Added `check-file-exists` IPC handler
   - Enhanced error logging for file operations

5. **package.json**
   - Updated version to 3.3.0

### Lines Changed:
- **Added:** ~120 lines
- **Modified:** ~40 lines
- **Total:** ~160 lines changed

---

## Testing Checklist

- [x] Experiment ID validation works
- [x] File existence check works
- [x] Modal alert shows for duplicate files
- [x] Recording starts with experiment
- [x] Recording stops with experiment
- [x] Batch writing still functional
- [x] Data saved to correct location
- [x] Recording status displays correctly
- [x] Input field disables during recording
- [x] Unity commands still work
- [ ] Production build testing needed

---

## Comparison: Before vs After

### v3.2 (Before):
```
1. Click Start Experiment
2. Unity starts
3. Separately start recording
4. Separately stop recording
5. Click Stop Experiment
```

### v3.3 (After):
```
1. Enter Experiment ID
2. Click Start Experiment → Recording starts automatically
3. Click Stop Experiment → Recording stops automatically
```

**Steps Reduced:** From 5 actions to 2 actions  
**Error Prevention:** File existence check added  
**User Experience:** Simplified and streamlined

---

## Benefits

1. **Simplified Workflow** - One action starts both experiment and recording
2. **Error Prevention** - Can't forget to start recording
3. **Data Protection** - Can't overwrite existing files
4. **Better Naming** - Experiment ID ensures organized data
5. **Automatic Cleanup** - Recording always stops with experiment
6. **Clear Feedback** - Recording status always visible

---

## Known Limitations

1. **Single Recording:** Can't have multiple simultaneous recordings (by design)
2. **No Pause:** Recording is all-or-nothing with experiment
3. **Fixed Format:** CSV columns are predefined
4. **Batch Size:** Fixed at 5 records (can be modified if needed)

---

## Future Enhancements

Potential improvements for future versions:

1. **Experiment Metadata:** Add subject ID, session number, notes
2. **Auto-increment IDs:** Suggest next available ID
3. **File Preview:** Show existing files before starting
4. **Resume Recording:** Continue from interrupted experiments
5. **Custom Headers:** Allow user-defined CSV columns
6. **Export Options:** Add JSON, Excel formats

---

## Migration Notes

### For Users:
- No data migration needed
- Old CSV files remain accessible
- New workflow is backward compatible
- Can still open data folder as before

### For Developers:
- Recording handlers remain unchanged
- Added file validation layer
- Start/Stop Experiment now trigger recording
- Modal system reused for file alerts

---

## Version History

| Version | Date | Key Feature |
|---------|------|-------------|
| v3.3 | 05 Nov 2025 | Integrated recording with experiments |
| v3.2 | 05 Nov 2025 | Added folder opening button |
| v3.1 | 04 Nov 2025 | Force file creation fix |
| v3.0 | 04 Nov 2025 | ExperimentalData folder |

---

**Status:** ✅ Complete  
**Tested:** Development mode  
**Production Build:** Pending testing  
**Documentation:** Complete

