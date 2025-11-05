# Left/Right Hand Experiment Support - Version 3.4

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025  
**Version:** v3.4.0

## Overview

Added support for separate left and right hand experiments. The single "Start Experiment" button has been split into two buttons: "Start Left Hand" and "Start Right Hand". Each button automatically prefixes the CSV filename with `LEFT_` or `RIGHT_` and sends the appropriate command to Unity.

---

## Key Changes

### 1. **Two Experiment Buttons**
- **Start Left Hand** - Starts left hand experiment
- **Start Right Hand** - Starts right hand experiment
- Both buttons are disabled during recording
- Stop Experiment button works for both

### 2. **Automatic Filename Prefixing**
- Left hand experiments: `LEFT_ExperimentID.csv`
- Right hand experiments: `RIGHT_ExperimentID.csv`
- Example: User enters "Trial_001"
  - Left hand → `LEFT_Trial_001.csv`
  - Right hand → `RIGHT_Trial_001.csv`

### 3. **Separate Unity Commands**
- Left hand sends: `CMD:START_LEFT_EXPERIMENT`
- Right hand sends: `CMD:START_RIGHT_EXPERIMENT`
- Stop sends: `CMD:STOP_EXPERIMENT` (same for both)

---

## User Interface Changes

### Before (v3.3):
```
┌─────────────────────────────────────┐
│  [Experiment ID Input]              │
│  [Start Experiment]                 │
│  [Stop Experiment]                  │
└─────────────────────────────────────┘
```

### After (v3.4):
```
┌─────────────────────────────────────┐
│  [Experiment ID Input]              │
│  [Start Left Hand]                  │
│  [Start Right Hand]                 │
│  [Stop Experiment]                  │
└─────────────────────────────────────┘
```

---

## User Workflow

### Left Hand Experiment:
1. Enter Experiment ID: `Trial_001`
2. Click **"Start Left Hand"**
3. System creates: `LEFT_Trial_001.csv`
4. System sends to Unity: `CMD:START_LEFT_EXPERIMENT`
5. Unity starts left hand experiment
6. Data streams and saves to CSV
7. Click **"Stop Experiment"**
8. File saved: `ExperimentalData/LEFT_Trial_001.csv`

### Right Hand Experiment:
1. Enter Experiment ID: `Trial_001`
2. Click **"Start Right Hand"**
3. System creates: `RIGHT_Trial_001.csv`
4. System sends to Unity: `CMD:START_RIGHT_EXPERIMENT`
5. Unity starts right hand experiment
6. Data streams and saves to CSV
7. Click **"Stop Experiment"**
8. File saved: `ExperimentalData/RIGHT_Trial_001.csv`

---

## Implementation Details

### index.html (v3.4)

```html
<!-- Unity Controls -->
<div class="control-group">
    <h3>Unity Connection</h3>
    <button id="connectUnity" class="btn-primary">Connect to Unity</button>
    <input type="text" id="experimentId" placeholder="Enter Experiment ID (required)" />
    <button id="startLeftExperiment" class="btn-success" disabled>Start Left Hand</button>
    <button id="startRightExperiment" class="btn-success" disabled>Start Right Hand</button>
    <button id="stopExperiment" class="btn-danger" disabled>Stop Experiment</button>
    <button id="refreshUnity" class="btn-secondary">Refresh</button>
    <div class="recording-status" id="recordingStatus"></div>
</div>
```

### renderer.js (v3.4)

**Added Button References:**
```javascript
startLeftExperiment: document.getElementById('startLeftExperiment'),
startRightExperiment: document.getElementById('startRightExperiment'),
```

**Event Listeners:**
```javascript
elements.startLeftExperiment.addEventListener('click', () => startExperiment('left'));
elements.startRightExperiment.addEventListener('click', () => startExperiment('right'));
```

**Modified startExperiment Function:**
```javascript
async function startExperiment(hand = 'right') {
    const experimentId = elements.experimentId.value.trim();
    
    // Validate Experiment ID
    if (!experimentId) {
        addLog('Please enter an Experiment ID', 'error');
        return;
    }
    
    // Modify filename based on hand
    const modifiedId = `${hand.toUpperCase()}_${experimentId}`;
    
    // Check if file already exists
    const checkResult = await window.api.checkFileExists(modifiedId);
    if (checkResult.exists) {
        showFileExistsModal(modifiedId);
        return;
    }
    
    // Start recording
    const recordResult = await window.api.startRecording(modifiedId);
    
    // Send appropriate command
    const command = hand === 'left' ? 'startLeftExperiment' : 'startRightExperiment';
    const result = await window.api[command]();
    
    // Update UI
    elements.startLeftExperiment.disabled = true;
    elements.startRightExperiment.disabled = true;
    elements.stopExperiment.disabled = false;
}
```

### preload.js (v3.4)

**Added API Functions:**
```javascript
startLeftExperiment: () => ipcRenderer.invoke('start-left-experiment'),
startRightExperiment: () => ipcRenderer.invoke('start-right-experiment'),
```

### main.js (v3.4)

**Added Command Constants:**
```javascript
const CMD_START_LEFT_EXPERIMENT = "START_LEFT_EXPERIMENT";
const CMD_START_RIGHT_EXPERIMENT = "START_RIGHT_EXPERIMENT";
const CMD_STOP_EXPERIMENT = "STOP_EXPERIMENT";
```

**Added IPC Handlers:**
```javascript
ipcMain.handle('start-left-experiment', async () => {
    if (!unityConnected || !unityEndpoint || !dataServer) {
        return { success: false, error: 'Unity not connected' };
    }
    
    const commandMsg = `${MSG_COMMAND}:${CMD_START_LEFT_EXPERIMENT}`;
    dataServer.send(
        Buffer.from(commandMsg),
        UNITY_DATA_PORT,
        unityEndpoint.address
    );
    
    return { success: true };
});

ipcMain.handle('start-right-experiment', async () => {
    if (!unityConnected || !unityEndpoint || !dataServer) {
        return { success: false, error: 'Unity not connected' };
    }
    
    const commandMsg = `${MSG_COMMAND}:${CMD_START_RIGHT_EXPERIMENT}`;
    dataServer.send(
        Buffer.from(commandMsg),
        UNITY_DATA_PORT,
        unityEndpoint.address
    );
    
    return { success: true };
});
```

---

## File Naming Examples

| User Input | Button Clicked | Filename |
|-----------|---------------|----------|
| `Trial_001` | Start Left Hand | `LEFT_Trial_001.csv` |
| `Trial_001` | Start Right Hand | `RIGHT_Trial_001.csv` |
| `Subject05` | Start Left Hand | `LEFT_Subject05.csv` |
| `Subject05` | Start Right Hand | `RIGHT_Subject05.csv` |
| `Test #1` | Start Left Hand | `LEFT_Test__1.csv` |
| `Test #1` | Start Right Hand | `RIGHT_Test__1.csv` |

---

## Unity Integration

### Commands Sent to Unity:

1. **Start Left Hand:**
   - Message: `CMD:START_LEFT_EXPERIMENT`
   - Port: 45101 (Unity's data port)
   - Unity should respond by starting left hand experiment

2. **Start Right Hand:**
   - Message: `CMD:START_RIGHT_EXPERIMENT`
   - Port: 45101 (Unity's data port)
   - Unity should respond by starting right hand experiment

3. **Stop Experiment:**
   - Message: `CMD:STOP_EXPERIMENT`
   - Port: 45101 (Unity's data port)
   - Unity should respond by stopping current experiment

### Unity C# Implementation Needed:

```csharp
void OnDataReceived(string message) {
    if (message.StartsWith("CMD:START_LEFT_EXPERIMENT")) {
        StartLeftHandExperiment();
    }
    else if (message.StartsWith("CMD:START_RIGHT_EXPERIMENT")) {
        StartRightHandExperiment();
    }
    else if (message.StartsWith("CMD:STOP_EXPERIMENT")) {
        StopExperiment();
    }
}
```

---

## Benefits

1. **Clear Separation** - Distinct files for left and right hand data
2. **No Manual Naming** - Automatic prefix prevents confusion
3. **Organized Data** - Easy to sort and filter by hand
4. **Same Workflow** - Familiar experiment start/stop process
5. **File Safety** - Each hand type checks for duplicates separately

---

## Error Handling

### Duplicate Filename:
- **Scenario:** `LEFT_Trial_001.csv` already exists
- **Action:** Show modal alert
- **Message:** "A file with Experiment ID 'LEFT_Trial_001.csv' already exists"
- **Resolution:** User must enter different ID or delete existing file

### Unity Not Connected:
- **Scenario:** Clicking start without Unity connection
- **Action:** Show error in log
- **Message:** "Unity must be connected to start experiment"
- **Resolution:** Click "Connect to Unity" first

---

## Testing Checklist

- [x] Left hand button creates `LEFT_` prefix
- [x] Right hand button creates `RIGHT_` prefix
- [x] Both buttons disabled during recording
- [x] Stop button works for both hands
- [x] File existence check works for both
- [x] Unity commands sent correctly
- [x] Filenames sanitized properly
- [x] No linter errors

---

## Files Modified

1. **index.html** - v3.4 (UI structure)
2. **renderer.js** - v3.4 (Frontend logic)
3. **preload.js** - v3.4 (API bridge)
4. **main.js** - v3.4 (Backend handlers)
5. **package.json** - v3.4.0 (Version bump)

---

## Version History

| Version | Date | Change |
|---------|------|--------|
| v3.4 | 05 Nov 2025 | Added left/right hand experiment support |
| v3.3 | 05 Nov 2025 | Integrated recording with experiments |
| v3.2 | 05 Nov 2025 | Added folder opening button |
| v3.1 | 04 Nov 2025 | Force file creation fix |

---

## Next Steps for Unity

To fully integrate this feature, Unity needs to:

1. **Handle New Commands:**
   - Listen for `CMD:START_LEFT_EXPERIMENT`
   - Listen for `CMD:START_RIGHT_EXPERIMENT`
   - Implement separate logic for each hand

2. **Suggested Implementation:**
   ```csharp
   public enum Hand { Left, Right }
   private Hand currentHand;
   
   void StartLeftHandExperiment() {
       currentHand = Hand.Left;
       // Configure for left hand
       StartDataStreaming();
   }
   
   void StartRightHandExperiment() {
       currentHand = Hand.Right;
       // Configure for right hand
       StartDataStreaming();
   }
   ```

3. **Data Streaming:**
   - Same data format for both hands
   - Same port (45102 for Electron)
   - Only difference is internal Unity configuration

---

**Status:** ✅ Complete and Tested  
**Ready for Unity Integration:** Yes  
**Backward Compatible:** Yes (old stop command still works)

