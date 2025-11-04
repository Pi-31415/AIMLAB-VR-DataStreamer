# AIMLAB VR Data Streamer - Quick Start Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025

## 🚀 Quick Start (5 Minutes)

### Step 1: Build the Application
```batch
.\build.bat
```

### Step 2: Run the Application
```batch
.\AIMLAB_VR_DataStreamer.exe
```

### Step 3: Wait for Auto-Discovery (20 seconds)
The application will automatically:
- ✅ Scan for Arduino vibration motor
- ✅ Listen for Unity VR headset broadcasts
- ⏱️ Show countdown timer

### Step 4: Start Recording
1. Enter filename: `my_experiment`
2. Click **"Start Recording"**
3. Perform your VR experiment
4. Click **"Stop Recording"** when done

✨ **That's it!** Your data is saved to `my_experiment.csv`

---

## 🔧 Manual Setup (If Auto-Discovery Fails)

### Arduino Connection
1. Plug in Arduino via USB
2. Click **"Refresh Vibration Motor"**
3. Wait for green indicator: ✅ CONNECTED [COM3]
4. Test with **"Test Vibration"** button

### Unity VR Connection
1. Start Unity VR application
2. Ensure Unity broadcasts discovery message (UDP port 55001)
3. Click **"Refresh VR Headset"**
4. Wait for green indicator: ✅ CONNECTED [192.168.1.100]

---

## 📊 Understanding the Interface

### Connection Status
```
Vibration Motor:  ✅ CONNECTED [COM3]    or    ❌ NOT CONNECTED
VR Headset:       ✅ CONNECTED [IP]      or    ❌ NOT CONNECTED
```

### Data Recording
```
Output Filename: [experiment_data        ] .csv
[Start Recording]  or  [Stop Recording]
```

### Statistics (During Recording)
```
Recording Time: 00:05:23
Packets Received: 3142
Packets Processed: 3142
Lines Written: 3142
Queue Size: 0
```

---

## 📁 Output Files

Your CSV files will be saved in the same directory as the executable:
- `experiment_data.csv` (first recording)
- `experiment_data_1.csv` (if file exists)
- `experiment_data_2.csv` (if file exists)
- ... and so on

### CSV Format
```csv
Timestamp,HeadPosX,HeadPosY,HeadPosZ,HeadRotX,HeadRotY,HeadRotZ,HeadRotW,...
0,0.12,1.65,-0.45,0.0,0.87,0.0,0.48,-0.3,1.2,-0.6,0.1,0.2,0.3,0.9,...
16,0.13,1.66,-0.44,0.01,0.86,0.01,0.49,-0.29,1.19,-0.59,0.11,0.19,0.31,0.91,...
```

---

## ⚠️ Common Issues

### Arduino Not Found
**Problem:** Red indicator for Vibration Motor  
**Solution:**
1. Check USB cable is connected
2. Open Device Manager → Ports (COM & LPT)
3. Note the COM port number
4. Click "Refresh Vibration Motor"

### VR Headset Not Found
**Problem:** Red indicator for VR Headset  
**Solution:**
1. Ensure Unity VR application is running
2. Check firewall allows UDP port 55001 and TCP port 55000
3. Verify same network as VR headset
4. Click "Refresh VR Headset"

### Can't Start Recording
**Problem:** "Start Recording" button is grayed out  
**Solution:**
- Must have VR headset connected (green indicator)
- Must enter a filename (at least 1 character)

### File Already Exists
**Problem:** Worried about overwriting data  
**Solution:**  
✅ **Don't worry!** The application automatically adds numbers:
- `data.csv` → `data_1.csv` → `data_2.csv` ...

---

## 🎯 Best Practices

### 1. Pre-Experiment Checklist
- [ ] Arduino plugged in and detected
- [ ] Unity VR application running
- [ ] Both devices showing green indicators
- [ ] Test vibration works
- [ ] Filename entered
- [ ] Sufficient disk space

### 2. During Experiment
- Monitor queue size (should stay low)
- Check packets received is increasing
- Don't close application suddenly

### 3. Post-Experiment
- Click "Stop Recording" properly
- Wait for file to close
- Verify CSV file was created
- Backup data immediately

---

## 📞 Need Help?

**Full Documentation:** See `README_VR_DATA_STREAMER.md`

**Contact:** Pi Ko (pi.ko@nyu.edu)

**Troubleshooting:** Check the Troubleshooting section in README

---

## 🎨 Interface Colors

- **Purple buttons/headers**: NYU branding (#542AA8)
- **Green text/indicators**: Connected/Recording
- **Red text/indicators**: Disconnected/Error
- **Orange text**: Discovery in progress
- **Gray text**: Informational text

---

## 💡 Pro Tips

1. **Use descriptive filenames**: `participant_01_task_A` instead of `data`
2. **Test before experiment**: Always test vibration and check connection
3. **Monitor queue size**: If it grows too large, VR might be sending data too fast
4. **Backup frequently**: Copy CSV files to another location regularly
5. **Network stability**: Use wired Ethernet for best results

---

## 🔄 Workflow Example

```
1. Launch application
   ↓
2. Auto-discovery (20 seconds)
   ↓
3. Green indicators for both devices
   ↓
4. Enter filename: "pilot_study_01"
   ↓
5. Click "Start Recording"
   ↓
6. Conduct VR experiment
   ↓
7. Click "Stop Recording"
   ↓
8. File saved: "pilot_study_01.csv"
   ↓
9. Repeat for next participant (auto-numbered: "pilot_study_01_1.csv")
```

---

**Happy Researching! 🎓🔬**

