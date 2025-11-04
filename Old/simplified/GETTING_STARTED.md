# Getting Started with AIMLAB VR Data Streamer

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

Welcome! This guide will help you get the AIMLAB VR Data Streamer up and running in just a few minutes.

---

## 🚀 Quick Start (5 Minutes)

### Step 1: Verify Your Setup
```batch
verify_setup.bat
```

This checks if you have all required tools installed.

### Step 2: Build the Application
```batch
build.bat
```

This compiles the console application. You should see:
```
Build SUCCESSFUL!
Run: AIMLAB_Console.exe
```

### Step 3: Connect Your Devices
1. **Plug in Arduino** via USB
2. **Put on VR headset** (ensure Unity app is running)

### Step 4: Run the Application
```batch
AIMLAB_Console.exe
```

The application will automatically try to connect to both devices!

---

## 📋 What You'll See

### On First Run

```
========================================
    AIMLAB VR Data Streamer Console    
    NYU Abu Dhabi - Version 1.0        
========================================

Starting auto-connect...

[Arduino Connection]
Scanning COM ports...
Found 3 COM ports.
Checking \\.\COM3... handshaking... SUCCESS!
Arduino connected on COM3

[Unity Connection]
Listening for Unity broadcast on port 55001...
Found Unity at 192.168.1.100
Connecting to Unity at 192.168.1.100:55000...
Unity connected successfully!
```

### Main Menu

```
[STATUS]
Arduino Motor: CONNECTED (COM3)
Unity VR:      CONNECTED (192.168.1.100)
Recording:     INACTIVE

[MENU]
1. Connect/Refresh Arduino
2. Connect/Refresh Unity
3. Test Vibration
4. Send Message to Unity
5. Start Recording
6. Stop Recording
7. Auto-Connect Both
8. Disconnect All
0. Exit

Choice: _
```

---

## 🎮 Basic Operations

### Test the Vibration Motor
1. Press **3** (Test Vibration)
2. You should feel/see the motor vibrate
3. Message: "Vibration command sent"

### Record a VR Session
1. Press **5** (Start Recording)
2. Type a filename: `my_first_recording`
3. Press Enter
4. Perform your VR experiment
5. Press **6** (Stop Recording)
6. Find your data in: `my_first_recording.csv`

### Analyze Your Data
```batch
analyze.bat my_first_recording.csv
```

This creates a folder with:
- 3D trajectory plots
- Position time series graphs
- Velocity analysis
- Statistical summary

---

## 📁 Project Files Guide

### Essential Files (Don't Delete!)
- **`main.cpp`** - Console application source code
- **`AIMLAB_Console.exe`** - The program you run
- **`Arduino_Vibration_Controller.ino`** - Arduino firmware
- **`Unity_VRDataStreamer.cs`** - Unity script

### Helper Scripts
- **`build.bat`** - Builds the application
- **`verify_setup.bat`** - Checks your setup
- **`analyze.bat`** - Runs data analysis

### Documentation
- **`README.md`** - Main documentation
- **`QUICK_START.md`** - Quick reference
- **`SETUP_GUIDE.md`** - Detailed setup instructions
- **`PROJECT_OVERVIEW.md`** - System architecture
- **`GETTING_STARTED.md`** - This file!

---

## 🔧 Setting Up Each Component

### Arduino Setup (First Time Only)

1. **Install Arduino IDE**
   - Download: https://www.arduino.cc/en/software
   - Install and open

2. **Open the Sketch**
   - File → Open → `Arduino_Vibration_Controller.ino`

3. **Select Your Board**
   - Tools → Board → Arduino Uno (or your model)

4. **Upload**
   - Sketch → Upload (or press Ctrl+U)
   - Wait for "Done uploading"

5. **Verify**
   - Tools → Serial Monitor
   - Set to 9600 baud
   - Type `HELLO` and press Enter
   - Should see: `Vibration Motor Controller Ready`

### Unity Setup (First Time Only)

1. **Copy the Script**
   - Copy `Unity_VRDataStreamer.cs` to your Unity project
   - Paste in: `Assets/Scripts/`

2. **Add to Scene**
   - Create empty GameObject (GameObject → Create Empty)
   - Name it: `VRDataStreamer`
   - Drag the script onto it

3. **Configure Inspector**
   - Select `VRDataStreamer` in Hierarchy
   - In Inspector, assign:
     - **Head Transform**: Your VR Camera
     - **Left Hand Transform**: Left Controller
     - **Right Hand Transform**: Right Controller

4. **Build and Deploy**
   - File → Build Settings
   - Select your platform (PC, Android for Quest, etc.)
   - Build and Run

---

## 💡 Tips for Beginners

### If Arduino Won't Connect
1. Check USB cable (try a different one)
2. Make sure Arduino sketch is uploaded
3. Close Arduino IDE Serial Monitor
4. Try pressing the reset button on Arduino
5. Replug the USB cable

### If Unity Won't Connect
1. Ensure Unity app is running on the headset
2. Check both devices are on same WiFi network
3. Try manual IP connection:
   - Press **2** in menu
   - Press any key to skip auto-discovery
   - Type **y** to enter IP manually
   - Enter the IP address of your VR device

### If Recording Doesn't Work
1. Make sure Unity is connected first
2. Check the status display shows "CONNECTED"
3. Try moving the headset to generate data
4. Look for the CSV file in the same folder

---

## 📖 Learning Path

### Beginner (Day 1)
1. ✅ Verify setup with `verify_setup.bat`
2. ✅ Build the application
3. ✅ Test Arduino connection
4. ✅ Test Unity connection
5. ✅ Record a short test session
6. ✅ Analyze the test data

### Intermediate (Week 1)
1. Read `SETUP_GUIDE.md` for detailed information
2. Experiment with different recording durations
3. Try manual IP connection
4. Modify vibration patterns in Arduino
5. Adjust stream rate in Unity

### Advanced (Ongoing)
1. Read `PROJECT_OVERVIEW.md` for architecture
2. Modify network ports if needed
3. Customize data analysis scripts
4. Integrate with your experiment software
5. Contribute improvements!

---

## 🆘 Common Questions

### Q: Do I need to rebuild after every change?
**A:** Only if you modify `main.cpp`. Arduino and Unity changes require re-uploading/rebuilding those components separately.

### Q: Can I use this with multiple VR headsets?
**A:** Currently, the console app connects to one headset at a time. You can disconnect and connect to a different one.

### Q: What if my COM port changes?
**A:** No problem! The app automatically scans all COM ports. Just run it again.

### Q: Where is my recorded data saved?
**A:** In the same folder as `AIMLAB_Console.exe`. Look for `.csv` files.

### Q: Can I use this on Mac/Linux?
**A:** The console app is Windows-only (uses Winsock). However, Arduino and Unity work cross-platform. The analysis script works on any OS with Python.

### Q: What VR headsets are supported?
**A:** Any headset that Unity supports (Quest, Vive, Index, Windows MR, etc.)

---

## 🎯 Next Steps

### Recommended Reading Order
1. ✅ **GETTING_STARTED.md** (You are here!)
2. 📖 **QUICK_START.md** - Quick reference for operations
3. 📖 **README.md** - Overview and features
4. 📖 **SETUP_GUIDE.md** - Detailed setup and troubleshooting
5. 📖 **PROJECT_OVERVIEW.md** - System architecture and customization

### Try These Tutorials
1. **Tutorial 1: First Recording**
   - Connect both devices
   - Record 30 seconds of head movement
   - Analyze the data
   - Goal: Understand the workflow

2. **Tutorial 2: Vibration Timing**
   - Start recording
   - Trigger vibration at specific moments
   - Note the timestamps
   - Goal: Sync events with data

3. **Tutorial 3: Multiple Trials**
   - Record multiple sessions
   - Name them systematically (trial_01, trial_02, etc.)
   - Batch analyze them
   - Goal: Manage experiment data

---

## 📞 Getting Help

### Self-Help Resources
1. Check the error message carefully
2. Read the relevant section in `SETUP_GUIDE.md`
3. Run `verify_setup.bat` to check your configuration
4. Try disconnecting and reconnecting (option 7 or 8)

### Contact Support
**Email:** pi.ko@nyu.edu

**Include in your email:**
- What you were trying to do
- Error messages (copy/paste or screenshot)
- Output from `verify_setup.bat`
- Which step you're stuck on

---

## ✅ Checklist for Success

Before your first experiment, ensure:

- [ ] `verify_setup.bat` shows "ALL CHECKS PASSED"
- [ ] `build.bat` completes successfully
- [ ] Arduino responds to "HELLO" in Serial Monitor
- [ ] Unity app runs on VR headset without errors
- [ ] Console app auto-connects to both devices
- [ ] Test vibration works (press 3)
- [ ] Can start and stop recording
- [ ] CSV file contains data after recording
- [ ] Analysis script generates graphs

---

## 🎉 You're Ready!

Congratulations! You now have a working VR experiment system.

**What you can do:**
- ✅ Record VR tracking data at high frequency
- ✅ Trigger haptic feedback during experiments
- ✅ Analyze movement patterns and statistics
- ✅ Export data for further analysis

**Happy experimenting!**

---

**Document Version:** 1.0  
**Last Updated:** 04 November 2025  
**Author:** Pi Ko (pi.ko@nyu.edu)

