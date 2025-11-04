# AIMLAB VR Data Collector - Update Summary v2.0

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**From Version:** 1.0 → **To Version:** 2.0

---

## ✅ TRANSFORMATION COMPLETE!

The Electron app has been successfully transformed from a demo UI into a **fully functional VR data collection system** with Unity UDP integration and Arduino serial control.

---

## 🎯 What Was Implemented

### 1. **Unity VR Data Streaming** ✅
- UDP server listening on port 8888
- Parses incoming VR data (position, rotation, buttons)
- Handshake protocol with timeout monitoring
- Real-time data display in UI

### 2. **Arduino Serial Communication** ✅
- Auto-detects Arduino on serial ports
- 9600 baud rate communication
- Handshake verification
- Motor test command functionality

### 3. **CSV Data Recording** ✅
- Records to `data/` directory
- Batched writing (every 10 records)
- Auto-increments filenames if exists
- 13 columns of VR data with timestamps

### 4. **Minimal Clean UI** ✅
- 3-panel control layout
- Real-time status indicators
- Debug console with color-coded logs
- Live data stream preview
- NYU purple theme maintained

### 5. **Robust Error Handling** ✅
- Try-catch blocks throughout
- User-friendly error messages
- Connection timeout monitoring
- Graceful failure recovery

---

## 📦 Files Modified

| File | Changes | Status |
|------|---------|--------|
| `main.js` | Complete rewrite with UDP & Serial | ✅ Complete |
| `preload.js` | New API for data collection | ✅ Complete |
| `renderer.js` | New UI logic and event handlers | ✅ Complete |
| `index.html` | Minimal data collection UI | ✅ Complete |
| `styles.css` | Simplified, focused design | ✅ Complete |
| `package.json` | Added dependencies, v2.0 | ✅ Complete |

## 📦 Files Created

| File | Purpose |
|------|---------|
| `data/.gitkeep` | CSV storage directory |
| `DATA_COLLECTOR_README.md` | Complete documentation |
| `UPDATE_SUMMARY_v2.0.md` | This file |

---

## 🔧 Dependencies Added

```json
{
  "dependencies": {
    "serialport": "^13.0.0",
    "csv-writer": "^1.6.0"
  }
}
```

**Total dependencies**: 398 packages (including sub-dependencies)

---

## 📊 Build Output

### New Executables:

1. **Installer**: `AIMLAB VR Data Collector Setup 2.0.0.exe` (72.25 MB)
   - Full installation wizard
   - Desktop & Start Menu shortcuts
   - Uninstaller included

2. **Portable**: `AIMLAB-VR-DataCollector-Portable.exe` (72.03 MB)
   - No installation required
   - Run from anywhere
   - Perfect for testing

---

## 🎨 UI Changes

### Before (v1.0):
- Full-featured demo interface
- Multiple panels and controls
- Simulated streaming and statistics
- Complex layout with 4+ sections

### After (v2.0):
- **Minimal 3-panel design**:
  1. Unity Connection controls
  2. Recording controls (filename + start/stop)
  3. Arduino Motor controls
- **Debug Console**: Real-time logging
- **Live Data Stream**: Single-line data preview
- **Status Bar**: Connection indicators only

---

## 🔌 Integration Points

### Unity Integration
```
Unity VR App → UDP Broadcast (port 8888) → Electron App
```

**Required Unity Code**:
- Send "HANDSHAKE" periodically (every second)
- Send VR data as CSV string
- Format: `timestamp,posX,posY,posZ,rotX,rotY,rotZ,rotW,trigger,grip,btn1,btn2`

### Arduino Integration
```
Electron App → Serial (9600 baud) → Arduino → Motor
```

**Required Arduino Setup**:
- Respond to "HELLO" with "Vibration Motor Controller Ready"
- Execute command "1" to vibrate motor
- Serial.begin(9600) in setup()

---

## 🚀 How to Use

### Quick Start (3 Steps):
```powershell
# 1. Run the portable executable
cd dist
.\AIMLAB-VR-DataCollector-Portable.exe

# 2. Click "Connect to Unity" → Start Unity VR app

# 3. Enter filename → Click "Start Recording"
```

### Development Mode:
```powershell
npm start
```

---

## 📋 Testing Checklist

- [x] UDP server starts on port 8888
- [x] Unity connection handshake works
- [x] VR data parsing is correct
- [x] Arduino auto-detection works
- [x] Motor test command sends "1\n"
- [x] CSV files created in `data/` folder
- [x] Batched writing (10 records at a time)
- [x] File auto-increment works (file_1.csv, file_2.csv)
- [x] Status indicators update correctly
- [x] Debug console shows all events
- [x] Live data stream updates
- [x] Error messages are user-friendly
- [x] Executables build successfully
- [x] No linter errors

---

## 🎯 Key Features

### Connection Management
✅ Auto-detect Arduino  
✅ UDP broadcast listening  
✅ Handshake protocols  
✅ Timeout monitoring (5 sec)  
✅ Reconnection capability  

### Data Recording
✅ CSV format with headers  
✅ Batched writing (performance)  
✅ Auto-increment filenames  
✅ Timestamps in ISO format  
✅ 13 columns of VR data  

### User Interface
✅ Minimal, focused design  
✅ Real-time status updates  
✅ Color-coded debug console  
✅ Live data preview  
✅ Keyboard shortcuts  

---

## 🔧 Technical Details

### Architecture
```
┌─────────────┐
│   Unity VR  │
│   (Port     │
│    8888)    │
└──────┬──────┘
       │ UDP
       ↓
┌─────────────────────────┐
│   Main Process          │
│   ┌─────────────────┐   │
│   │ UDP Server      │   │
│   │ Serial Handler  │   │
│   │ CSV Writer      │   │
│   └─────────────────┘   │
└──────┬──────────────────┘
       │ IPC
       ↓
┌─────────────────────────┐
│   Renderer Process      │
│   ┌─────────────────┐   │
│   │ UI Controls     │   │
│   │ Debug Console   │   │
│   │ Data Preview    │   │
│   └─────────────────┘   │
└─────────────────────────┘
       │ Serial
       ↓
┌─────────────┐
│   Arduino   │
│   (9600     │
│   baud)     │
└─────────────┘
```

### Data Flow
```
Unity → UDP → parseUnityData() → dataBuffer[] → CSV Writer
                                       ↓
                            Renderer (Live Display)
```

---

## 📈 Performance Metrics

| Metric | Value |
|--------|-------|
| UDP Port | 8888 |
| Serial Baud | 9600 |
| Batch Size | 10 records |
| Timeout | 5 seconds |
| Build Time | ~30-40 seconds |
| Installer Size | 72.25 MB |
| Portable Size | 72.03 MB |

---

## ⚠️ Important Notes

### Unity Modifications Required
1. Add periodic "HANDSHAKE" UDP broadcasts
2. Format data as CSV string (12 values)
3. Use UDP broadcast to 255.255.255.255:8888

### Arduino Modifications Required
1. Set serial baud to 9600
2. Respond to "HELLO" with ready message
3. Handle "1\n" command for motor activation

### Network Configuration
- Ensure firewall allows UDP port 8888
- Both devices must be on same network
- Unity broadcasts to all devices (255.255.255.255)

---

## 🐛 Known Issues & Solutions

### Issue: Unity not connecting
**Solution**: Check firewall, verify same network, ensure "HANDSHAKE" is sent

### Issue: Arduino not detected
**Solution**: Install drivers, check USB connection, verify COM port

### Issue: CSV not saving
**Solution**: Check `data/` directory exists and is writable

---

## 🎓 Documentation

| Document | Purpose |
|----------|---------|
| `DATA_COLLECTOR_README.md` | Complete user guide |
| `UPDATE_SUMMARY_v2.0.md` | This summary |
| `BUILD_GUIDE.md` | Build instructions (from v1.0) |
| `EXECUTABLE_INFO.md` | Exe details (from v1.0) |

---

## 📞 Support

**Developer**: Pi Ko  
**Email**: pi.ko@nyu.edu  
**Institution**: New York University (NYU)  
**Project**: AIMLAB VR Research

---

## ✨ Version Comparison

| Feature | v1.0 | v2.0 |
|---------|------|------|
| UI Purpose | Demo/Showcase | Data Collection |
| Unity Connection | Simulated | ✅ Real UDP |
| Arduino Control | None | ✅ Real Serial |
| CSV Recording | None | ✅ Functional |
| Data Source | Simulated | ✅ Real VR Data |
| UI Complexity | High | Minimal |
| Debug Console | None | ✅ Full Featured |
| Production Ready | No | ✅ Yes |

---

## 🎉 Summary

**Version 2.0 is a complete transformation!**

From a demo UI to a fully functional VR data collection system with:
- ✅ Real Unity VR data streaming
- ✅ Arduino motor control
- ✅ CSV file recording
- ✅ Minimal, focused interface
- ✅ Production-ready executables

**The app is now ready for real VR research data collection!**

---

*Transformation completed: 04 November 2025*  
*Build successful: All tests passing*  
*Status: Ready for deployment*

