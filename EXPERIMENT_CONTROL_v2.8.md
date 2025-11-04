# AIMLAB VR Data Collector - Experiment Control v2.8

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v2.8.0 - **FULL CONTROL**

---

## 🎉 NEW FEATURES + CRITICAL FIXES!

### v2.8 Additions:

1. ✅ **Start/Stop Experiment Buttons** - Control Unity from Electron
2. ✅ **ENOTDIR Fix** - Proper directory creation with recursive flag
3. ✅ **Smaller Batches** - Save every 5 lines (was 10) for safety
4. ✅ **File Rename Modal** - Notification when file already exists
5. ✅ **Unity Command Protocol** - Send commands to Unity

---

## 🎮 New Experiment Control

### Workflow:
```
1. Connect to Unity (handshake completes)
2. Click "Start Experiment" → Unity begins sending data
3. Enter filename and click "Start Recording"
4. Perform VR actions
5. Click "Stop Recording"
6. Click "Stop Experiment" → Unity stops sending data
```

### Benefits:
- **Remote control** of Unity from Electron
- **Clean data** - only record when experiment is running
- **Better organization** - separate experiments clearly
- **Power control** - start/stop VR data streaming on demand

---

## 🔧 Fixes Implemented

### 1. ENOTDIR Error Fixed ✅

**Problem**: `data/.gitkeep` file prevented directory creation

**Solution**:
```javascript
// Check if 'data' exists and is a file (not directory)
try {
  const stats = fs.statSync(dataDir);
  if (!stats.isDirectory()) {
    // It's a file, remove it and create directory
    fs.unlinkSync(dataDir);
    fs.mkdirSync(dataDir, { recursive: true });
  }
} catch (err) {
  // Doesn't exist, create it
  fs.mkdirSync(dataDir, { recursive: true });
}
```

### 2. Batch Size Reduced ✅

**Before**: Saved every 10 lines

**After**: Saves every 5 lines

**Benefit**: Less data loss if app crashes

```javascript
// Write in smaller batches (every 5 lines for safety)
if (dataBuffer.length >= 5) {
  csvWriter.writeRecords(dataBuffer)
    .then(() => {
      sendLog(`Saved ${dataBuffer.length} data points`, 'debug');
      dataBuffer = [];
    });
}
```

### 3. File Rename Notification ✅

**Feature**: Modal popup when filename already exists

**Shows**:
- Original filename requested
- New filename being used
- List of existing files

```javascript
// Notify UI if file was renamed
if (existingFiles.length > 0) {
  mainWindow.webContents.send('file-renamed', {
    original: baseFilename,
    renamed: finalFilename,
    existing: existingFiles
  });
}
```

---

## 🎯 Unity Command Protocol

### Commands Sent to Unity:

| Command | Message Format | Purpose |
|---------|----------------|---------|
| **START_EXPERIMENT** | `CMD:START_EXPERIMENT` | Start Unity data streaming |
| **STOP_EXPERIMENT** | `CMD:STOP_EXPERIMENT` | Stop Unity data streaming |

### Unity Response:
```
Unity receives: "CMD:START_EXPERIMENT"
Unity responds: "CMD:Experiment started"
Unity starts: Sending DATA: messages
```

---

## 📱 Updated UI

### Unity Control Panel:
```
Unity Connection
├── Connect to Unity (Purple)
├── Start Experiment (Green) [disabled until connected]
├── Stop Experiment (Red) [disabled until connected]
└── Refresh (Gray)
```

### Button States:

| Button | When Enabled | Color |
|--------|-------------|-------|
| Connect to Unity | Always | Purple |
| Start Experiment | Unity connected | Green |
| Stop Experiment | Unity connected | Red |
| Refresh | Always | Gray |

---

## 🔄 Complete Workflow

### Full Experiment Workflow:

```
Step 1: Connect to Unity
  ↓
[Click "Connect to Unity"]
  ↓
Handshake completes
  ↓
Status: Unity ● Green
  ↓

Step 2: Start Experiment
  ↓
[Click "Start Experiment"]
  ↓
Command sent to Unity
  ↓
Unity starts sending VR data
  ↓

Step 3: Start Recording
  ↓
[Enter filename: "experiment_001"]
[Click "Start Recording"]
  ↓
CSV file created
  ↓
Data being saved (batches of 5)
  ↓

Step 4: Perform VR Actions
  ↓
[Subject performs VR tasks]
  ↓
Data streaming and recording
  ↓

Step 5: Stop Recording
  ↓
[Click "Stop Recording"]
  ↓
Remaining data written
  ↓
File saved: data/experiment_001.csv
  ↓

Step 6: Stop Experiment
  ↓
[Click "Stop Experiment"]
  ↓
Unity stops sending data
  ↓
Experiment complete!
```

---

## 📁 File Management

### Auto-Increment:
```
Request: "experiment"
Existing: experiment.csv

Result: experiment_1.csv
Modal: "File 'experiment.csv' already exists. Saving as 'experiment_1.csv'"
```

### Invalid Characters:
```
Input:  "my test! @#$ recording"
Output: "my_test_____recording.csv"

All special characters replaced with underscores
```

### Empty Filename:
```
Input:  ""
Output: "recording_1699123456789.csv"

Auto-generated using timestamp
```

---

## 🐛 Troubleshooting

### ENOTDIR Error

**Status**: ✅ **FIXED in v2.8**

Was caused by `.gitkeep` file in `data/` folder

### Recording Not Saving

**Check**:
1. Unity connected (green status)
2. Filename entered
3. "Start Recording" clicked
4. Data directory created successfully
5. Check debug log for "Saved X data points"

**Verify**:
```powershell
# Check data directory exists
dir data

# Check CSV files
dir data\*.csv

# View CSV contents
type data\experiment_001.csv
```

### Experiment Buttons Disabled

**Cause**: Unity not connected

**Solution**:
1. Click "Connect to Unity"
2. Wait for handshake to complete
3. Status turns ● Green
4. Buttons become enabled

---

## 🎯 Unity C# Integration

### Add Command Handling to Unity:

Add to your `AIMLABVRDataStreamer.cs` in the `ProcessReceivedMessage` method:

```csharp
case "CMD":
    if (parts.Length > 1)
    {
        ProcessCommand(parts[1]);
    }
    break;
```

Add this method:

```csharp
private void ProcessCommand(string command)
{
    RunOnMainThread(() =>
    {
        LogMessage($"Received command: {command}", LogLevel.Info);
        
        switch (command)
        {
            case "START_EXPERIMENT":
                StartExperiment();
                SendMessage("CMD:Experiment started");
                break;
                
            case "STOP_EXPERIMENT":
                StopExperiment();
                SendMessage("CMD:Experiment stopped");
                break;
        }
    });
}
```

Add public methods:

```csharp
public void StartExperiment()
{
    LogMessage("Experiment started by Electron", LogLevel.Success);
    enablePeriodicData = true;
    fileOpen = true;  // Enable data sending
}

public void StopExperiment()
{
    LogMessage("Experiment stopped by Electron", LogLevel.Info);
    enablePeriodicData = false;
    fileOpen = false;  // Disable data sending
}
```

---

## ✅ What's New in v2.8

### Experiment Control:
- [x] Start Experiment button (green)
- [x] Stop Experiment button (red)
- [x] Send CMD:START_EXPERIMENT to Unity
- [x] Send CMD:STOP_EXPERIMENT to Unity
- [x] Buttons enabled when Unity connected

### Recording Fixes:
- [x] Proper directory creation (recursive)
- [x] Handle .gitkeep file conflict
- [x] Smaller batch size (5 instead of 10)
- [x] File rename modal notification
- [x] Empty filename fallback

### UI Enhancements:
- [x] Modal popup for file renames
- [x] Success/Danger button colors
- [x] Better button state management
- [x] Improved logging

---

## 📊 Port Summary

| Port | Unity | Electron | Purpose |
|------|-------|----------|---------|
| 45000 | ✅ Server | ❌ | Discovery (Unity) |
| 45001 | ❌ | ✅ Server | Discovery (Electron) |
| 45101 | ✅ Server | 📤 Sends | Data (Unity listens) |
| 45102 | 📤 Sends | ✅ Server | Data (Electron listens) |

**All ports working!** No conflicts!

---

## 🎉 Status

**Version**: 2.8.0  
**Experiment Control**: ✅ **ADDED**  
**Recording**: ✅ **FIXED**  
**Port Conflicts**: ✅ **RESOLVED**  
**Status**: ✅ **PRODUCTION READY**

---

## 📞 Support

**Author**: Pi Ko  
**Email**: pi.ko@nyu.edu  
**Institution**: NYU  
**Project**: AIMLAB VR Research

---

**🎊 Full experiment control + all fixes - Ready for research! 🎊**

---

*Last Updated: 04 November 2025*  
*Experiment control and recording fixes complete*

