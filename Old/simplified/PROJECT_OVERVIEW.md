# AIMLAB VR Data Streamer - Project Overview

**Project:** AIMLAB VR Data Streamer Console Application  
**Institution:** NYU Abu Dhabi  
**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## 📋 Project Summary

The AIMLAB VR Data Streamer is a comprehensive system for conducting VR experiments with synchronized haptic feedback and precise data recording. The system consists of three main components:

1. **Windows Console Application** - Controls experiment flow and records data
2. **Arduino Vibration Controller** - Provides haptic feedback via serial communication
3. **Unity VR Application** - Streams real-time tracking data from VR headset

---

## 🎯 Project Goals

- **Real-time VR Tracking**: Capture head and controller positions/rotations at 90Hz
- **Haptic Feedback Control**: Trigger vibration motors for experimental stimuli
- **Data Recording**: Export tracking data to CSV for post-experiment analysis
- **Easy Operation**: Simple console interface for experimenters
- **Network Discovery**: Automatic detection of VR devices on local network
- **Reliability**: Robust error handling and connection management

---

## 📁 Project Structure

```
simplified/
│
├── main.cpp                              # Console application source code
├── build.bat                             # Windows build script
├── AIMLAB_Console.exe                    # Compiled executable
│
├── Arduino_Vibration_Controller.ino      # Arduino firmware
│
├── Unity_VRDataStreamer.cs               # Unity C# script
│
├── analyze_vr_data.py                    # Python data analysis script
├── analyze.bat                           # Analysis launcher
├── requirements.txt                      # Python dependencies
│
├── README.md                             # Main documentation
├── QUICK_START.md                        # Quick start guide
├── SETUP_GUIDE.md                        # Detailed setup instructions
└── PROJECT_OVERVIEW.md                   # This file
```

---

## 🔧 System Architecture

### Component Interaction

```
┌─────────────────┐         USB Serial          ┌──────────────┐
│                 │ ◄────────────────────────── │              │
│  Arduino Board  │                              │   Windows    │
│  (Vibration     │                              │   Console    │
│   Motor)        │                              │   App        │
│                 │                              │              │
└─────────────────┘                              │              │
                                                 │              │
┌─────────────────┐    TCP/IP (Port 55000)      │              │
│                 │ ◄────────────────────────── │              │
│  Unity VR       │                              │              │
│  (Quest/Vive/   │    UDP (Port 55001)         │              │
│   Any XR)       │ ─────────────────────────► │              │
│                 │    [Discovery Broadcast]     │              │
└─────────────────┘                              └──────────────┘
```

### Data Flow

```
VR Headset → Unity Script → Network Stream → Console App → CSV File
                                                    ↓
                                            [Analysis Script]
                                                    ↓
                                      Graphs + Statistics
```

---

## 🌐 Network Protocol

### Discovery Phase (UDP)
1. Unity broadcasts: `"VR_HEADSET_DISCOVERY"` on port 55001
2. Console app listens on port 55001
3. Console app receives broadcast and extracts sender IP
4. Console app initiates TCP connection

### Connection Phase (TCP)
1. Console app connects to Unity on port 55000
2. Console app sends: `"DATA_RECEIVER_READY"`
3. Unity responds: `"VR_HEADSET_READY"`
4. Connection established

### Data Streaming Phase (TCP)
1. Unity sends VR data at configured rate (default 90Hz)
2. Data format: CSV line with 21 values
   ```
   HeadPosX,HeadPosY,HeadPosZ,HeadRotX,HeadRotY,HeadRotZ,HeadRotW,
   LeftHandPosX,LeftHandPosY,LeftHandPosZ,LeftHandRotX,LeftHandRotY,LeftHandRotZ,LeftHandRotW,
   RightHandPosX,RightHandPosY,RightHandPosZ,RightHandRotX,RightHandRotY,RightHandRotZ,RightHandRotW
   ```
3. Console app adds timestamp and writes to CSV

---

## 🔌 Serial Protocol (Arduino)

### Handshake
```
PC → Arduino: "HELLO\r\n"
Arduino → PC: "Vibration Motor Controller Ready\n"
```

### Commands
- `'1'` - Activate vibration (500ms pulse)
- `'0'` - Stop vibration immediately
- Other - Echo back as unknown command

### Configuration
- **Baud Rate**: 9600
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Flow Control**: Hardware (DTR/RTS enabled)

---

## 📊 Data Format

### CSV Output Structure

| Column | Description | Units | Example |
|--------|-------------|-------|---------|
| Timestamp | Milliseconds since recording start | ms | 1234 |
| HeadPosX | Head X position | meters | 0.123 |
| HeadPosY | Head Y position | meters | 1.650 |
| HeadPosZ | Head Z position | meters | -0.456 |
| HeadRotX | Head rotation (quaternion X) | - | 0.000 |
| HeadRotY | Head rotation (quaternion Y) | - | 0.707 |
| HeadRotZ | Head rotation (quaternion Z) | - | 0.000 |
| HeadRotW | Head rotation (quaternion W) | - | 0.707 |
| LeftHandPos* | Left controller position (X,Y,Z) | meters | ... |
| LeftHandRot* | Left controller rotation (X,Y,Z,W) | - | ... |
| RightHandPos* | Right controller position (X,Y,Z) | meters | ... |
| RightHandRot* | Right controller rotation (X,Y,Z,W) | - | ... |

### Coordinate System
- Unity uses left-handed coordinate system
- +X: Right
- +Y: Up
- +Z: Forward
- Rotations: Quaternions (X, Y, Z, W)

---

## 🚀 Typical Workflow

### Experiment Setup (5 minutes)
1. Build console application: `build.bat`
2. Upload Arduino firmware
3. Deploy Unity VR application to headset
4. Connect Arduino via USB
5. Start Unity VR app
6. Run `AIMLAB_Console.exe`
7. Wait for auto-connect (both devices)

### Running Experiment (variable)
1. Press `3` to test vibration (verify Arduino works)
2. Press `5` to start recording
3. Enter experiment filename
4. Subject performs VR task
5. Experimenter triggers vibration as needed (press `3`)
6. Press `6` to stop recording
7. Repeat for multiple trials/subjects

### Data Analysis (5-10 minutes)
1. Run analysis: `analyze.bat experiment_01.csv`
2. Review generated graphs and statistics
3. Import CSV into R, Python, or MATLAB for custom analysis

---

## 📈 Performance Specifications

### Data Acquisition
- **Sampling Rate**: Up to 120 Hz (configurable)
- **Recommended Rate**: 90 Hz for VR
- **Latency**: < 20ms network latency typical
- **Data Loss**: None (TCP ensures delivery)

### Hardware Requirements
- **Arduino**: Any model with Serial support (Uno, Nano, Mega)
- **PC**: Windows 10/11, 4GB RAM minimum
- **Network**: Local network (WiFi or Ethernet)
- **VR Headset**: Any Unity-supported XR device

### Storage
- **Data Size**: ~2.5 KB per second at 90Hz
- **Example**: 10-minute session = ~1.5 MB CSV file

---

## 🔬 Use Cases

### Research Applications
1. **Spatial Memory Studies**: Track head movement patterns in virtual environments
2. **Hand-Eye Coordination**: Analyze controller trajectories during tasks
3. **Haptic Feedback Research**: Correlate vibration timing with behavioral responses
4. **Presence Studies**: Measure body movement as presence indicator
5. **Training Simulations**: Record user performance metrics

### Experiment Types
- Target acquisition tasks
- Navigation studies
- Reaction time experiments
- Motor learning studies
- Multi-modal integration research

---

## 🛠️ Customization Options

### Console Application
```cpp
// In main.cpp

// Change network ports
const int VR_PORT = 55000;
const int DISCOVERY_PORT = 55001;

// Change discovery timeout
const int DISCOVERY_TIMEOUT = 10;  // seconds
```

### Arduino Controller
```cpp
// In Arduino_Vibration_Controller.ino

// Change vibration duration
const int VIBRATION_DURATION = 500;  // milliseconds

// Change motor intensity
const int MOTOR_INTENSITY = 255;  // 0-255 (PWM)

// Change motor pin
const int MOTOR_PIN = 9;
```

### Unity Streamer
```csharp
// In Unity_VRDataStreamer.cs

// Change stream rate
public int streamRate = 90;  // Hz

// Change network ports
public int dataPort = 55000;
public int discoveryPort = 55001;
```

---

## 📚 Dependencies

### Console Application
- **Compiler**: MinGW-w64 (g++) or MSVC
- **Standard**: C++17
- **Libraries**: Winsock2 (ws2_32.lib)
- **OS**: Windows 10/11

### Arduino Firmware
- **IDE**: Arduino IDE 1.8.x or 2.x
- **Board**: AVR-based Arduino (Uno/Nano/Mega)
- **Libraries**: None (uses built-in Serial)

### Unity VR Application
- **Unity Version**: 2020.3 LTS or newer
- **Packages**: XR Plugin Management, specific XR SDK
- **Scripting Backend**: Mono or IL2CPP
- **Platform**: Standalone or Android (Quest)

### Analysis Script
- **Python**: 3.7 or later
- **Packages**: pandas, numpy, matplotlib
- **OS**: Cross-platform (Windows/Linux/macOS)

---

## 🐛 Known Issues and Limitations

### Current Limitations
1. **Single Client**: Console app supports one VR device at a time
2. **Windows Only**: Console app is Windows-specific (Winsock)
3. **No Event Markers**: Must manually sync events with CSV timestamps
4. **Fixed Data Format**: CSV columns are predetermined

### Potential Improvements
- [ ] Multi-headset support for group experiments
- [ ] Cross-platform support (Linux/macOS)
- [ ] Event marker system (keyboard shortcuts)
- [ ] Real-time data visualization
- [ ] Configuration file for network settings
- [ ] Automatic CSV backup
- [ ] WiFi signal quality monitoring
- [ ] Data compression for long sessions

---

## 📖 Additional Documentation

### Quick Reference
- **Quick Start**: See `QUICK_START.md`
- **Detailed Setup**: See `SETUP_GUIDE.md`
- **Main README**: See `README.md`

### External Resources
- **Arduino Serial**: https://www.arduino.cc/reference/en/language/functions/communication/serial/
- **Unity Networking**: https://docs.unity3d.com/Manual/UNet.html
- **Windows Sockets**: https://docs.microsoft.com/en-us/windows/win32/winsock/

---

## 📝 Version History

### v1.0 - 04 November 2025
- Initial release
- Console-based interface
- Arduino serial communication
- Unity TCP/UDP networking
- CSV data recording
- Python analysis script
- Complete documentation

---

## 🤝 Contributing

This project is part of ongoing research at NYU Abu Dhabi. For questions, suggestions, or collaboration inquiries:

**Contact**: Pi Ko (pi.ko@nyu.edu)

---

## 📄 License

Copyright © 2025 NYU Abu Dhabi. All rights reserved.

This software is provided for research and educational purposes. Commercial use requires permission.

---

## 🙏 Acknowledgments

- NYU Abu Dhabi Research Computing
- AIMLAB VR Research Team
- Unity XR Community
- Arduino Community

---

**Last Updated**: 04 November 2025  
**Document Version**: 1.0  
**Project Status**: Active Development

