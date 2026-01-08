# AIMLAB VR Data Collector - Version 4.0 Release Notes

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Release Date:** 08 January 2026  
**Version:** 4.0.0

---

## 🎉 What's New in Version 4.0

### Major Features

#### 1. **Timestamp-Based File Naming**
Files are now saved with automatic timestamps, eliminating the risk of accidental overwrites.

**Format:** `[ExperimentID]-YYYY-Mon-DD-HH-MM-am/pm.extension`

**Example:**
```
TEST001-2026-Jan-08-02-30-pm.tsv
```

**Benefits:**
- 🛡️ **Data Safety:** Never overwrites existing files
- 📅 **Time Tracking:** Exact timestamp embedded in filename
- 🔄 **Reusable IDs:** Same Experiment ID can be used multiple times
- 📊 **Easy Organization:** Files naturally sort chronologically

#### 2. **Simplified User Interface**
Streamlined UI by hiding less frequently used buttons:
- Hidden: "Start Left Hand" button
- Hidden: "Toggle Transparency for Peg Board" button
- Kept: All core functionality remains accessible

**Benefits:**
- ✨ **Cleaner Interface:** Less clutter, easier to use
- 🎯 **Focus on Essentials:** Right hand experiment is primary workflow
- 🔧 **Preserved Functionality:** Hidden buttons still work if needed

#### 3. **No More File Existence Checks**
The system no longer blocks experiments based on existing files.

**Previous Behavior:**
```
❌ Error: "File TEST001.tsv already exists. Use different ID."
```

**New Behavior:**
```
✅ Creates: TEST001-2026-Jan-08-02-30-pm.tsv
✅ No blocking, no warnings
```

---

## 📋 Complete Feature List

### Core Features (Retained from v3.9)
- ✅ Unity VR connection via UDP
- ✅ Arduino vibration motor control
- ✅ Real-time data streaming
- ✅ TSV file reception from Unity
- ✅ Chunked file transfer (large files)
- ✅ Mid-experiment save functionality
- ✅ ADB sync from Android headset
- ✅ Separate Left/Right hand folders
- ✅ Debug console with logging
- ✅ Splash screen
- ✅ Status indicators

### New Features (v4.0)
- ✅ Automatic timestamp generation
- ✅ Collision-free filename creation
- ✅ Hidden UI buttons (preserving functionality)
- ✅ No file existence blocking
- ✅ Improved file organization

---

## 🔧 Technical Details

### Timestamp Format Specification

| Component | Format | Example | Range |
|-----------|--------|---------|-------|
| Year | YYYY | 2026 | 0000-9999 |
| Month | Mon | Jan | Jan-Dec |
| Day | DD | 08 | 01-31 |
| Hour | HH | 02 | 01-12 |
| Minute | MM | 30 | 00-59 |
| Period | am/pm | pm | am, pm |
| Separator | - | - | Fixed |

**Full Format:** `YYYY-Mon-DD-HH-MM-am/pm`

### Collision Handling

The system prevents filename collisions using a three-tier approach:

**Tier 1:** Base timestamp (minute precision)
```
TEST001-2026-Jan-08-02-30-pm.tsv
```

**Tier 2:** Add seconds if needed
```
TEST001-2026-Jan-08-02-30-pm-45.tsv
```

**Tier 3:** Add milliseconds if still needed (rare)
```
TEST001-2026-Jan-08-02-30-pm-45-789.tsv
```

### File Locations

```
ExperimentData/
├── Left_Hand/          (Left hand experiments)
└── Right_Hand/         (Right hand experiments)
```

---

## 📊 Comparison: v3.9 vs v4.0

| Feature | v3.9 | v4.0 |
|---------|------|------|
| **File Naming** | `ExperimentID.tsv` | `ExperimentID-YYYY-Mon-DD-HH-MM-am/pm.tsv` |
| **Overwrite Protection** | Warning modal | Automatic prevention |
| **Same ID Multiple Times** | ❌ Blocked | ✅ Allowed |
| **File Existence Check** | Blocks start | Always allows |
| **Left Hand Button** | Visible | Hidden |
| **Transparency Button** | Visible | Hidden |
| **Mid-Save Naming** | Static | Timestamped |
| **Version Display** | 3.9.0 | 4.0.0 |

---

## 🚀 Upgrade Instructions

### For Users Already Running v3.9

1. **Close the application** if running
2. **Pull latest code** from repository
3. **No data migration needed** - old files remain as-is
4. **Run the application:** `npm start`
5. **Verify version** shows "4.0.0" in UI

### For New Users

1. Clone repository
2. Install dependencies: `npm install`
3. Run application: `npm start`
4. Follow on-screen instructions to connect Unity and Arduino

---

## 📖 Documentation Updates

New documentation files included:
- `IMPLEMENTATION_v4.0.md` - Detailed technical implementation
- `TESTING_GUIDE_v4.0.md` - Comprehensive testing procedures
- `QUICK_REFERENCE_v4.0.md` - Quick reference guide
- `VERSION_4.0_RELEASE_NOTES.md` - This document

---

## 🐛 Bug Fixes

### Fixed in v4.0
- **File Overwrite Risk:** Eliminated by timestamp-based naming
- **Experiment ID Blocking:** No longer blocks on existing files
- **UI Clutter:** Simplified by hiding less-used buttons

---

## ⚠️ Breaking Changes

### None!
Version 4.0 maintains full backward compatibility:
- ✅ Unity C# code requires no changes
- ✅ Arduino code requires no changes
- ✅ Old data files remain accessible
- ✅ File formats unchanged (TSV/CSV)
- ✅ Protocol remains the same
- ✅ Hidden buttons still functional

---

## 🔮 Future Roadmap

### Planned for v4.1+
- File management UI (view/delete old files)
- Custom timestamp format options
- Batch export functionality
- Metadata file generation
- Timezone support
- File compression options

### Under Consideration
- Database integration
- Cloud backup
- Multi-user support
- Experiment session linking
- Advanced analytics

---

## 📝 Known Limitations

### Current Limitations
1. **Filename Length:** Long Experiment IDs + timestamp may create long paths
   - Workaround: Use shorter Experiment IDs
   
2. **Timezone:** Uses local system time, no timezone indicator
   - Workaround: Note timezone in experiment documentation
   
3. **Rapid Saves:** Multiple saves per second add seconds/milliseconds to filename
   - Not a functional issue, just aesthetic

---

## 🧪 Testing Status

All critical tests passing:
- ✅ UI changes verified
- ✅ Timestamp generation functional
- ✅ File naming correct
- ✅ No overwrites confirmed
- ✅ Multiple saves create unique files
- ✅ Mid-experiment saves work
- ✅ No file blocking
- ✅ Hidden buttons preserve functionality
- ✅ Folder structure maintained
- ✅ No linter errors
- ✅ Application builds successfully

---

## 💻 System Requirements

### Unchanged from v3.9
- **OS:** Windows 10 or later
- **Node.js:** v16 or later
- **Unity:** 2021.3.22f1 or later (for VR app)
- **Arduino:** IDE 1.8+ (for motor controller)
- **ADB:** Android Debug Bridge (for Quest 3 sync)

### Disk Space
- Application: ~200 MB
- Each TSV file: ~1-10 MB (varies by experiment length)

---

## 📞 Support

### Getting Help
- **Email:** pi.ko@nyu.edu
- **Institution:** NYU Abu Dhabi - Applied Interactive Multimedia Laboratory
- **Documentation:** See included `.md` files
- **Issues:** Report via email with log files

### Before Contacting Support
1. Check `QUICK_REFERENCE_v4.0.md` for common scenarios
2. Review `TESTING_GUIDE_v4.0.md` for troubleshooting
3. Check debug console for error messages
4. Note exact steps to reproduce issue

---

## 🙏 Acknowledgments

### Contributors
- **Pi Ko** - Primary development and implementation
- **AIMLAB Research Team** - Testing and feedback
- **NYU Abu Dhabi** - Research support

### Technology Stack
- **Electron** - Desktop application framework
- **Node.js** - Backend runtime
- **SerialPort** - Arduino communication
- **csv-writer** - File I/O operations
- **dgram** - UDP networking

---

## 📄 License

Copyright © 2026 NYU Abu Dhabi - Applied Interactive Multimedia Laboratory

---

## 📈 Statistics

### Code Changes
- **3 files modified:** `index.html`, `renderer.js`, `main.js`
- **2 functions added:** `generateTimestamp()`, `generateUniqueFilename()`
- **4 handlers modified:** File checking and TSV saving logic
- **Lines added:** ~120 lines of new code
- **Lines modified:** ~50 lines of existing code

### Documentation
- **4 new documents:** Implementation, testing, quick reference, release notes
- **Total pages:** ~40 pages of documentation
- **Test cases:** 15+ comprehensive test scenarios

---

## 🎯 Version Numbering

Version format: **MAJOR.MINOR.PATCH**

- **4** (Major) - Significant new feature (timestamp-based naming)
- **0** (Minor) - UI changes and improvements
- **0** (Patch) - Initial release of v4.0

---

## 📅 Release Timeline

- **19 November 2025:** v3.9 released (mid-experiment save)
- **08 January 2026:** v4.0 released (timestamp naming, UI cleanup)
- **TBD:** v4.1 planned (file management features)

---

## ✅ Checklist for v4.0 Deployment

Deployment checklist:
- [x] Code changes implemented
- [x] All files updated with version info
- [x] Documentation created
- [x] Testing guide prepared
- [x] No linter errors
- [x] Application runs successfully
- [x] Build tested (optional)
- [ ] User acceptance testing
- [ ] Production deployment
- [ ] User training materials

---

## 🔒 Security Notes

No security changes in v4.0:
- Same security model as v3.9
- No new network connections
- No new permissions required
- File operations remain local
- No cloud/external services

---

## 📦 Distribution

### Development Version
```bash
git clone [repository]
cd AIMLAB-VR-DataStreamer
npm install
npm start
```

### Production Build
```bash
npm run build
# Creates aimlab_network.exe in project root
```

---

**Version 4.0 is ready for use!**

For questions or issues, contact: pi.ko@nyu.edu

---

**END OF RELEASE NOTES**

