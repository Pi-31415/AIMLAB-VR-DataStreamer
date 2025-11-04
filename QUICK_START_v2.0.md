# AIMLAB VR Data Collector - Quick Start v2.0

**Author:** Pi Ko (pi.ko@nyu.edu) | **Date:** 04 November 2025

---

## 🚀 ULTRA QUICK START

```powershell
# Run portable version
cd dist
.\AIMLAB-VR-DataCollector-Portable.exe

# 1. Click "Connect to Unity"
# 2. Start your Unity VR app
# 3. Enter filename → Click "Start Recording"
```

---

## 📦 NEW EXECUTABLES (v2.0)

| File | Size | Location |
|------|------|----------|
| **Installer** | 72.25 MB | `dist\AIMLAB VR Data Collector Setup 2.0.0.exe` |
| **Portable** | 72.03 MB | `dist\AIMLAB-VR-DataCollector-Portable.exe` |

---

## 🎯 What's New in v2.0

✅ **Unity UDP Connection** (port 8888)  
✅ **Arduino Serial Control** (9600 baud)  
✅ **CSV Data Recording** (data/ folder)  
✅ **Minimal Clean UI**  
✅ **Debug Console**  
✅ **Live Data Stream**  

---

## 🔌 Unity Setup Required

Add to your Unity C# script:

```csharp
// Send handshake every second
UdpClient udp = new UdpClient();
IPEndPoint endPoint = new IPEndPoint(IPAddress.Broadcast, 8888);

// Handshake
byte[] data = Encoding.UTF8.GetBytes("HANDSHAKE");
udp.Send(data, data.Length, endPoint);

// VR Data (CSV format)
string vrData = $"{time},{posX},{posY},{posZ},{rotX},{rotY},{rotZ},{rotW},{trigger},{grip},{btn1},{btn2}";
byte[] bytes = Encoding.UTF8.GetBytes(vrData);
udp.Send(bytes, bytes.Length, endPoint);
```

---

## 🤖 Arduino Setup Required

```cpp
void setup() {
    Serial.begin(9600);
    pinMode(9, OUTPUT); // Motor pin
    Serial.println("Vibration Motor Controller Ready");
}

void loop() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == "1") {
            digitalWrite(9, HIGH);
            delay(100);
            digitalWrite(9, LOW);
        }
    }
}
```

---

## 🎮 Usage Steps

### 1. Connect to Unity
- Click "Connect to Unity"
- Start Unity VR application
- Wait for green ● status
- Look for "Unity connected" in log

### 2. Record Data
- Enter filename (no .csv)
- Click "Start Recording"
- 🔴 Recording indicator appears
- Perform VR actions
- Click "Stop Recording"
- File saved to `data/filename.csv`

### 3. Control Arduino
- Connect Arduino via USB
- Click "Connect Arduino"
- Wait for green ● status
- Click "Test Motor (1)" to vibrate

---

## 📊 CSV Data Format

13 columns:
```
Timestamp, Position_X, Position_Y, Position_Z,
Rotation_X, Rotation_Y, Rotation_Z, Rotation_W,
Trigger_Value, Grip_Value,
Primary_Button, Secondary_Button, Received_At
```

---

## ⌨️ Keyboard Shortcuts

- `Ctrl+R` → Start/Stop Recording
- `Ctrl+L` → Clear Debug Log

---

## 🐛 Troubleshooting

### Unity not connecting?
- Check firewall (allow UDP 8888)
- Verify same network
- Ensure Unity sends "HANDSHAKE"

### Arduino not found?
- Install Arduino drivers
- Check USB connection
- Try different COM port

### Recording disabled?
- Ensure Unity connected (green)
- Enter valid filename

---

## 📁 File Locations

- **CSV Files**: `data/`
- **Source**: Root directory
- **Executables**: `dist/`
- **Documentation**: `DATA_COLLECTOR_README.md`

---

## 📚 Full Documentation

- **Complete Guide**: `DATA_COLLECTOR_README.md`
- **Update Summary**: `UPDATE_SUMMARY_v2.0.md`
- **Build Guide**: `BUILD_GUIDE.md`

---

## ✅ Status

- [x] All features implemented
- [x] Executables built (v2.0)
- [x] No linter errors
- [x] Ready for data collection

---

**Version**: 2.0.0 | **Status**: ✅ Production Ready

