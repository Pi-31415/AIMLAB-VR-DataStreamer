# AIMLAB VR Data Collector v4.0 - Testing Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 08 January 2026  
**Version:** v4.0

---

## Testing Overview

This guide provides step-by-step instructions to verify all changes implemented in version 4.0.

---

## Pre-Testing Checklist

- [ ] Electron application runs without errors (`npm start`)
- [ ] Unity VR application is available for connection testing
- [ ] Arduino vibration motor is available for hardware testing (optional)
- [ ] ExperimentData folder exists and is accessible

---

## Test 1: UI Changes Verification

### Objective
Verify that the specified buttons are hidden from the user interface.

### Steps
1. Launch the application: `npm start`
2. Wait for the splash screen to complete (3 seconds)
3. Observe the "Unity Connection" section

### Expected Results
- ✓ **"Start Left Hand" button is NOT visible**
- ✓ **"Start Right Hand" button IS visible**
- ✓ **"Toggle Transparency for Peg Board" button is NOT visible**
- ✓ All other buttons remain visible (Connect to Unity, Stop Experiment, Refresh)
- ✓ Version displayed as "Version 4.0.0"

### Pass Criteria
All hidden buttons should be invisible while other UI elements remain functional.

---

## Test 2: Timestamp Generation Function

### Objective
Verify the timestamp generation function creates properly formatted timestamps.

### Steps
1. Open the browser DevTools (F12 or Ctrl+Shift+I)
2. Open the application
3. Check the debug console logs for timestamp-related messages

### Expected Results
- Timestamps should follow format: `YYYY-Mon-DD-HH-MM-am/pm`
- Example: `2026-Jan-08-02-30-pm`
- Month should be abbreviated (Jan, Feb, Mar, etc.)
- Hours should be in 12-hour format with am/pm

### Pass Criteria
All generated timestamps match the expected format.

---

## Test 3: File Naming with Timestamps

### Objective
Verify that TSV files are saved with timestamp-based filenames.

### Prerequisites
- Unity VR application connected
- Experiment ID entered

### Steps
1. Launch the application
2. Click "Connect to Unity"
3. Wait for Unity connection confirmation
4. Enter an Experiment ID (e.g., "TEST001")
5. Click "Start Right Hand"
6. Perform some actions in Unity
7. Click "Stop Experiment"
8. Wait for Unity to send TSV file
9. Check the ExperimentData folder

### Expected Results
- ✓ File saved to `ExperimentData/Right_Hand/` folder
- ✓ Filename format: `TEST001-YYYY-Mon-DD-HH-MM-am/pm.tsv`
- ✓ Timestamp reflects current date/time
- ✓ Debug console shows "TSV file saved: [filename with timestamp]"

### Pass Criteria
File is saved with correct timestamp format and no overwrites occurred.

---

## Test 4: No File Overwrite (Multiple Saves)

### Objective
Verify that saving multiple times with the same Experiment ID creates unique files.

### Steps
1. Connect to Unity
2. Enter Experiment ID: "MULTI001"
3. Start Right Hand experiment
4. Stop experiment (wait for TSV save)
5. Immediately start another Right Hand experiment with same ID "MULTI001"
6. Stop experiment again
7. Check ExperimentData/Right_Hand/ folder

### Expected Results
- ✓ Two separate files created:
  - `MULTI001-YYYY-Mon-DD-HH-MM-am/pm.tsv`
  - `MULTI001-YYYY-Mon-DD-HH-MM-am/pm.tsv` (different timestamp)
  
  OR if saved in same minute:
  - `MULTI001-YYYY-Mon-DD-HH-MM-am/pm.tsv`
  - `MULTI001-YYYY-Mon-DD-HH-MM-am/pm-45.tsv` (seconds added)

- ✓ No file was overwritten
- ✓ Both files contain different data

### Pass Criteria
Multiple unique files created, no data loss.

---

## Test 5: Rapid Save Testing (Collision Handling)

### Objective
Test the system's ability to handle rapid saves that might create filename collisions.

### Steps
1. Connect to Unity
2. Enter Experiment ID: "RAPID001"
3. Start and stop experiment quickly (within same minute)
4. Repeat 3-4 times
5. Check the ExperimentData folder

### Expected Results
- ✓ All files have unique names
- ✓ Files may have seconds suffix: `RAPID001-YYYY-Mon-DD-HH-MM-am/pm-SS.tsv`
- ✓ In extreme case, milliseconds suffix: `RAPID001-YYYY-Mon-DD-HH-MM-am/pm-SS-MMM.tsv`
- ✓ No files overwritten

### Pass Criteria
System successfully prevents collisions even with rapid saves.

---

## Test 6: Mid-Experiment Save with Timestamps

### Objective
Verify that mid-experiment saves also use timestamp-based naming.

### Steps
1. Connect to Unity
2. Enter Experiment ID: "MIDSAVE001"
3. Start Right Hand experiment
4. During experiment, click "Save Data Mid Experiment"
5. Wait for save confirmation
6. Continue experiment
7. Click "Save Data Mid Experiment" again
8. Stop experiment
9. Check ExperimentData folder

### Expected Results
- ✓ At least 3 files created (2 mid-saves + 1 final)
- ✓ All files use timestamp naming
- ✓ Files have sequential timestamps
- ✓ No overwrites occurred
- ✓ Auto-sync messages appear in debug console

### Pass Criteria
Mid-experiment saves create unique timestamped files.

---

## Test 7: File Check Handler (No Blocking)

### Objective
Verify that the system no longer blocks experiments based on existing files.

### Steps
1. Manually create a file: `ExperimentData/Right_Hand/BLOCK001.tsv`
2. Launch application
3. Connect to Unity
4. Enter Experiment ID: "BLOCK001"
5. Try to start experiment

### Expected Results
- ✓ NO warning modal appears
- ✓ Experiment starts successfully
- ✓ When saved, creates: `BLOCK001-YYYY-Mon-DD-HH-MM-am/pm.tsv`
- ✓ Original `BLOCK001.tsv` file remains untouched

### Pass Criteria
System allows starting experiment even with matching filename in folder.

---

## Test 8: Left Hand Button Functionality (Hidden but Working)

### Objective
Verify that hidden "Start Left Hand" button still functions if triggered programmatically.

### Steps
1. Open browser DevTools (F12)
2. In Console, type and execute:
   ```javascript
   document.getElementById('startLeftExperiment').click();
   ```
3. Observe behavior

### Expected Results
- ✓ Button click triggers event handler
- ✓ Appropriate error or action occurs (depends on connection state)
- ✓ Console shows the button's functionality is intact

### Pass Criteria
Hidden button still has working event listener (for future use/backward compatibility).

---

## Test 9: Transparency Toggle Functionality (Hidden but Working)

### Objective
Verify that hidden "Toggle Transparency" button still functions if triggered programmatically.

### Steps
1. Connect to Unity first
2. Open browser DevTools (F12)
3. In Console, type and execute:
   ```javascript
   document.getElementById('togglePegBoardTransparency').click();
   ```
4. Check Unity application and debug console

### Expected Results
- ✓ Command sent to Unity
- ✓ Debug console shows: "Toggling peg board transparency..."
- ✓ Unity receives toggle command
- ✓ Functionality is intact even though button is hidden

### Pass Criteria
Hidden button still sends commands to Unity when programmatically triggered.

---

## Test 10: Folder Structure Verification

### Objective
Verify that files are saved to correct subfolders.

### Steps
1. Run experiments with both left and right hand (programmatically trigger left if needed)
2. Check folder structure

### Expected Results
- ✓ `ExperimentData/` folder exists
- ✓ `ExperimentData/Left_Hand/` folder exists
- ✓ `ExperimentData/Right_Hand/` folder exists
- ✓ Right hand files in `Right_Hand/` folder
- ✓ All files use timestamp naming

### Pass Criteria
Folder structure is correct and files are organized properly.

---

## Test 11: Backward Compatibility (Legacy TSV Format)

### Objective
Verify that non-chunked TSV messages still work with new naming system.

### Steps
1. If Unity sends non-chunked TSV messages (legacy format)
2. Verify they are saved with timestamp naming

### Expected Results
- ✓ Legacy format still processed
- ✓ Files saved with timestamp naming
- ✓ No errors in console

### Pass Criteria
System handles both chunked and non-chunked TSV formats with timestamp naming.

---

## Test 12: ADB Sync After Save

### Objective
Verify that ADB sync works correctly after experiments with timestamp-named files.

### Prerequisites
- Android headset connected via ADB
- ADB path configured

### Steps
1. Run an experiment and save
2. Click "Sync Experiment Data to PC"
3. Check if files are pulled from headset

### Expected Results
- ✓ Files synced from headset
- ✓ Files appear in ExperimentData folder
- ✓ Timestamp-based naming preserved
- ✓ No errors during sync

### Pass Criteria
ADB sync works with new file naming system.

---

## Test 13: Debug Console Messages

### Objective
Verify that appropriate log messages appear for file operations.

### Steps
1. Run a complete experiment workflow
2. Monitor debug console

### Expected Results
Console should show:
- ✓ "File check: [ID] - will create timestamped file"
- ✓ "TSV file saved: [ID]-YYYY-Mon-DD-HH-MM-am/pm.tsv"
- ✓ "File location: [full path]"
- ✓ No error messages related to file naming

### Pass Criteria
All log messages are clear and informative.

---

## Test 14: Package.json Version

### Objective
Verify package.json is updated (optional - for releases).

### Steps
1. Open `package.json`
2. Check version number

### Expected Results
- Version should be updated to "4.0.0" (if doing a release)
- OR remain "3.9.0" (if not yet released)

### Note
The package.json version is typically updated when doing official releases. For development, it can remain at the previous version.

---

## Test 15: Application Build Test

### Objective
Verify that the application builds correctly with new code.

### Steps
1. Run build command: `npm run build` (or `build_exe.bat`)
2. Wait for build to complete
3. Run the built executable

### Expected Results
- ✓ Build completes without errors
- ✓ Executable runs successfully
- ✓ All UI changes visible
- ✓ Timestamp naming works in built version

### Pass Criteria
Application builds and runs correctly with all new features.

---

## Regression Testing

### Areas to Test for Regression
1. **Unity Connection**
   - Connect/Disconnect still works
   - Handshake protocol functional
   - Data reception intact

2. **Arduino Motor Control**
   - Connection works
   - Test motor command works
   - Vibration commands from Unity work

3. **Discovery Protocol**
   - Unity discovery works
   - Bidirectional communication functional
   - Port configuration correct

4. **Data Recording**
   - VR data is captured correctly
   - CSV format remains valid (if used)
   - TSV format remains valid

5. **UI Responsiveness**
   - Buttons enable/disable correctly
   - Status indicators update properly
   - Modal dialogs work (if any are shown)

---

## Performance Testing

### File I/O Performance
- Monitor file write times for timestamped files
- Verify no performance degradation with longer filenames
- Check filesystem handles long paths correctly

### Memory Usage
- Monitor memory usage during multiple saves
- Verify no memory leaks with timestamp generation
- Check cleanup of transfer tracking

---

## Edge Cases to Test

### 1. Very Long Experiment IDs
- Test with: "THIS_IS_A_VERY_LONG_EXPERIMENT_ID_FOR_TESTING_PURPOSES_001"
- Verify filename doesn't exceed OS limits

### 2. Special Characters in Experiment ID
- Test with: "TEST@123", "TEST#456", "TEST$789"
- Verify special characters are handled appropriately

### 3. Multiple Rapid Mid-Saves
- Save mid-experiment 5+ times rapidly
- Verify all files are unique
- Check for performance issues

### 4. Save During Connection Loss
- Start experiment
- Disconnect Unity
- Attempt to save
- Verify graceful error handling

### 5. Simultaneous Multi-User (if applicable)
- Multiple instances running
- Same experiment ID
- Verify separate timestamps prevent conflicts

---

## Bug Reporting Template

If issues are found during testing, use this template:

```
**Bug Title:** [Brief description]

**Test Case:** Test #X - [Test name]

**Steps to Reproduce:**
1. 
2. 
3. 

**Expected Behavior:**


**Actual Behavior:**


**Screenshots/Logs:**


**Environment:**
- OS: Windows 10/11
- Electron Version: 
- Node Version: 

**Severity:** Critical / High / Medium / Low

**Additional Notes:**

```

---

## Sign-Off Checklist

After completing all tests, verify:

- [ ] All UI changes are correct and visible
- [ ] Timestamp generation produces correct format
- [ ] File naming uses timestamps consistently
- [ ] No file overwrites occur
- [ ] Multiple saves create unique files
- [ ] Mid-experiment saves work correctly
- [ ] File check no longer blocks experiments
- [ ] Hidden buttons preserve functionality
- [ ] Folder structure is maintained
- [ ] Debug console shows appropriate messages
- [ ] No linter errors
- [ ] No runtime errors
- [ ] Application builds successfully
- [ ] All regression tests pass
- [ ] Performance is acceptable

---

## Testing Status

| Test | Status | Date | Tester | Notes |
|------|--------|------|--------|-------|
| Test 1: UI Changes | ⬜ Not Started | | | |
| Test 2: Timestamps | ⬜ Not Started | | | |
| Test 3: File Naming | ⬜ Not Started | | | |
| Test 4: No Overwrite | ⬜ Not Started | | | |
| Test 5: Rapid Saves | ⬜ Not Started | | | |
| Test 6: Mid-Save | ⬜ Not Started | | | |
| Test 7: No Blocking | ⬜ Not Started | | | |
| Test 8: Left Hand | ⬜ Not Started | | | |
| Test 9: Transparency | ⬜ Not Started | | | |
| Test 10: Folders | ⬜ Not Started | | | |
| Test 11: Legacy TSV | ⬜ Not Started | | | |
| Test 12: ADB Sync | ⬜ Not Started | | | |
| Test 13: Console | ⬜ Not Started | | | |
| Test 14: Version | ⬜ Not Started | | | |
| Test 15: Build | ⬜ Not Started | | | |

**Status Legend:**
- ⬜ Not Started
- 🔄 In Progress
- ✅ Passed
- ❌ Failed
- ⚠️ Blocked

---

**END OF TESTING GUIDE**

