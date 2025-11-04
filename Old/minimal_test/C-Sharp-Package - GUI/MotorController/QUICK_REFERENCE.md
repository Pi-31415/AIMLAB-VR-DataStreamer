# 🚀 Quick Reference - AIMLAB VR Data Streamer

## 📋 Two Versions Available

| | v6.1 | v6.2 |
|---|---|---|
| **File** | `main.cpp` | `main_v6.2.cpp` |
| **Build** | `build.bat` | `build_v6.2.bat` |
| **Output** | `AIMLAB_VR_DataStreamer.exe` | `AIMLAB_VR_DataStreamer_v6.2.exe` |
| **Size** | 1.93 MB | 1.97 MB |
| **Progress Bars** | ❌ | ✅ |
| **Status Messages** | ❌ | ✅ |
| **Use For** | Production | Development/Demo |

## 🔨 Build Commands

```batch
# Build v6.1
.\build.bat

# Build v6.2
.\build_v6.2.bat
```

## ▶️ Run Commands

```batch
# Run v6.1
.\AIMLAB_VR_DataStreamer.exe

# Run v6.2
.\AIMLAB_VR_DataStreamer_v6.2.exe
```

## 📖 Documentation

| Document | Purpose |
|----------|---------|
| `README_VR_DATA_STREAMER.md` | General documentation |
| `README_v6.2.md` | v6.2 specific features |
| `V6.1_VS_V6.2_COMPARISON.md` | Detailed comparison |
| `QUICKSTART_VR_STREAMER.md` | 5-minute quick start |
| `BUGFIX_v6.1.md` | Bug fixes in v6.1 |
| `V6.2_IMPLEMENTATION_COMPLETE.md` | Implementation summary |

## ⚡ Quick Decision

**Use v6.1 if:** You want minimal UI and proven stability

**Use v6.2 if:** You want progress feedback and status messages

**Can't decide?** Try both - they can coexist!

## 🎯 Key Features (Both Versions)

- ✅ Arduino motor control
- ✅ Unity VR streaming
- ✅ CSV data logging
- ✅ Auto-discovery
- ✅ Manual refresh
- ✅ Debug console
- ✅ NYU purple UI

## 🆘 Quick Help

**No devices found?**
- Check Device Manager for COM ports
- Ensure Unity is running and broadcasting
- Try 30-second manual refresh (v6.2)

**Console not showing?**
- Should auto-open on startup
- Check Windows console settings
- Try running from command prompt

**Build failed?**
- Run `download_sdl2.ps1` first
- Run `download_imgui_sdl2_backends.ps1` first
- Ensure MinGW-w64 in PATH

## 📧 Contact

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Lab:** AIMLAB, NYU Abu Dhabi

---

**Both versions: ✅ Production Ready**

