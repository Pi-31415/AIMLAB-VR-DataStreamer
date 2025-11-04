# AIMLAB VR Data Streamer v6.2 - WITH PROGRESS INDICATORS

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** 6.2

## 🎯 Quick Start

```batch
# Build v6.2
.\build_v6.2.bat

# Run v6.2
.\AIMLAB_VR_DataStreamer_v6.2.exe
```

---

## ✨ What's New in v6.2

### 📊 Progress Bars
Watch real-time progress during connection attempts!

**Arduino Scanning:**
```
[████████████░░░░░░] Checking COM5... [5/12]
```

**Unity Discovery:**
```
[███████████████░░░] Listening for VR headset... (15s remaining)
```

### 🕐 Enhanced Logging
Millisecond-precision timestamps in debug console:
```
14:23:15.342 | Winsock initialized successfully
14:23:15.401 | === Arduino Connection Attempt Started ===
14:23:15.402 | Found 12 COM ports to scan
14:23:15.403 | Checking port: \\.\COM3
14:23:20.123 | SUCCESS: Connected to Arduino on COM5
```

### 💬 Status Messages
Know exactly what's happening:
- "Initializing search..."
- "Checking COM5... [5/12]"
- "Listening for VR headset... (15s remaining)"
- "Connecting to 192.168.1.100..."
- "Connected to COM5!"
- "No vibration motor found"

### ⏱️ Extended Timeouts
- Auto-discovery: 20 seconds (same as v6.1)
- Manual refresh: **30 seconds** (NEW - 50% longer)

### 🔄 Auto-Reset
Progress bars and status messages automatically disappear after 2 seconds

---

## 🆚 v6.1 vs v6.2

| Feature | v6.1 | v6.2 |
|---------|------|------|
| All core features | ✅ | ✅ |
| Progress bars | ❌ | ✅ |
| Status messages | ❌ | ✅ |
| Timestamped logs | ❌ | ✅ |
| Manual timeout | 20s | 30s |
| Auto-reset UI | ❌ | ✅ |
| File size | 1.93 MB | 1.97 MB |

**Both versions are fully functional!**

---

## 📋 Features

### Core Functionality (Same as v6.1)
- ✅ Arduino vibration motor control
- ✅ Unity VR headset data streaming
- ✅ CSV data logging with auto-numbering
- ✅ Auto-discovery on startup
- ✅ Manual refresh buttons
- ✅ NYU purple-themed UI
- ✅ Debug console
- ✅ Real-time statistics

### Enhanced in v6.2
- 📊 Progress bars during operations
- 💬 Real-time status messages
- 🕐 Millisecond timestamps in logs
- ⏱️ Longer timeout for manual operations
- 🔄 Auto-reset progress indicators
- 🎯 Better user feedback

---

## 🎮 Usage

### First Launch
1. Run `AIMLAB_VR_DataStreamer_v6.2.exe`
2. **Console window opens** - watch for logs
3. **Auto-discovery starts automatically**
4. Watch progress indicators:
   - Auto-discovery countdown: 20 seconds
   - Progress updates in console

### Manual Refresh
1. Click "Refresh Vibration Motor" or "Refresh VR Headset"
2. **Progress bar appears immediately**
3. Watch status messages update:
   - Arduino: "Checking COM5... [5/12]"
   - Unity: "Listening for VR headset... (15s remaining)"
4. **30-second timeout** for thorough scanning
5. Result message displays for 2 seconds
6. Progress bar auto-resets

### Recording Data
1. Ensure VR headset connected (green indicator)
2. Enter filename: `my_experiment`
3. Click "Start Recording"
4. File created: `my_experiment.csv` (or `my_experiment_1.csv` if exists)
5. Real-time statistics displayed
6. Click "Stop Recording" when done

---

## 🖥️ Console Output Examples

### Arduino Connection Success
```
14:25:10.123 | === Arduino Connection Attempt Started ===
14:25:10.124 | Found 12 COM ports to scan
14:25:10.125 | Checking port: \\.\COM1
14:25:10.127 |   - Could not open port
14:25:10.128 | Checking port: \\.\COM3
14:25:10.130 |   - Could not open port
14:25:10.131 | Checking port: \\.\COM5
14:25:10.133 |   - Attempting handshake...
14:25:13.456 | SUCCESS: Connected to Arduino on COM5
```

### Unity Discovery Success
```
14:26:30.789 | === Unity Discovery Started (timeout: 30s) ===
14:26:30.790 | Listening for VR headset broadcast on port 55001
14:26:35.123 | Received broadcast: 'VR_HEADSET_DISCOVERY'
14:26:35.124 | FOUND: VR headset at 192.168.1.100
14:26:35.125 | Attempting TCP connection to 192.168.1.100:55000
14:26:35.234 | TCP connection established, sending handshake...
14:26:35.345 | Received handshake response: 'VR_HEADSET_READY'
14:26:35.346 | SUCCESS: Connected to Unity VR application!
14:26:35.347 | Unity receive thread started
```

### No Devices Found
```
14:28:00.000 | === Arduino Connection Attempt Started ===
14:28:00.001 | Found 12 COM ports to scan
14:28:00.002 | Checking port: \\.\COM1
14:28:00.004 |   - Could not open port
... (checks all 12 ports)
14:28:10.567 | FAILED: No Arduino found on any COM port
```

---

## 🎨 User Interface

### Progress Bar States

#### Scanning (0-99%)
```
[████████░░░░░░░░░░] Checking COM5... [5/12]
```

#### Success (100%)
```
Connected to COM5!
```
*(Green text, displays for 2 seconds)*

#### Failure (100%)
```
No vibration motor found
```
*(Gray text, displays for 2 seconds)*

### Connection Indicators

**Connected:**
```
Vibration Motor: ● CONNECTED [COM5]
```
*(Green circle, green text)*

**Not Connected:**
```
Vibration Motor: ● NOT CONNECTED
```
*(Red circle, red text)*

---

## 📊 Statistics Display

### During Recording
```
Statistics
──────────────────────────────
Recording Time: 00:05:23
Packets Received: 3142
Packets Processed: 3142
Lines Written: 3142
Queue Size: 0
```

### When Idle
```
Statistics
──────────────────────────────
Not recording
```
*(Gray text)*

---

## 🔧 Build Information

### Requirements
- MinGW-w64 (g++)
- SDL2 development libraries
- Dear ImGui (included)
- Winsock2 (Windows built-in)

### Build Process
```batch
# Automated build
.\build_v6.2.bat

# Manual build
g++ main_v6.2.cpp imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp ^
    backends/imgui_impl_sdl2.cpp backends/imgui_impl_sdlrenderer2.cpp ^
    -I. -Ibackends -ISDL2/x86_64-w64-mingw32/include/SDL2 ^
    -LSDL2/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 ^
    -lgdi32 -lws2_32 -mwindows -std=c++17 ^
    -o AIMLAB_VR_DataStreamer_v6.2.exe
```

### Output
```
AIMLAB_VR_DataStreamer_v6.2.exe (1.97 MB)
SDL2.dll (automatically copied)
```

---

## 🐛 Troubleshooting

### Progress Bar Not Updating
**Problem:** Progress bar stuck at 0%

**Solutions:**
1. Check console for errors
2. Verify COM ports exist (Device Manager)
3. Ensure Unity is broadcasting
4. Try closing and reopening

### Status Message Not Changing
**Problem:** Sees "Initializing search..." forever

**Solutions:**
1. Check console for actual progress
2. Wait for full timeout (30 seconds)
3. Look for error messages in console
4. Restart application

### Console Not Showing Logs
**Problem:** Console window empty

**Solutions:**
1. Console should auto-open on startup
2. If missing, check Windows console settings
3. Try running from command prompt
4. Rebuild application

### Auto-Reset Not Working
**Problem:** Progress bars don't disappear

**Solutions:**
1. Wait 2 seconds after operation completes
2. Click anywhere to refresh UI
3. Should auto-reset automatically
4. Not critical - just visual

---

## 📈 Performance

### Resource Usage
- **CPU:** ~2-4% during discovery
- **Memory:** ~15 MB
- **Disk:** Minimal (CSV writes only)
- **Network:** UDP broadcasts (minimal)

### Benchmarks
- Arduino scan (12 ports): ~10-15 seconds
- Unity discovery (found): ~2-5 seconds
- Unity discovery (timeout): 30 seconds
- CSV write speed: ~1000 lines/second

---

## 🔒 Security

### Network Ports
- **55001 UDP:** Unity discovery (incoming)
- **55000 TCP:** Unity data stream (outgoing)

### Data Privacy
- All data stored locally
- No internet connection required
- CSV files unencrypted (local storage)
- No telemetry or analytics

---

## 📝 CSV Output Format

### Header
```csv
Timestamp,HeadPosX,HeadPosY,HeadPosZ,HeadRotX,HeadRotY,HeadRotZ,HeadRotW,
LeftHandPosX,LeftHandPosY,LeftHandPosZ,LeftHandRotX,LeftHandRotY,LeftHandRotZ,LeftHandRotW,
RightHandPosX,RightHandPosY,RightHandPosZ,RightHandRotX,RightHandRotY,RightHandRotZ,RightHandRotW
```

### Data Example
```csv
0,0.12,1.65,-0.45,0.0,0.87,0.0,0.48,-0.3,1.2,-0.6,0.1,0.2,0.3,0.9,0.3,1.2,-0.6,-0.1,0.2,0.3,0.9
16,0.13,1.66,-0.44,0.01,0.86,0.01,0.49,-0.29,1.19,-0.59,0.11,0.19,0.31,0.91,0.31,1.19,-0.59,-0.09,0.21,0.29,0.92
```

- **Timestamp:** Milliseconds since recording started
- **21 data columns:** 7 values each for head, left hand, right hand
- **Positions:** X, Y, Z in Unity coordinates
- **Rotations:** Quaternion (X, Y, Z, W)

---

## 🎯 Use Cases

### When to Use v6.2

✅ **Perfect for:**
- Teaching/training new users
- Demonstrating to stakeholders
- Debugging connection issues
- Slow or unreliable connections
- Lab environments with multiple users
- When users need reassurance app is working

❌ **Overkill for:**
- Fast, reliable connections
- Experienced users who understand the system
- Minimal UI preference
- Resource-constrained environments

### When to Use v6.1 Instead
- Simpler UI preferred
- Proven stability required
- Minimal resource usage needed
- Quick operations expected

**See V6.1_VS_V6.2_COMPARISON.md for detailed comparison**

---

## 📚 Documentation

### File Structure
```
Documentation:
├── README_v6.2.md                     (this file - v6.2 specific)
├── README_VR_DATA_STREAMER.md         (general documentation)
├── QUICKSTART_VR_STREAMER.md          (quick start guide)
├── V6.1_VS_V6.2_COMPARISON.md         (detailed comparison)
├── BUGFIX_v6.1.md                     (v6.1 bug fixes)
├── UPGRADE_GUIDE_v6.2.md              (upgrade guide)
└── VERSION_STATUS.md                  (version overview)

Source Code:
├── main_v6.2.cpp                      (v6.2 source - 1,650 lines)
├── main.cpp                           (v6.1 source - 1,070 lines)
└── build_v6.2.bat                     (v6.2 build script)
```

---

## 🎓 Tips & Tricks

### Maximize Console Visibility
1. Resize console window to see more logs
2. Use "Select Mode" in console to copy logs
3. Scroll to see historical events
4. Console persists for entire session

### Understanding Progress
- **0-99%:** Operation in progress
- **100%:** Operation complete (success or failure)
- **2-second display:** Gives you time to read result
- **Auto-reset:** Clears UI for next operation

### Optimal Workflow
1. Launch application
2. Watch auto-discovery in console
3. If not found, wait for timeout
4. Use manual refresh (30s) for thorough scan
5. Monitor progress bars for feedback
6. Check console for detailed logs

---

## 🆘 Support

### Contact
**Author:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Affiliation:** AIMLAB, New York University Abu Dhabi

### Bug Reports
Please include:
1. **Version:** v6.2
2. **Console output:** Copy from console window
3. **Screenshot:** Of UI with issue
4. **Steps to reproduce:** What you did
5. **Expected vs actual:** What should/did happen

### Feature Requests
v6.2 makes adding new progress indicators easy! Request features like:
- Custom timeout durations
- Additional device types
- More detailed progress breakdowns
- etc.

---

## 📜 License

Research use at NYU Abu Dhabi. Contact author for other usage.

---

## 🙏 Acknowledgments

**Technologies:**
- Dear ImGui by Omar Cornut
- SDL2 by Sam Lantinga
- OpenSans font by Steve Matteson

**Developed at:**
AIMLAB (AI & Machine Learning Applied to Behavior)
New York University Abu Dhabi

---

## 🎉 Enjoy v6.2!

Watch those progress bars fill up! 📊✨

---

**README Version:** 1.0  
**Last Updated:** 04 November 2025  
**Status:** ✅ Production Ready

