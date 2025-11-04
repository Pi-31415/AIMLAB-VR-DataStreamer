# 🚀 Quick Start Guide - AIMLAB VR Streamer Electron App

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## ⚡ Fastest Way to Run

### Windows PowerShell
```powershell
.\start_electron.bat
```

**OR**

```powershell
npm start
```

That's it! The application will launch automatically.

---

## 📋 Prerequisites

- ✅ **Node.js** installed (any recent version)
- ✅ **npm** (comes with Node.js)
- ✅ Windows 10/11 (or macOS/Linux with minor path adjustments)

---

## 🎯 First Time Setup

If you're running for the first time:

1. **Check Node.js is installed:**
   ```powershell
   node --version
   ```

2. **Install dependencies** (if not already done):
   ```powershell
   npm install
   ```

3. **Run the app:**
   ```powershell
   npm start
   ```

---

## 🎨 What You'll See

When the app launches, you'll see:

1. **Purple Header** with "AIMLAB VR Streamer" title
2. **Connection Settings** panel with IP/Port inputs
3. **Streaming Controls** with Start/Stop buttons
4. **Progress Bars** showing data transmission
5. **Statistics Dashboard** with real-time counters
6. **Activity Log** showing application events

---

## 🔧 How to Use

### Step 1: Connect
1. Enter IP Address (default: `127.0.0.1`)
2. Enter Port (default: `8888`)
3. Click **Connect** button
4. Watch status indicator turn green ✅

### Step 2: Start Streaming
1. Click **Start Streaming** button
2. Watch progress bars animate
3. See statistics update in real-time
4. Monitor activity log for events

### Step 3: Stop Streaming
1. Click **Stop Streaming** when done
2. Click **Disconnect** to close connection

---

## 📁 Project Files

```
AIMLAB-VR-DataStreamer/
├── main.js              ← Main Electron process
├── preload.js           ← IPC bridge (security)
├── index.html           ← UI structure
├── styles.css           ← NYU purple theme
├── renderer.js          ← UI interactions
├── package.json         ← Dependencies
└── start_electron.bat   ← Quick launch script
```

---

## 🎨 Theme Colors

- **NYU Purple**: `#57068c` (main color)
- **White Background**: `#FFFFFF` (solid white)
- **Success**: Green `#4CAF50`
- **Danger**: Red `#F44336`
- **Info**: Blue `#2196F3`

---

## 🐛 Troubleshooting

### Problem: "npm not found"
**Solution**: Install Node.js from https://nodejs.org/

### Problem: "Cannot find module 'electron'"
**Solution**: Run `npm install` first

### Problem: Window doesn't open
**Solution**: Check console output, try `npm run dev` for logs

### Problem: Port already in use
**Solution**: Change port number in UI or use different port

---

## 💡 Tips

1. **Development Mode**: Use `npm run dev` to see console logs
2. **Refresh**: Press `Ctrl+R` to reload the UI
3. **DevTools**: Uncomment line in `main.js` to open DevTools
4. **Customize**: Edit `styles.css` to change colors/layout

---

## 🔗 Integration Ready

The app is ready to integrate with:
- ✅ C++ backend (UDP/TCP)
- ✅ Serial port communication
- ✅ Unity VR applications
- ✅ Custom data streaming protocols

Edit the IPC handlers in `main.js` to connect your backend!

---

## 📚 Documentation

- **Complete Guide**: See `README_ELECTRON.md`
- **UI Layout**: See `UI_LAYOUT_REFERENCE.md`
- **Summary**: See `ELECTRON_APP_SUMMARY.md`

---

## 🎓 NYU Branding

This application follows NYU visual identity guidelines:
- NYU Violet (#57068c) as primary color
- Clean, professional white background
- Modern, accessible UI design

---

## ✉️ Support

**Author**: Pi Ko  
**Email**: pi.ko@nyu.edu  
**Project**: AIMLAB VR Research - NYU

---

## ✨ Enjoy Your App!

You now have a fully functional Electron application ready for VR data streaming! 

**Next Steps:**
1. Test the UI and familiarize yourself with controls
2. Integrate with your VR backend
3. Customize features as needed

Happy coding! 🎉

