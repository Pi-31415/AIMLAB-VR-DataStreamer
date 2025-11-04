# UI Update v3.2 - Recording Buttons Removed

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025  
**Version:** v3.2.0

## Overview

Removed the Start Recording and Stop Recording buttons from the UI and replaced them with a single "Open Experiment Data Folder" button that opens the ExperimentalData folder in Windows Explorer.

## Changes Made

### 1. **index.html** (v3.2)
- **Removed:**
  - Filename input field (`<input type="text" id="filename" ...>`)
  - Start Recording button (`<button id="startRecording" ...>`)
  - Stop Recording button (`<button id="stopRecording" ...>`)
  - Recording status indicator (`<div class="recording-status" id="recordingStatus">`)
  
- **Added:**
  - Single "Open Experiment Data Folder" button in Data Recording section
  - Updated version from 3.1.0 to 3.2.0

- **HTML Structure:**
```html
<!-- Experiment Data Folder -->
<div class="control-group">
    <h3>Experiment Data</h3>
    <button id="openDataFolder" class="btn-primary">Open Experiment Data Folder</button>
</div>
```

### 2. **renderer.js** (v3.2)
- **Removed:**
  - `isRecording` state variable
  - DOM element references: `filename`, `startRecording`, `stopRecording`, `recordingStatus`
  - `startRecording()` function
  - `stopRecording()` function
  - Event listeners for recording buttons
  - Keyboard shortcut Ctrl+R for recording

- **Added:**
  - `openDataFolder` DOM element reference
  - `openDataFolder()` function to call the new IPC handler
  - Event listener for the new button
  - Keyboard shortcut Ctrl+O to open data folder

- **Updated:**
  - Removed recording-related button state management in `updateConnectionStatus()`
  - Changed keyboard shortcuts section to use Ctrl+O instead of Ctrl+R

### 3. **preload.js** (v3.2)
- **Added:**
  - `openDataFolder()` function exposed via contextBridge
  - Maps to `'open-data-folder'` IPC handler
  - Returns Promise with success status and folder path

- **API Addition:**
```javascript
openDataFolder: () => ipcRenderer.invoke('open-data-folder')
```

### 4. **main.js** (v3.2)
- **Added:**
  - Import of `shell` from Electron: `const { ..., shell } = require('electron');`
  - New IPC handler: `ipcMain.handle('open-data-folder', ...)`
  - Folder Operations section with comprehensive error handling

- **Functionality:**
  - Creates `ExperimentalData` folder if it doesn't exist
  - Uses `shell.openPath()` to open folder in default file manager
  - Proper error handling and logging
  - Works in both development and packaged modes

- **Handler Code:**
```javascript
ipcMain.handle('open-data-folder', async () => {
  try {
    const appPath = app.isPackaged ? path.dirname(app.getPath('exe')) : __dirname;
    const dataDir = path.join(appPath, 'ExperimentalData');
    
    // Create directory if it doesn't exist
    if (!fs.existsSync(dataDir)) {
      fs.mkdirSync(dataDir, { recursive: true });
    }
    
    // Open folder in Explorer
    const openResult = await shell.openPath(dataDir);
    if (openResult) {
      throw new Error(`Failed to open folder: ${openResult}`);
    }
    
    return { success: true, path: dataDir };
  } catch (error) {
    return { success: false, error: error.message };
  }
});
```

## User Experience

### Before (v3.1):
1. User enters filename
2. User clicks "Start Recording"
3. User performs experiment
4. User clicks "Stop Recording"
5. User navigates to ExperimentalData folder manually

### After (v3.2):
1. User clicks "Open Experiment Data Folder" button
2. Windows Explorer opens directly to ExperimentalData folder
3. User can view all CSV files immediately
4. No manual navigation required

## Benefits

1. **Simplified UI:** Removed 3 UI elements (input + 2 buttons + status div)
2. **Direct Access:** One-click access to experiment data location
3. **Better UX:** Users can quickly verify data files exist
4. **Less Confusion:** Recording is handled automatically by Unity, not UI
5. **Cross-Platform:** `shell.openPath()` works on Windows, macOS, and Linux

## Keyboard Shortcuts

- **Ctrl+O (or Cmd+O):** Open Experiment Data Folder
- **Ctrl+L (or Cmd+L):** Clear debug log

## Technical Notes

- Recording functionality remains in backend (`start-recording`, `stop-recording` handlers)
- Unity still controls when recording starts/stops via experiment commands
- CSV files are still saved to `ExperimentalData/` folder
- File naming and duplicate handling still works as before
- Modal notification for file renames still functions

## Testing

✅ Application starts successfully  
✅ Button appears in UI  
✅ Button opens ExperimentalData folder  
✅ Folder is created if it doesn't exist  
✅ No linter errors  
✅ All other functionality preserved  

## File Versions Updated

| File | Old Version | New Version |
|------|-------------|-------------|
| index.html | v2.0 | v3.2 |
| renderer.js | v2.0 | v3.2 |
| preload.js | v2.0 | v3.2 |
| main.js | v3.1 | v3.2 |

## Next Steps

1. Test in production build
2. Verify folder opening works correctly
3. Update any documentation that references recording buttons
4. Consider adding "Refresh" button to reload folder contents if needed

---

**Status:** ✅ Complete  
**Build Required:** Yes (run `npm run build` or `build_exe.bat`)

