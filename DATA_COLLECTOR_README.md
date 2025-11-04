# AIMLAB VR Data Collector - Documentation

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v2.0

---

## 🎉 UPDATE COMPLETE!

The Electron app has been successfully transformed into a minimal VR data collection interface!

---

## 📦 What Changed - Version 2.0

### Major Updates:
1. **Unity UDP Connection** - Receives VR data via UDP broadcast on port 8888
2. **Arduino Serial Communication** - Controls vibration motor via serial (9600 baud)
3. **CSV Recording** - Saves VR data to timestamped CSV files
4. **Minimal UI** - Clean, focused interface for data collection
5. **Debug Console** - Real-time logging and monitoring
6. **Connection Handshakes** - Robust connection verification

---

## 🚀 Quick Start

### Method 1: Run Portable (No Installation)
```powershell
cd dist
.\AIMLAB-VR-DataCollector-Portable.exe
```

### Method 2: Install Full Version
```powershell
cd dist
.\AIMLAB VR Data Collector Setup 2.0.0.exe
```

### Method 3: Development Mode
```powershell
npm start
```

---

## 📋 Features

### 1. Unity VR Data Reception
- **Protocol**: UDP broadcast
- **Port**: 8888
- **Format**: CSV string (timestamp,posX,posY,posZ,rotX,rotY,rotZ,rotW,trigger,grip,button1,button2)
- **Handshake**: Expects "HANDSHAKE" messages from Unity
- **Timeout**: 5 seconds without data triggers disconnection

### 2. CSV Recording
- **Location**: `data/` directory
- **Format**: CSV with headers
- **Batching**: Writes in batches of 10 for performance
- **Auto-naming**: Appends numbers if filename exists (file.csv, file_1.csv, etc.)

### 3. Arduino Motor Control
- **Baud Rate**: 9600
- **Auto-detection**: Finds Arduino automatically
- **Handshake**: Sends "HELLO", expects "Vibration Motor Controller Ready"
- **Test Command**: Sends "1\n" to trigger vibration

---

## 🎯 Data Format

### CSV Columns:
| Column | Description |
|--------|-------------|
| `Timestamp` | Unity timestamp (float) |
| `Position_X` | X position (float) |
| `Position_Y` | Y position (float) |
| `Position_Z` | Z position (float) |
| `Rotation_X` | X rotation (quaternion) |
| `Rotation_Y` | Y rotation (quaternion) |
| `Rotation_Z` | Z rotation (quaternion) |
| `Rotation_W` | W rotation (quaternion) |
| `Trigger_Value` | Trigger press value (0-1) |
| `Grip_Value` | Grip press value (0-1) |
| `Primary_Button` | Primary button state (true/false) |
| `Secondary_Button` | Secondary button state (true/false) |
| `Received_At` | ISO timestamp when data received |

---

## 🔧 Setup Instructions

### Step 1: Unity Configuration

Your Unity VR application needs to send UDP data on port 8888:

```csharp
// Unity C# Example
using System.Net.Sockets;
using System.Text;

UdpClient udpClient = new UdpClient();
IPEndPoint endPoint = new IPEndPoint(IPAddress.Broadcast, 8888);

// Send handshake periodically (every second)
string handshake = "HANDSHAKE";
byte[] handshakeData = Encoding.UTF8.GetBytes(handshake);
udpClient.Send(handshakeData, handshakeData.Length, endPoint);

// Send VR data format:
string data = $"{timestamp},{posX},{posY},{posZ},{rotX},{rotY},{rotZ},{rotW},{trigger},{grip},{button1},{button2}";
byte[] dataBytes = Encoding.UTF8.GetBytes(data);
udpClient.Send(dataBytes, dataBytes.Length, endPoint);
```

### Step 2: Arduino Setup

Upload vibration motor controller code to Arduino:

```cpp
// Arduino sketch
const int motorPin = 9;

void setup() {
    Serial.begin(9600);
    pinMode(motorPin, OUTPUT);
    Serial.println("Vibration Motor Controller Ready");
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        
        if (command == "HELLO") {
            Serial.println("Vibration Motor Controller Ready");
        } else if (command == "1") {
            // Vibrate motor
            digitalWrite(motorPin, HIGH);
            delay(100);
            digitalWrite(motorPin, LOW);
            Serial.println("Motor activated");
        }
    }
}
```

### Step 3: Network Configuration

- Ensure Unity PC and Electron app are on the **same network**
- Unity broadcasts to `255.255.255.255:8888` (all devices)
- May need to configure **firewall** to allow UDP port 8888
- Windows Firewall: Allow incoming UDP on port 8888

---

## 📱 User Interface Guide

### Status Bar
- **Unity**: ● Green = Connected | ● Red = Disconnected
- **Arduino**: ● Green = Connected | ● Red = Disconnected

### Control Panels

#### Unity Connection
1. Click "Connect to Unity" - starts UDP listener on port 8888
2. Start Unity VR application
3. Status should turn green when handshake received
4. Click "Refresh" to restart connection

#### Data Recording
1. Enter filename (without .csv extension)
2. Ensure Unity is connected (green status)
3. Click "Start Recording"
4. Recording indicator shows: 🔴 Recording to filename.csv
5. Click "Stop Recording" when done
6. File saved to `data/filename.csv`

#### Vibration Motor
1. Connect Arduino via USB
2. Click "Connect Arduino"
3. Wait for connection confirmation
4. Click "Test Motor (1)" to test vibration
5. Click "Refresh" to reconnect

### Debug Console
- Shows all system events
- Color-coded messages:
  - **Blue**: Info messages
  - **Green**: Success messages
  - **Red**: Error messages
  - **Orange**: Warning messages
  - **Cyan**: Arduino messages
- Click "Clear" to clear log

### Live Data Stream
- Displays most recent VR data received
- Updates in real-time
- Shows position, rotation, button states

---

## 🎹 Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+R` | Start/Stop Recording |
| `Ctrl+L` | Clear Debug Log |

---

## 🐛 Troubleshooting

### Unity Not Connecting

**Problem**: Status stays red after clicking "Connect to Unity"

**Solutions**:
1. Check if Unity is running and sending UDP data
2. Verify firewall allows UDP port 8888
3. Ensure both devices on same network
4. Check Unity is sending "HANDSHAKE" messages
5. Try "Refresh" button

### Arduino Not Detected

**Problem**: "No serial ports found" error

**Solutions**:
1. Check Arduino is plugged in via USB
2. Install Arduino drivers if needed
3. Check Device Manager for COM port
4. Try different USB port
5. Restart application

### Recording Not Starting

**Problem**: "Start Recording" button stays disabled

**Solutions**:
1. Ensure Unity is connected (green status)
2. Enter a valid filename
3. Check `data/` directory exists and is writable

### No Data Appearing

**Problem**: Unity connected but no data in live stream

**Solutions**:
1. Verify Unity is sending data in correct format
2. Check Unity console for errors
3. Ensure data format matches expected CSV format
4. Look for parse errors in debug console

---

## 📊 Performance Notes

- **Batching**: Data written to CSV in batches of 10 for performance
- **Memory**: Data buffer cleared after each batch write
- **Timeout**: Unity connection times out after 5 seconds of no data
- **File Size**: Typical recording generates ~10-50 KB per minute

---

## 🔒 Security Notes

- Application uses context isolation for security
- No remote code execution
- All data stored locally
- No internet connection required
- IPC communication is sandboxed

---

## 📁 File Locations

### Application Files
```
project/
├── main.js                  ← Main process (UDP & Serial)
├── preload.js               ← IPC bridge
├── renderer.js              ← UI logic
├── index.html               ← UI structure
├── styles.css               ← Minimal NYU theme
├── package.json             ← Dependencies
└── data/                    ← CSV files saved here
```

### Recorded Data
- **Development**: `data/filename.csv`
- **Installed**: `%LOCALAPPDATA%/aimlab-vr-streamer/data/filename.csv`

---

## 🔄 Building from Source

### Install Dependencies
```powershell
npm install
```

### Run in Development
```powershell
npm start
```

### Build Executables
```powershell
npm run build
```

### Outputs
- Installer: `dist/AIMLAB VR Data Collector Setup 2.0.0.exe` (72.25 MB)
- Portable: `dist/AIMLAB-VR-DataCollector-Portable.exe` (72.03 MB)

---

## 📝 Dependencies

### Runtime
- **electron**: ^27.0.0
- **serialport**: ^13.0.0
- **csv-writer**: ^1.6.0
- **dgram**: Built-in (UDP)

### Development
- **electron-builder**: ^26.0.12

---

## 🧪 Testing Procedure

### 1. Test Unity Connection
- [ ] Click "Connect to Unity"
- [ ] Start Unity VR application
- [ ] Verify status turns green
- [ ] Check debug console for connection message
- [ ] Verify live data stream updates

### 2. Test Arduino Connection
- [ ] Connect Arduino via USB
- [ ] Click "Connect Arduino"
- [ ] Verify status turns green
- [ ] Click "Test Motor (1)"
- [ ] Verify motor vibrates
- [ ] Check debug console for Arduino responses

### 3. Test CSV Recording
- [ ] Enter filename "test_recording"
- [ ] Click "Start Recording"
- [ ] Perform VR actions in Unity
- [ ] Click "Stop Recording"
- [ ] Verify `data/test_recording.csv` exists
- [ ] Open CSV and verify data format

### 4. Test Error Handling
- [ ] Try recording without Unity connected
- [ ] Try connecting Arduino when none plugged in
- [ ] Try starting recording without filename
- [ ] Verify appropriate error messages

---

## 🎨 UI Theme

- **Primary Color**: NYU Purple (#57068C)
- **Background**: Solid White (#FFFFFF)
- **Success**: Green (#4CAF50)
- **Error**: Red (#f44336)
- **Console**: Dark theme (#1e1e1e)

---

## 📞 Support

For issues or questions:
- **Email**: pi.ko@nyu.edu
- **Project**: NYU AIMLAB VR Research

---

## ✅ Verification Checklist

- [x] UDP server listens on port 8888
- [x] Serial communication at 9600 baud
- [x] CSV writing with batching
- [x] Connection handshakes implemented
- [x] Timeout monitoring active
- [x] Minimal UI complete
- [x] Debug console functional
- [x] Live data preview working
- [x] Executables built successfully
- [x] All dependencies installed

---

## 🎉 Success!

Your AIMLAB VR Data Collector is ready to use!

**Next Steps:**
1. Test with your Unity VR application
2. Connect Arduino vibration motor
3. Start collecting VR data
4. Analyze CSV files for research

---

*Last Updated: 04 November 2025*

