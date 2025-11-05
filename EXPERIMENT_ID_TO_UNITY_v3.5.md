# Experiment ID Passing to Unity - Version 3.5

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025  
**Version:** v3.5.0

## Overview

Updated the application to pass the Experiment ID to Unity when starting experiments. Unity now receives the experiment ID in the command message for internal tracking and experiment management. Also added validation requiring that experiment IDs must end with a number.

---

## Key Changes

### 1. **Experiment ID Validation**
- New requirement: Experiment ID must end with a number
- Prevents starting experiments with invalid IDs
- Example valid IDs: `Trial1`, `Subject_001`, `Test123`
- Example invalid IDs: `Trial`, `Subject_A`, `TestData`

### 2. **Experiment ID Sent to Unity**
- Left hand command: `CMD:START_LEFT_EXPERIMENT:experimentId`
- Right hand command: `CMD:START_RIGHT_EXPERIMENT:experimentId`
- Unity receives the raw experiment ID (not the prefixed filename)

### 3. **Updated Message Format**
- **Before:** `CMD:START_LEFT_EXPERIMENT`
- **After:** `CMD:START_LEFT_EXPERIMENT:Trial1`

---

## Message Format Details

### Command Structure:
```
CMD:{COMMAND}:{EXPERIMENT_ID}
```

### Examples:

| User Input | Button | Message Sent to Unity |
|-----------|--------|----------------------|
| `Trial1` | Left Hand | `CMD:START_LEFT_EXPERIMENT:Trial1` |
| `Trial1` | Right Hand | `CMD:START_RIGHT_EXPERIMENT:Trial1` |
| `Subject_001` | Left Hand | `CMD:START_LEFT_EXPERIMENT:Subject_001` |
| `Subject_001` | Right Hand | `CMD:START_RIGHT_EXPERIMENT:Subject_001` |

### File vs Unity ID:

| User Input | Filename | Unity Receives |
|-----------|----------|----------------|
| `Trial1` (Left) | `LEFT_Trial1.csv` | `Trial1` |
| `Trial1` (Right) | `RIGHT_Trial1.csv` | `Trial1` |

**Note:** Unity receives the original experiment ID, NOT the prefixed filename.

---

## Validation Rules

### Valid Experiment IDs:
✅ `Trial1` - Ends with number  
✅ `Subject_001` - Ends with number  
✅ `Test123` - Ends with number  
✅ `Exp_5` - Ends with number  

### Invalid Experiment IDs:
❌ `Trial` - Does not end with number  
❌ `Subject_A` - Ends with letter  
❌ `TestData` - Ends with letters  
❌ `Exp_` - Ends with underscore  

### Error Message:
```
"Experiment ID must end with a number"
```

---

## Implementation Details

### renderer.js Changes

**Added Validation:**
```javascript
// Validate that experiment ID ends with a number
if (!/\d$/.test(experimentId)) {
    addLog('Experiment ID must end with a number', 'error');
    elements.experimentId.focus();
    return;
}
```

**Pass ID to Command:**
```javascript
// Send appropriate command based on hand WITH experiment ID
const command = hand === 'left' ? 'startLeftExperiment' : 'startRightExperiment';
const result = await window.api[command](experimentId);  // Pass the experiment ID
```

### preload.js Changes

**Updated Function Signatures:**
```javascript
startLeftExperiment: (experimentId) => ipcRenderer.invoke('start-left-experiment', experimentId),
startRightExperiment: (experimentId) => ipcRenderer.invoke('start-right-experiment', experimentId),
```

### main.js Changes

**Updated IPC Handlers:**
```javascript
ipcMain.handle('start-left-experiment', async (event, experimentId) => {
    // Send START_LEFT_EXPERIMENT command with experiment ID to Unity
    const commandMsg = `${MSG_COMMAND}:${CMD_START_LEFT_EXPERIMENT}:${experimentId}`;
    dataServer.send(
        Buffer.from(commandMsg),
        UNITY_DATA_PORT,
        unityEndpoint.address
    );
    
    return { success: true };
});

ipcMain.handle('start-right-experiment', async (event, experimentId) => {
    // Send START_RIGHT_EXPERIMENT command with experiment ID to Unity
    const commandMsg = `${MSG_COMMAND}:${CMD_START_RIGHT_EXPERIMENT}:${experimentId}`;
    dataServer.send(
        Buffer.from(commandMsg),
        UNITY_DATA_PORT,
        unityEndpoint.address
    );
    
    return { success: true };
});
```

---

## Unity Integration

### C# Message Parsing:

```csharp
void OnDataReceived(string message) {
    if (message.StartsWith("CMD:")) {
        string[] parts = message.Split(':');
        string command = parts[1];
        string experimentId = parts.Length > 2 ? parts[2] : "";
        
        if (command == "START_LEFT_EXPERIMENT") {
            StartLeftHandExperiment(experimentId);
        }
        else if (command == "START_RIGHT_EXPERIMENT") {
            StartRightHandExperiment(experimentId);
        }
        else if (command == "STOP_EXPERIMENT") {
            StopExperiment();
        }
    }
}

void StartLeftHandExperiment(string experimentId) {
    Debug.Log($"Starting left hand experiment: {experimentId}");
    currentExperimentId = experimentId;
    currentHand = Hand.Left;
    // Configure for left hand
    StartDataStreaming();
}

void StartRightHandExperiment(string experimentId) {
    Debug.Log($"Starting right hand experiment: {experimentId}");
    currentExperimentId = experimentId;
    currentHand = Hand.Right;
    // Configure for right hand
    StartDataStreaming();
}
```

---

## User Workflow

### Complete Flow:

1. **Enter Experiment ID:**
   - User types: `Trial1`
   - System validates: ✅ Ends with number

2. **Click Start Left Hand:**
   - Filename created: `LEFT_Trial1.csv`
   - Message sent: `CMD:START_LEFT_EXPERIMENT:Trial1`
   - Unity receives: `Trial1`
   - Unity logs: "Starting left hand experiment: Trial1"

3. **Data Streaming:**
   - Unity streams data to Electron
   - Data saved to: `ExperimentalData/LEFT_Trial1.csv`
   - Unity tracks internally as experiment: `Trial1`

4. **Click Stop Experiment:**
   - Message sent: `CMD:STOP_EXPERIMENT`
   - Recording stops
   - File saved

---

## Error Scenarios

### Scenario 1: Invalid ID Format
```
User Input: "Trial"
Validation: ❌ Does not end with number
Action: Show error, focus input
Message: "Experiment ID must end with a number"
Result: Experiment does not start
```

### Scenario 2: Empty ID
```
User Input: ""
Validation: ❌ Empty
Action: Show error, focus input
Message: "Please enter an Experiment ID"
Result: Experiment does not start
```

### Scenario 3: Valid ID
```
User Input: "Trial1"
Validation: ✅ Ends with number
Action: Start experiment
Message: "CMD:START_LEFT_EXPERIMENT:Trial1"
Result: Experiment starts successfully
```

---

## Benefits

1. **Unity Tracking:** Unity can internally track experiment ID
2. **Validation:** Ensures consistent ID format across experiments
3. **Organization:** Unity can organize data by experiment ID
4. **Debugging:** Easier to debug with experiment IDs in Unity logs
5. **Flexibility:** Unity can implement custom logic per experiment ID

---

## Use Cases in Unity

### 1. **Experiment Configuration:**
```csharp
void StartLeftHandExperiment(string experimentId) {
    // Load specific configuration based on ID
    if (experimentId.StartsWith("Pilot")) {
        LoadPilotConfiguration();
    } else if (experimentId.StartsWith("Main")) {
        LoadMainConfiguration();
    }
}
```

### 2. **Data Tagging:**
```csharp
void SendDataPoint(Vector3 position) {
    string dataLine = $"{currentExperimentId},{currentHand},{position.x},{position.y},{position.z}";
    SendToElectron(dataLine);
}
```

### 3. **Session Management:**
```csharp
void StartExperiment(string experimentId) {
    sessionStartTime = Time.time;
    sessionId = experimentId;
    LogEvent($"Session {sessionId} started at {DateTime.Now}");
}
```

---

## Testing Checklist

- [x] Validation works for IDs ending with numbers
- [x] Validation rejects IDs not ending with numbers
- [x] Experiment ID passed to Unity correctly
- [x] Message format correct: `CMD:COMMAND:ID`
- [x] Left and right hand both work
- [x] Error message displays correctly
- [x] Focus returns to input on error
- [x] No linter errors

---

## Files Modified

1. **main.js** - v3.5
   - Updated IPC handlers to accept experimentId parameter
   - Modified command messages to include experiment ID
   - Enhanced logging to show experiment ID

2. **renderer.js** - v3.5
   - Added validation for experiment ID format
   - Modified startExperiment() to pass ID to command
   - Updated log messages to include experiment ID

3. **preload.js** - v3.5
   - Updated function signatures to accept experimentId
   - Added parameter documentation

4. **index.html** - v3.5
   - Updated version to 3.5.0
   - Updated changelog

5. **package.json** - v3.5.0
   - Updated version number

---

## Comparison: Before vs After

### v3.4 (Before):
```javascript
// User enters: "Trial1"
// Message sent: "CMD:START_LEFT_EXPERIMENT"
// Unity receives: No experiment ID
```

### v3.5 (After):
```javascript
// User enters: "Trial1"
// Validation: Must end with number ✓
// Message sent: "CMD:START_LEFT_EXPERIMENT:Trial1"
// Unity receives: experimentId = "Trial1"
```

---

## Version History

| Version | Date | Change |
|---------|------|--------|
| v3.5 | 05 Nov 2025 | Pass experiment ID to Unity, add validation |
| v3.4 | 05 Nov 2025 | Added left/right hand experiment support |
| v3.3 | 05 Nov 2025 | Integrated recording with experiments |
| v3.2 | 05 Nov 2025 | Added folder opening button |

---

## Next Steps for Unity

1. **Parse Experiment ID:**
   - Extract from command message: `parts[2]`
   - Store in variable for session tracking

2. **Implement ID-Based Logic:**
   - Configure experiments based on ID
   - Tag data with experiment ID
   - Log experiment start/stop with ID

3. **Optional Enhancements:**
   - Validate ID format on Unity side
   - Send confirmation back to Electron with ID
   - Store experiment metadata with ID

---

**Status:** ✅ Complete and Tested  
**Backward Compatible:** Yes (Unity can ignore ID if not needed)  
**Ready for Integration:** Yes

