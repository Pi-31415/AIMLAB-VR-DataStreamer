# AIMLAB VR Data Collector - Quick Start v2.3

**Version:** 2.3.0 | **Date:** 04 November 2025

---

## ⚡ QUICK START

```powershell
# Run the app
npm start

# 1. Click "Connect to Unity"
# 2. Start Unity VR application
# 3. Data should connect automatically!
```

---

## 🔧 IMPORTANT: Port Configuration

### ✅ Correct Port (v2.3):
- **Electron listens on**: Port **45101**
- **Unity broadcasts to**: Port **45101**
- **Status**: ✅ **MATCHED!**

### ❌ Old Configuration (v2.2 and earlier):
- Electron was on port 8888 (WRONG!)
- Unity was on port 45101
- **Status**: ❌ **MISMATCH**

---

## 📡 Unity Ports

Unity uses **two ports**:

| Port | Purpose | Status in Electron |
|------|---------|-------------------|
| **45000** | Discovery/Handshake | ℹ️ Not implemented (optional) |
| **45101** | VR Data Stream | ✅ **Listening** |

**Note**: Handshake on port 45000 is optional. Unity sends data to 45101 regardless.

---

## 🚀 Connection Steps

### 1. Start Electron
```powershell
npm start
```

### 2. Connect to Unity
- Click "Connect to Unity" button
- See: `UDP server ready on port 45101`

### 3. Start Unity VR App
- Unity starts broadcasting to port 45101
- Electron receives data immediately
- Status turns ● **Green**

### 4. Start Recording (Optional)
- Enter filename: `my_vr_session`
- Click "Start Recording"
- Perform VR actions
- Click "Stop Recording"
- File saved: `data/my_vr_session.csv`

---

## 🐛 Troubleshooting

### Unity Not Connecting?

**Check Port**:
```powershell
# Verify Electron is listening on 45101
netstat -an | findstr "45101"
# Should show: UDP 0.0.0.0:45101
```

**Check Firewall**:
```powershell
# Allow UDP port 45101
New-NetFirewallRule -DisplayName "Unity VR Port 45101" `
  -Direction Inbound -Protocol UDP -LocalPort 45101 -Action Allow
```

**Verify Unity Port**:
```csharp
// In Unity AIMLABVRDataStreamer.cs:
private const int dataPort = 45101;  // Should match Electron
```

---

## 📊 What's New in v2.3

| Change | Old (v2.2) | New (v2.3) |
|--------|-----------|-----------|
| **Electron Port** | 8888 | **45101** ✅ |
| **Unity Port** | 45101 | 45101 |
| **Match Status** | ❌ Mismatch | ✅ **MATCHED** |

---

## ✅ Status

- [x] Port mismatch fixed (8888 → 45101)
- [x] Matches Unity configuration
- [x] Ready for VR data collection
- [x] All other features working

---

## 📞 Support

**Email**: pi.ko@nyu.edu  
**Version**: 2.3.0  
**Status**: ✅ Production Ready

---

**🎉 Ports now match - Unity connection should work!** 🎉

