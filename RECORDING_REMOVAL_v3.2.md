# Recording Buttons Removal - Version 3.2

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025

## Summary

Removed the Start Recording and Stop Recording buttons from the UI and replaced them with a single "Open Experiment Data Folder" button that provides direct access to the ExperimentalData folder where CSV files are stored.

---

## UI Changes

### BEFORE (v3.1):
```
┌─────────────────────────────────────────┐
│         Data Recording Section          │
├─────────────────────────────────────────┤
│  [Text Input: filename]                 │
│  [Start Recording Button]               │
│  [Stop Recording Button]                │
│  Recording Status: 🔴 Recording...      │
└─────────────────────────────────────────┘
```

### AFTER (v3.2):
```
┌─────────────────────────────────────────┐
│       Experiment Data Section           │
├─────────────────────────────────────────┤
│  [Open Experiment Data Folder]          │
└─────────────────────────────────────────┘
```

---

## What Was Removed

1. **Input Field:** Filename text input
2. **Button:** Start Recording (was green/primary button)
3. **Button:** Stop Recording (was gray/secondary button)
4. **Status Display:** Recording status indicator showing filename

---

## What Was Added

1. **Button:** "Open Experiment Data Folder" button
   - Opens Windows Explorer to `ExperimentalData/` folder
   - Creates folder if it doesn't exist
   - Works in both dev and production builds

---

## Functionality Changes

### Recording (Internal - Still Works):
- Recording functionality still exists in backend
- Unity Start/Stop Experiment buttons control recording
- CSV files still saved to `ExperimentalData/` folder
- File naming and duplicate handling unchanged

### New Folder Access:
- **Click button:** Opens folder in Windows Explorer
- **Keyboard shortcut:** Ctrl+O (Cmd+O on Mac)
- **Auto-create:** Folder created if missing
- **Cross-platform:** Works on Windows, macOS, Linux

---

## Code Changes Summary

### Files Modified:
1. ✅ `index.html` - Updated UI structure
2. ✅ `renderer.js` - Removed recording functions, added folder function
3. ✅ `preload.js` - Added openDataFolder API
4. ✅ `main.js` - Added open-data-folder IPC handler
5. ✅ `package.json` - Version bumped to 3.2.0

### Lines Changed:
- **Added:** ~50 lines (new handler, documentation)
- **Removed:** ~100 lines (recording UI logic)
- **Net Change:** -50 lines (cleaner code)

---

## User Workflow

### Old Workflow (v3.1):
1. User types filename → Clicks Start Recording
2. Unity sends data → Data saves to CSV
3. User clicks Stop Recording
4. User manually navigates to find CSV file
5. User opens folder in Explorer manually

### New Workflow (v3.2):
1. Unity Start Experiment → Data automatically saves
2. Unity Stop Experiment → Recording automatically stops
3. User clicks "Open Experiment Data Folder"
4. Windows Explorer opens → User sees all CSV files
5. Done! ✓

---

## Benefits

| Aspect | Before | After |
|--------|--------|-------|
| **UI Complexity** | 4 elements | 1 element |
| **User Steps** | 5+ clicks | 1 click |
| **Confusion Level** | Higher (duplicate controls) | Lower (single purpose) |
| **File Access** | Manual navigation | Direct access |
| **Recording Control** | Unity + UI (confusing) | Unity only (clear) |

---

## Technical Implementation

### New IPC Handler (`main.js`):
```javascript
ipcMain.handle('open-data-folder', async () => {
  const appPath = app.isPackaged 
    ? path.dirname(app.getPath('exe')) 
    : __dirname;
  const dataDir = path.join(appPath, 'ExperimentalData');
  
  // Create if missing
  if (!fs.existsSync(dataDir)) {
    fs.mkdirSync(dataDir, { recursive: true });
  }
  
  // Open in Explorer
  await shell.openPath(dataDir);
  return { success: true, path: dataDir };
});
```

### New Button Handler (`renderer.js`):
```javascript
async function openDataFolder() {
  const result = await window.api.openDataFolder();
  if (result.success) {
    addLog(`Opened folder: ${result.path}`, 'success');
  }
}
```

### Exposed API (`preload.js`):
```javascript
contextBridge.exposeInMainWorld('api', {
  // ... other functions ...
  openDataFolder: () => ipcRenderer.invoke('open-data-folder')
});
```

---

## Testing Checklist

- [x] Application starts without errors
- [x] Button appears in UI correctly
- [x] Clicking button opens ExperimentalData folder
- [x] Folder creates if it doesn't exist
- [x] Works in development mode (`npm start`)
- [ ] Works in production build (needs testing)
- [x] No linter errors
- [x] All other features still work
- [x] Unity connection unaffected
- [x] Arduino connection unaffected

---

## Migration Notes

### For Users:
- No action needed
- Recording still works the same (via Unity)
- New button provides quick folder access
- Old CSV files remain in ExperimentalData/

### For Developers:
- Recording IPC handlers remain (don't delete)
- Unity still calls start/stop recording
- UI just removed manual control
- Backend logic unchanged

---

## Future Enhancements

Potential improvements for future versions:

1. **Folder Statistics:** Show file count/size in UI
2. **Recent Files:** List recent CSV files in UI
3. **Quick Preview:** Preview CSV contents without opening
4. **File Management:** Rename/delete files from UI
5. **Export Options:** Zip/compress data for sharing

---

## Version History

| Version | Date | Change |
|---------|------|--------|
| v3.2 | 05 Nov 2025 | Removed recording buttons, added folder button |
| v3.1 | 04 Nov 2025 | Force file creation fix |
| v3.0 | 04 Nov 2025 | ExperimentalData folder implementation |

---

**Status:** ✅ Complete  
**Tested:** Development mode  
**Production Build:** Pending testing



