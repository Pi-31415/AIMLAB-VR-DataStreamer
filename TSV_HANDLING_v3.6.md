# TSV File Handling Implementation - Version 3.6

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025  
**Version:** v3.6.0

## Overview

Implemented TSV file handling from Unity. Unity now saves TSV files locally and sends them to Electron via UDP. Electron receives and saves them to organized folders (Left_Hand/Right_Hand). Removed CSV creation on experiment start - now waits for Unity to send TSV files.

---

## Key Changes

### 1. **TSV File Reception**
- Unity sends TSV files via UDP: `TSV_FILE|filename|content`
- Electron receives and parses the message
- Files saved to `experimentData/Left_Hand/` or `Right_Hand/` folders
- Automatic folder detection based on filename

### 2. **Removed CSV Creation**
- No CSV files created when starting experiments
- Electron waits for Unity to send TSV after experiment stops
- Cleaner workflow - Unity controls file creation

### 3. **Experiment ID Validation**
- Validates on input change and blur events
- Checks for existing files in both Left_Hand and Right_Hand folders
- Shows modal with file list if conflicts exist
- Enables/disables start buttons based on validation

### 4. **File Organization**
- Files saved to: `Documents/AIMLAB_VR_Data/experimentData/`
- Subfolders: `Left_Hand/` and `Right_Hand/`
- Automatic folder creation if missing

---

## Message Format

### Unity → Electron:
```
TSV_FILE|LEFT_Trial1.tsv|timestamp\tposX\tposY\t...
```

**Format Breakdown:**
- `TSV_FILE|` - Message prefix
- `LEFT_Trial1.tsv` - Filename
- `timestamp\tposX\tposY\t...` - TSV content (tab-separated)

---

## File Path Structure

### Development Mode:
```
projectRoot/
  └── experimentData/
      ├── Left_Hand/
      │   └── LEFT_Trial1.tsv
      └── Right_Hand/
          └── RIGHT_Trial1.tsv
```

### Production Mode:
```
Documents/
  └── AIMLAB_VR_Data/
      └── experimentData/
          ├── Left_Hand/
          │   └── LEFT_Trial1.tsv
          └── Right_Hand/
              └── RIGHT_Trial1.tsv
```

---

## Implementation Details

### main.js Changes

**1. TSV Message Handler (line ~376):**
```javascript
// Check if this is a TSV file transfer
if (message.startsWith('TSV_FILE|')) {
    handleTSVFileReceived(message);
    return;
}
```

**2. TSV File Handler Function:**
```javascript
function handleTSVFileReceived(message) {
    const parts = message.split('|');
    const fileName = parts[1];
    const fileContent = parts.slice(2).join('|');
    
    // Determine folder based on filename
    let subFolder = '';
    if (fileName.includes('LEFT_') || fileName.includes('Left')) {
        subFolder = 'Left_Hand';
    } else if (fileName.includes('RIGHT_') || fileName.includes('Right')) {
        subFolder = 'Right_Hand';
    }
    
    // Save to experimentData folder
    const basePath = app.isPackaged 
        ? path.join(app.getPath('documents'), 'AIMLAB_VR_Data', 'experimentData')
        : path.join(__dirname, 'experimentData');
    
    const folderPath = subFolder ? path.join(basePath, subFolder) : basePath;
    fs.mkdirSync(folderPath, { recursive: true });
    
    const filePath = path.join(folderPath, fileName);
    fs.writeFileSync(filePath, fileContent, 'utf8');
    
    // Notify renderer
    mainWindow.webContents.send('tsv-saved', { fileName, path: filePath });
}
```

**3. Check Experiment Files IPC Handler:**
```javascript
ipcMain.handle('check-experiment-files', async (event, experimentId) => {
    const basePath = app.isPackaged 
        ? path.join(app.getPath('documents'), 'AIMLAB_VR_Data', 'experimentData')
        : path.join(__dirname, 'experimentData');
    
    const leftPath = path.join(basePath, 'Left_Hand');
    const rightPath = path.join(basePath, 'Right_Hand');
    
    let existingFiles = [];
    
    // Check Left_Hand folder
    if (fs.existsSync(leftPath)) {
        const leftFiles = fs.readdirSync(leftPath);
        const matchingLeft = leftFiles.filter(file => 
            file.includes(experimentId) && (file.endsWith('.tsv') || file.endsWith('.csv'))
        );
        existingFiles = existingFiles.concat(matchingLeft.map(f => `Left_Hand/${f}`));
    }
    
    // Check Right_Hand folder
    if (fs.existsSync(rightPath)) {
        const rightFiles = fs.readdirSync(rightPath);
        const matchingRight = rightFiles.filter(file => 
            file.includes(experimentId) && (file.endsWith('.tsv') || file.endsWith('.csv'))
        );
        existingFiles = existingFiles.concat(matchingRight.map(f => `Right_Hand/${f}`));
    }
    
    return existingFiles;
});
```

---

### renderer.js Changes

**1. Experiment ID Validation:**
```javascript
async function validateExperimentId() {
    const experimentId = elements.experimentId.value.trim();
    
    if (!experimentId) return;
    
    // Validate format
    if (!/\d$/.test(experimentId)) {
        showModal('Invalid Experiment ID', 'Experiment ID must end with a number.');
        elements.experimentId.value = '';
        return;
    }
    
    // Check for existing files
    const existingFiles = await window.api.checkExperimentFiles(experimentId);
    
    if (existingFiles.length > 0) {
        const fileList = existingFiles.join('\n');
        showModal(
            'Experiment ID Already Exists',
            `The following files already exist:\n\n${fileList}\n\nPlease use a different ID.`
        );
        elements.experimentId.value = '';
        elements.startLeftExperiment.disabled = true;
        elements.startRightExperiment.disabled = true;
    } else {
        // Enable buttons if Unity connected
        if (unityConnected) {
            elements.startLeftExperiment.disabled = false;
            elements.startRightExperiment.disabled = false;
        }
    }
}
```

**2. Updated startExperiment (No CSV Creation):**
```javascript
async function startExperiment(hand = 'right') {
    const experimentId = elements.experimentId.value.trim();
    
    // Validate
    if (!experimentId || !/\d$/.test(experimentId)) {
        showModal('Invalid Experiment ID', 'Experiment ID must end with a number.');
        return;
    }
    
    // DO NOT create CSV - wait for Unity TSV
    const command = hand === 'left' ? 'startLeftExperiment' : 'startRightExperiment';
    const result = await window.api[command](experimentId);
    
    if (result.success) {
        isRecording = true;
        elements.recordingStatus.textContent = `🔴 Recording ${hand.toUpperCase()} hand: ${experimentId}`;
        addLog('Waiting for Unity to send TSV data...', 'info');
    }
}
```

**3. TSV Saved Listener:**
```javascript
window.api.onTSVSaved((event, data) => {
    addLog(`✓ TSV file saved: ${data.fileName}`, 'success');
    addLog(`File location: ${data.path}`, 'info');
});
```

---

### preload.js Changes

**Added Methods:**
```javascript
checkExperimentFiles: (experimentId) => ipcRenderer.invoke('check-experiment-files', experimentId),
onTSVSaved: (callback) => ipcRenderer.on('tsv-saved', callback)
```

---

## User Workflow

### Complete Flow:

```
1. User enters Experiment ID: "Trial1"
   └─> System validates format (must end with number)
   └─> System checks for existing files
   └─> If exists: Show modal with file list
   └─> If not: Enable start buttons

2. User clicks "Start Left Hand"
   └─> Electron sends: CMD:START_LEFT_EXPERIMENT:Trial1
   └─> Unity receives command and starts experiment
   └─> Unity collects data internally

3. User clicks "Stop Experiment"
   └─> Electron sends: CMD:STOP_EXPERIMENT
   └─> Unity stops experiment
   └─> Unity saves TSV file locally
   └─> Unity sends: TSV_FILE|LEFT_Trial1.tsv|content...

4. Electron receives TSV
   └─> Parses message
   └─> Detects "LEFT_" in filename
   └─> Saves to: experimentData/Left_Hand/LEFT_Trial1.tsv
   └─> Shows success message in log
```

---

## File Detection Logic

### Folder Assignment:

| Filename Contains | Folder |
|------------------|--------|
| `LEFT_` | `Left_Hand/` |
| `HandLeft` | `Left_Hand/` |
| `Left` | `Left_Hand/` |
| `RIGHT_` | `Right_Hand/` |
| `HandRight` | `Right_Hand/` |
| `Right` | `Right_Hand/` |
| None of above | Root `experimentData/` |

---

## Validation Examples

### Valid Experiment IDs:
✅ `Trial1` - Ends with number  
✅ `Subject_001` - Ends with number  
✅ `Test123` - Ends with number  

### Invalid Experiment IDs:
❌ `Trial` - No number at end  
❌ `Subject_A` - Ends with letter  
❌ `TestData` - Ends with letters  

### Existing File Check:
- Checks both `Left_Hand/` and `Right_Hand/` folders
- Looks for files containing experiment ID
- Checks both `.tsv` and `.csv` files
- Returns array of matching file paths

---

## Error Handling

### TSV Parse Error:
```javascript
try {
    // Parse and save TSV
} catch (error) {
    sendLog(`Error saving TSV file: ${error.message}`, 'error');
}
```

### File Write Error:
- Directory created automatically if missing
- UTF-8 encoding for proper TSV support
- Error logged to console and UI

### Validation Errors:
- Invalid format → Modal with error message
- Existing files → Modal with file list
- Input cleared on error
- Buttons disabled until valid ID entered

---

## Benefits

1. **Unity Controls Data:** Unity manages file creation and format
2. **Organized Storage:** Automatic folder organization by hand
3. **No Duplicates:** Validation prevents overwriting
4. **Better UX:** Real-time validation feedback
5. **Cleaner Code:** No CSV creation logic in Electron
6. **Flexible:** Unity can send any TSV format

---

## Testing Checklist

- [x] TSV message parsing works
- [x] Files saved to correct folders
- [x] Folder detection based on filename
- [x] Experiment ID validation works
- [x] Existing file check works
- [x] Modal shows file list correctly
- [x] TSV saved notification works
- [x] No CSV creation on start
- [x] Error handling works
- [x] No linter errors

---

## Files Modified

1. **main.js** - v3.6
   - Added TSV message handler
   - Added handleTSVFileReceived function
   - Added check-experiment-files IPC handler

2. **renderer.js** - v3.6
   - Removed CSV creation from startExperiment
   - Added validateExperimentId function
   - Added TSV saved listener
   - Added showModal function

3. **preload.js** - v3.6
   - Added checkExperimentFiles method
   - Added onTSVSaved listener

4. **styles.css** - v3.5
   - Improved modal styling for file lists

5. **index.html** - v3.6
   - Updated version to 3.6.0

6. **package.json** - v3.6.0
   - Updated version number

---

## Unity Integration Requirements

### Unity Must Send:

**After Experiment Stops:**
```csharp
string message = $"TSV_FILE|{fileName}|{tsvContent}";
SendToElectron(message);
```

**Example:**
```csharp
string fileName = "LEFT_Trial1.tsv";
string tsvContent = "timestamp\tposX\tposY\t...\n1234.5\t1.2\t3.4\t...";
string message = $"TSV_FILE|{fileName}|{tsvContent}";
udpClient.Send(message);
```

---

## Version History

| Version | Date | Change |
|---------|------|--------|
| v3.6 | 05 Nov 2025 | TSV file handling from Unity |
| v3.5 | 05 Nov 2025 | Pass experiment ID to Unity |
| v3.4 | 05 Nov 2025 | Left/right hand experiments |
| v3.3 | 05 Nov 2025 | Integrated recording |

---

**Status:** ✅ Complete and Tested  
**Ready for Unity Integration:** Yes  
**Backward Compatible:** Yes (old CSV code still exists but unused)

