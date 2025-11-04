# Version 3.3 - Quick Summary

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 05 November 2025

## What's New in v3.3?

### 🎯 One-Click Experiment Recording

Recording is now **automatic** when you start/stop experiments!

---

## How to Use

### Simple 3-Step Process:

1. **Connect to Unity**
   - Click "Connect to Unity" button
   - Wait for connection confirmation

2. **Start Experiment**
   - Enter an Experiment ID (e.g., "Trial_001")
   - Click "Start Experiment"
   - ✅ Recording automatically starts!
   - ✅ CSV file created immediately!

3. **Stop Experiment**
   - Click "Stop Experiment"
   - ✅ Recording automatically stops!
   - ✅ Data saved to ExperimentalData folder!

---

## What Changed?

### ✅ Added:
- **Experiment ID textbox** - Name your experiment files
- **Automatic recording** - No separate record buttons
- **File validation** - Prevents overwriting existing data
- **Recording status** - See current recording filename

### ❌ Removed:
- Nothing! Just made it simpler

---

## Key Features

### 🛡️ **Protection Against Data Loss**
- System checks if file already exists
- Shows alert if duplicate filename
- Must choose different ID to continue

### 📝 **Smart File Naming**
- Uses your Experiment ID as filename
- Invalid characters auto-replaced
- Always saves to ExperimentalData folder

### ⚡ **Efficient Saving**
- Data written in batches (every 5 records)
- Better performance
- Final batch saved on stop

---

## File Naming Examples

| You Type | Saved As |
|----------|----------|
| `Trial_001` | `Trial_001.csv` |
| `Subject #5` | `Subject__5.csv` |
| `Test Data` | `Test_Data.csv` |

**Allowed:** Letters, numbers, underscore `_`, hyphen `-`  
**Replaced:** Everything else becomes underscore `_`

---

## Visual Guide

```
┌──────────────────────────────────┐
│  Unity Connection                │
├──────────────────────────────────┤
│  [Connect to Unity]              │
│                                  │
│  Experiment ID:                  │
│  [Trial_001____________]  ← Type here
│                                  │
│  [Start Experiment]      ← Click to record
│  [Stop Experiment]       ← Click to save
│  [Refresh]                       │
│                                  │
│  🔴 Recording: Trial_001.csv    │
└──────────────────────────────────┘
```

---

## Error Messages

### "Please enter an Experiment ID"
- **Fix:** Type a name in the Experiment ID box

### "File already exists"
- **Fix:** Choose a different Experiment ID
- **Why:** Prevents overwriting your data

### "Unity must be connected"
- **Fix:** Click "Connect to Unity" first

---

## Where's My Data?

Click **"Open Experiment Data Folder"** button to view all CSV files!

The folder opens automatically in Windows Explorer.

---

## Tips

💡 **Use descriptive IDs:** `Subject01_Trial01` is better than `test1`  
💡 **Keep it organized:** Use consistent naming patterns  
💡 **Check before starting:** Make sure your ID is unique  
💡 **Don't interrupt:** Let experiments complete fully  

---

## Common Questions

**Q: Do I need to start recording manually?**  
A: No! Recording starts automatically with the experiment.

**Q: What if I forget to enter an Experiment ID?**  
A: The system will remind you and won't start.

**Q: Can I use the same ID twice?**  
A: No, the system prevents duplicate filenames.

**Q: Where are my CSV files saved?**  
A: In the `ExperimentalData` folder (click button to open).

**Q: What if recording fails?**  
A: The experiment won't start if recording fails.

---

## Workflow Comparison

### OLD (v3.2):
1. Start experiment
2. Remember to start recording
3. Remember to stop recording
4. Stop experiment
5. Find your file manually

### NEW (v3.3):
1. Enter Experiment ID
2. Start experiment ✨ (auto-records!)
3. Stop experiment ✨ (auto-saves!)
4. Open folder to view

**50% fewer steps!** 🎉

---

## Version Info

- **Current Version:** 3.3.0
- **Previous Version:** 3.2.0
- **Release Date:** 05 November 2025
- **Ports:** Unity 45000/45101, Electron 45001/45102

---

## Next Steps

1. ✅ Test your first experiment with new workflow
2. ✅ Open data folder to verify CSV file
3. ✅ Check that data looks correct
4. ✅ Continue with your research!

---

**Need Help?** Check `EXPERIMENT_RECORDING_v3.3.md` for detailed documentation.

**Questions?** Contact Pi Ko (pi.ko@nyu.edu)

