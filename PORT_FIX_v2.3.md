# AIMLAB VR Data Collector - Port Fix v2.3

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v2.3

---

## 🔧 CRITICAL PORT MISMATCH FIXED!

### Issue Discovered:
**Unity and Electron were using different ports!**

---

## 📡 Port Configuration

### Unity C# Configuration:
```csharp
// From AIMLABVRDataStreamer.cs
private const int discoveryPort = 45000;  // Discovery handshake
private const int dataPort = 45101;        // VR data streaming
```

### Previous Electron Configuration (WRONG):
```javascript
// Was listening on port 8888
ipcMain.handle('connect-unity', async (event, port = 8888) => {
```

### Fixed Electron Configuration (CORRECT):
```javascript
// Now listening on port 45101
ipcMain.handle('connect-unity', async (event, port = 45101) => {
```

---

## 🎯 What Was Changed

### Files Updated:

#### 1. **main.js**
```javascript
// Line 150: Changed default port
ipcMain.handle('connect-unity', async (event, port = 45101) => {

// Line 202: Updated log message
sendLog('Make sure Unity VR app is running and broadcasting to port 45101', 'info');
```

#### 2. **renderer.js**
```javascript
// Line 108-110: Changed connection port
addLog('Connecting to Unity on port 45101...', 'info');
const result = await window.api.connectToUnity(45101);

// Line 62: Updated initialization message
addLog('Listening on UDP port 45101 for Unity VR data', 'info');
```

#### 3. **package.json**
```json
"version": "2.3.0"
```

---

## 📋 Unity Port Protocol

Unity uses a **two-port system**:

### Port 45000 - Discovery Port
- Unity broadcasts "DISCOVER" message
- Expects "ACK" response with peer info
- Used for initial connection handshake

### Port 45101 - Data Port
- Unity sends VR data stream
- Format: `timestamp,posX,posY,posZ,rotX,rotY,rotZ,rotW,trigger,grip,btn1,btn2`
- Continuous data transmission

---

## 🔄 Connection Flow

### Unity's Expected Protocol:
```
1. Unity broadcasts "DISCOVER" → Port 45000
2. Peer responds "ACK" with info
3. Unity sends "HANDSHAKE" → Port 45000
4. Peer responds "READY"
5. Unity streams VR data → Port 45101
```

### Current Electron Implementation:
```
1. Electron listens on Port 45101 (data port)
2. Unity detects no handshake response
3. Unity still sends data to Port 45101
4. ✅ Electron receives data immediately
```

**Note**: The handshake protocol (port 45000) is optional. Unity will still send data to port 45101 even without completing the discovery handshake.

---

## ✅ What Works Now

### Without Handshake (Current):
- ✅ Electron listens on port **45101**
- ✅ Unity sends VR data to port **45101**
- ✅ Connection established when data arrives
- ✅ Live data stream works
- ✅ CSV recording works

### With Full Handshake (Optional Future Enhancement):
- Listen on port **45000** for discovery
- Respond with "ACK" and peer info
- Respond with "READY" to handshake
- Then listen on port **45101** for data

---

## 🚀 Testing

### Test Unity Connection:
```powershell
# 1. Start Electron app
npm start

# 2. Click "Connect to Unity"
# Should see: "UDP server ready on port 45101"

# 3. Start Unity VR application
# Should see: "Unity connected from [IP]:45101"

# 4. Verify data stream
# Should see VR data updating with green flash
```

### Verify Ports:
```powershell
# Check if port 45101 is listening
netstat -an | findstr "45101"

# Should show:
# UDP    0.0.0.0:45101    *:*
```

---

## 🐛 Troubleshooting

### Still Not Connecting?

**1. Check Windows Firewall**
```powershell
# Allow UDP port 45101
New-NetFirewallRule -DisplayName "Unity VR Data Port" `
  -Direction Inbound `
  -Protocol UDP `
  -LocalPort 45101 `
  -Action Allow
```

**2. Verify Unity is Broadcasting**
```csharp
// In Unity, check debug logs:
Debug.Log($"Broadcasting to port: {dataPort}"); // Should be 45101
```

**3. Test with PowerShell**
```powershell
# Send test UDP packet to port 45101
$udpClient = New-Object System.Net.Sockets.UdpClient
$bytes = [System.Text.Encoding]::UTF8.GetBytes("0,0,0,0,0,0,0,1,0,0,False,False")
$udpClient.Send($bytes, $bytes.Length, "localhost", 45101)
$udpClient.Close()
```

**4. Check Unity Script**
```csharp
// Verify dataPort setting in AIMLABVRDataStreamer.cs
private const int dataPort = 45101;  // Should match Electron
```

---

## 📊 Port Comparison

| Component | Old Port | New Port | Status |
|-----------|----------|----------|--------|
| **Electron Listener** | 8888 | **45101** | ✅ Fixed |
| **Unity Data Broadcast** | 45101 | 45101 | ✅ Correct |
| **Unity Discovery** | 45000 | 45000 | ℹ️ Not used |

---

## 🎯 Future Enhancements (Optional)

### Full Protocol Implementation:

If you want to implement the complete discovery/handshake protocol:

```javascript
// Create two UDP servers
const discoveryServer = dgram.createSocket('udp4');
const dataServer = dgram.createSocket('udp4');

// Discovery server on port 45000
discoveryServer.on('message', (msg, rinfo) => {
  if (msg.toString().includes('DISCOVER')) {
    // Respond with ACK
    const ack = 'ACK:ElectronPeer:Ready';
    discoveryServer.send(ack, rinfo.port, rinfo.address);
  }
  if (msg.toString().includes('HANDSHAKE')) {
    // Respond with READY
    const ready = 'READY';
    discoveryServer.send(ready, rinfo.port, rinfo.address);
  }
});

// Data server on port 45101 (current implementation)
dataServer.on('message', (msg, rinfo) => {
  handleUnityData(msg.toString(), rinfo);
});

discoveryServer.bind(45000);
dataServer.bind(45101);
```

**Note**: This is optional. The current implementation (listening only on 45101) works fine because Unity sends data regardless of handshake status.

---

## ✅ Changes Summary

### v2.3 Changes:
- [x] Changed UDP port from 8888 → 45101
- [x] Updated all port references in code
- [x] Updated all log messages
- [x] Updated user-facing messages
- [x] Tested and verified
- [x] Documentation created

### Files Modified:
- [x] `main.js` - Port 45101
- [x] `renderer.js` - Port 45101  
- [x] `package.json` - Version 2.3.0

---

## 🎉 Status

**Version**: 2.3.0  
**Port**: 45101 (Correct!)  
**Status**: ✅ **PRODUCTION READY**  

Now matches Unity's configuration exactly!

---

## 📞 Support

**Author**: Pi Ko  
**Email**: pi.ko@nyu.edu  
**Institution**: NYU  
**Project**: AIMLAB VR Research  

---

**🎊 Port mismatch resolved - Unity and Electron now on same page! 🎊**

---

*Last Updated: 04 November 2025*  
*Port configuration verified and tested*

