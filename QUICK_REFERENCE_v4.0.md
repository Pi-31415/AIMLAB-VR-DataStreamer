# AIMLAB VR Data Collector v4.0 - Quick Reference

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 08 January 2026  
**Version:** v4.0

---

## What Changed in v4.0?

### 1. Hidden Buttons
- **"Start Left Hand"** button is now hidden (only "Start Right Hand" visible)
- **"Toggle Transparency for Peg Board"** button is now hidden

### 2. New File Naming System
Files are now saved with timestamps in this format:

```
[ExperimentID]-YYYY-Mon-DD-HH-MM-am/pm.extension
```

**Example:**
- `TEST001-2026-Jan-08-02-30-pm.tsv`
- `EXP123-2026-Feb-14-09-15-am.tsv`

### 3. No More Overwrites
- System NEVER overwrites existing files
- Same Experiment ID can be used multiple times
- Each save creates a new timestamped file
- If saved in same minute, adds seconds: `TEST001-2026-Jan-08-02-30-pm-45.tsv`

---

## File Naming Format Explained

```
ExperimentID-2026-Jan-08-02-30-pm.tsv
     ↓        ↓   ↓   ↓  ↓  ↓  ↓   ↓
     ID      Year Mon Day Hr Min PM Ext
```

- **ExperimentID:** Your chosen experiment identifier
- **Year:** Four-digit year (2026)
- **Month:** Three-letter month abbreviation (Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec)
- **Day:** Two-digit day (01-31)
- **Hour:** Two-digit hour in 12-hour format (01-12)
- **Minute:** Two-digit minute (00-59)
- **am/pm:** Lowercase am or pm
- **Extension:** File type (.tsv or .csv)

---

## Quick Start

### 1. Launch Application
```powershell
npm start
```

### 2. Connect to Unity
1. Click **"Connect to Unity"**
2. Wait for connection confirmation

### 3. Run Experiment
1. Enter **Experiment ID** (e.g., "TEST001")
2. Click **"Start Right Hand"**
3. Perform VR experiment in Unity
4. Click **"Stop Experiment"**

### 4. Find Your Data
Files saved to:
```
ExperimentData/Right_Hand/TEST001-2026-Jan-08-02-30-pm.tsv
```

---

## Common Scenarios

### Scenario 1: Running Multiple Sessions with Same ID

**Previous Behavior (v3.9):**
- ❌ Warning: "File already exists"
- ❌ Required different Experiment ID or manual file deletion

**New Behavior (v4.0):**
- ✅ No warnings
- ✅ Each session creates unique timestamped file
- ✅ All data preserved

**Example:**
```
TEST001-2026-Jan-08-09-00-am.tsv   (Morning session)
TEST001-2026-Jan-08-02-30-pm.tsv   (Afternoon session)
TEST001-2026-Jan-08-04-15-pm.tsv   (Evening session)
```

### Scenario 2: Mid-Experiment Saves

**Action:** Click "Save Data Mid Experiment" during experiment

**Result:**
```
TEST001-2026-Jan-08-02-30-pm.tsv   (Mid-save #1)
TEST001-2026-Jan-08-02-35-pm.tsv   (Mid-save #2)
TEST001-2026-Jan-08-02-40-pm.tsv   (Final save at stop)
```

### Scenario 3: Rapid Saves (Same Minute)

**Action:** Start/stop experiments very quickly

**Result:**
```
RAPID001-2026-Jan-08-02-30-pm.tsv
RAPID001-2026-Jan-08-02-30-pm-15.tsv    (15 seconds after)
RAPID001-2026-Jan-08-02-30-pm-47.tsv    (47 seconds after)
```

---

## Folder Structure

```
ExperimentData/
├── Left_Hand/
│   ├── LEFTEXP001-2026-Jan-08-09-00-am.tsv
│   └── LEFTEXP002-2026-Jan-08-10-30-am.tsv
│
└── Right_Hand/
    ├── TEST001-2026-Jan-08-02-30-pm.tsv
    ├── TEST001-2026-Jan-08-03-45-pm.tsv
    └── TEST002-2026-Jan-08-04-15-pm.tsv
```

---

## FAQ

### Q: Can I still use the same Experiment ID for multiple experiments?
**A:** Yes! Each save creates a unique timestamped file. No more conflicts.

### Q: What if I want to find all data for one Experiment ID?
**A:** Files are named starting with the Experiment ID, so you can:
- Sort by name to group them together
- Search for the ID in File Explorer
- Use filters in data analysis tools

### Q: Are the hidden buttons gone forever?
**A:** No, they still exist and function. They're just hidden from view. They can be re-enabled if needed.

### Q: Does this work with the Unity C# code?
**A:** Yes, no Unity changes needed. Unity sends files the same way; we just save them with timestamps.

### Q: What happens if the file already exists with the exact timestamp?
**A:** The system adds seconds (or milliseconds) to make it unique. This is extremely rare.

### Q: Can I change the timestamp format?
**A:** Currently no, but it's possible to modify in future versions. The format was chosen for:
- Readability (human-friendly)
- Sortability (chronological order)
- Compatibility (Windows-safe characters)

### Q: How do I find today's experiments?
**A:** Files are automatically sorted by date in the filename. Look for today's date:
```
2026-Jan-08
```

### Q: Do old files (pre-v4.0) still work?
**A:** Yes! Old files without timestamps remain as-is and can still be read/analyzed normally.

---

## Troubleshooting

### Issue: Timestamp shows wrong time
**Cause:** System clock is incorrect
**Solution:** Verify Windows time settings are correct

### Issue: Very long filenames
**Cause:** Long Experiment ID + timestamp
**Solution:** Use shorter Experiment IDs (Windows has 260-character path limit)

### Issue: Files not saving
**Cause:** Permission issues or full disk
**Solution:** 
1. Check disk space
2. Verify ExperimentData folder permissions
3. Run application as administrator if needed

### Issue: Can't find saved files
**Cause:** Looking in wrong folder
**Solution:** Click "Open Experiment Data Folder" button to navigate directly

---

## Tips & Best Practices

### Experiment ID Naming
✅ **Good:**
- `EXP001`, `EXP002`, `EXP003`
- `SubjectA_Trial1`, `SubjectA_Trial2`
- `Pilot01`, `Pilot02`

❌ **Avoid:**
- Very long IDs (creates long filenames)
- Special characters (@, #, $, %, etc.)
- Spaces (use underscores instead)

### Organization
1. **Use descriptive IDs:** Include subject/condition info
2. **Keep naming consistent:** Stick to one format
3. **Document your system:** Keep a log of Experiment IDs
4. **Backup regularly:** Copy ExperimentData folder to backup location

### Data Management
- Sort files by name to group by Experiment ID
- Sort files by date (in filename) for chronological order
- Use Excel/Python to batch process multiple files
- Create summary sheets linking timestamps to conditions

---

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+O | Open Data Folder |
| Ctrl+L | Clear Debug Log |

---

## Version History

| Version | Date | Key Changes |
|---------|------|-------------|
| v4.0 | 08 Jan 2026 | Timestamp-based naming, hidden buttons |
| v3.9 | 19 Nov 2025 | Mid-experiment save, auto-sync |
| v3.8 | 19 Nov 2025 | ADB sync functionality |
| v3.7 | 05 Nov 2025 | Chunked TSV transfer |
| v3.6 | 05 Nov 2025 | TSV file handling from Unity |

---

## Support & Contact

**Developer:** Pi Ko  
**Email:** pi.ko@nyu.edu  
**Institution:** NYU Abu Dhabi - Applied Interactive Multimedia Laboratory

---

## Files Modified in v4.0

- `index.html` - Hidden buttons, updated version
- `renderer.js` - Updated version info, added changelog
- `main.js` - Timestamp functions, file naming logic
- `IMPLEMENTATION_v4.0.md` - Detailed implementation notes
- `TESTING_GUIDE_v4.0.md` - Complete testing procedures
- `QUICK_REFERENCE_v4.0.md` - This document

---

**END OF QUICK REFERENCE**

