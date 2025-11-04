# AIMLAB VR Data Streamer

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** 6.0

## Overview

The AIMLAB VR Data Streamer is a unified GUI application designed for VR research experiments at AIMLAB, New York University Abu Dhabi. It provides seamless integration between Unity VR applications, Arduino-based vibration motor control, and real-time CSV data logging.

## Features

### 🎮 Unity VR Integration
- **Auto-discovery**: Automatically discovers Unity VR headsets on the local network
- **TCP Communication**: Reliable TCP socket connection for real-time data streaming
- **Non-blocking**: All network operations run in separate threads
- **Connection monitoring**: Visual status indicators with IP address display

### 🔌 Arduino Motor Control
- **Serial communication**: Automatic COM port scanning (COM1-COM40)
- **Handshake protocol**: Proper DTR reset and timeout handling
- **Test vibration**: Manual vibration testing for verification
- **Connection refresh**: Manual reconnection capability

### 📊 Data Recording
- **CSV logging**: Automatic CSV file generation with headers
- **Auto-numbering**: Prevents file overwrites with automatic numbering
- **Real-time statistics**: Track packets received, processed, and lines written
- **Queue management**: Buffered data writing to prevent data loss
- **Timestamps**: Millisecond-precision timestamps for all data points

### 🎨 Professional UI
- **NYU Branding**: Purple and white color scheme (#542AA8)
- **OpenSans Font**: Professional typography at multiple sizes
- **Real-time status**: Visual connection indicators with colored circles
- **Modern design**: Rounded corners, proper spacing, and clean layout
- **Responsive**: Non-blocking UI with threaded operations

## System Requirements

### Software
- **Operating System**: Windows 10/11
- **Compiler**: MinGW-w64 (g++) with C++17 support
- **Libraries**:
  - SDL2 (included in project)
  - Dear ImGui (included in project)
  - Winsock2 (Windows system library)

### Hardware
- **Arduino**: Any Arduino board with serial communication (Uno, Mega, etc.)
- **Vibration Motor**: Connected to Arduino
- **VR Headset**: Unity-based VR application with data streaming capability

## Installation

### 1. Download SDL2 (if not already present)
```powershell
powershell -ExecutionPolicy Bypass .\download_sdl2.ps1
```

### 2. Download ImGui SDL2 Backends (if not already present)
```powershell
powershell -ExecutionPolicy Bypass .\download_imgui_sdl2_backends.ps1
```

### 3. Build the Application
```batch
.\build.bat
```

The build script will:
- Compile all source files
- Link required libraries (SDL2, Winsock2, GDI32)
- Copy SDL2.dll to the output directory
- Create `AIMLAB_VR_DataStreamer.exe`

## Usage

### Starting the Application
```batch
.\AIMLAB_VR_DataStreamer.exe
```

### Auto-Discovery
Upon launch, the application automatically:
1. Scans for available Arduino devices (COM ports)
2. Listens for Unity VR headset discovery broadcasts (20-second timeout)
3. Displays countdown timer during discovery
4. Connects automatically when devices are found

### Manual Connection
If auto-discovery fails or you need to reconnect:
- **Refresh Vibration Motor**: Scans COM ports and reconnects to Arduino
- **Refresh VR Headset**: Listens for Unity discovery broadcast (10-second timeout)

### Testing Vibration
1. Ensure Arduino is connected (green indicator)
2. Click "Test Vibration" button
3. Motor should activate briefly

### Recording VR Data

#### Starting Recording
1. Ensure VR headset is connected (green indicator)
2. Enter a filename in the "Output Filename" field (default: `experiment_data`)
3. Click "Start Recording"
4. The system will:
   - Automatically add `.csv` extension
   - Add numbers if file exists (e.g., `experiment_data_1.csv`)
   - Write CSV header with column names
   - Begin logging data packets

#### During Recording
The interface displays real-time statistics:
- **Recording Time**: HH:MM:SS format
- **Packets Received**: Total packets from Unity
- **Packets Processed**: Packets written to file
- **Lines Written**: Total CSV lines (excluding header)
- **Queue Size**: Number of packets waiting to be written

#### Stopping Recording
1. Click "Stop Recording"
2. File is automatically closed and saved
3. Statistics are preserved until next recording

## Data Format

### CSV Output
The CSV file contains the following columns:

```csv
Timestamp,HeadPosX,HeadPosY,HeadPosZ,HeadRotX,HeadRotY,HeadRotZ,HeadRotW,
LeftHandPosX,LeftHandPosY,LeftHandPosZ,LeftHandRotX,LeftHandRotY,LeftHandRotZ,LeftHandRotW,
RightHandPosX,RightHandPosY,RightHandPosZ,RightHandRotX,RightHandRotY,RightHandRotZ,RightHandRotW
```

- **Timestamp**: Milliseconds since recording started
- **HeadPos**: Head position (X, Y, Z in Unity coordinates)
- **HeadRot**: Head rotation quaternion (X, Y, Z, W)
- **LeftHandPos**: Left controller position
- **LeftHandRot**: Left controller rotation quaternion
- **RightHandPos**: Right controller position
- **RightHandRot**: Right controller rotation quaternion

### Example Data
```csv
0,0.12,1.65,-0.45,0.0,0.87,0.0,0.48,-0.3,1.2,-0.6,0.1,0.2,0.3,0.9,0.3,1.2,-0.6,-0.1,0.2,0.3,0.9
16,0.13,1.66,-0.44,0.01,0.86,0.01,0.49,-0.29,1.19,-0.59,0.11,0.19,0.31,0.91,0.31,1.19,-0.59,-0.09,0.21,0.29,0.92
```

## Network Protocol

### Unity Discovery Protocol
1. **Unity sends UDP broadcast**:
   - Port: 55001
   - Message: `"VR_HEADSET_DISCOVERY"`
   - Interval: Every 1 second

2. **Application responds**:
   - Captures sender IP address
   - Initiates TCP connection to Unity

### Unity Data Streaming
1. **TCP Handshake**:
   - Port: 55000
   - Client sends: `"DATA_RECEIVER_READY\n"`
   - Unity responds: `"VR_HEADSET_READY"`

2. **Data Packets**:
   - Comma-separated values
   - Newline-terminated
   - Format matches CSV columns (without timestamp)

## Arduino Protocol

### Handshake Sequence
1. Toggle DTR to reset Arduino
2. Wait 2 seconds for bootloader
3. Clear serial buffer
4. Send: `"HELLO\r\n"`
5. Receive: `"Vibration Motor Controller Ready"`

### Vibration Command
- Send: `'1'` followed by `'\n'`
- Arduino activates motor for configured duration

## Troubleshooting

### Arduino Not Connecting
- **Check COM ports**: Ensure Arduino is plugged in and drivers are installed
- **Try manual refresh**: Use "Refresh Vibration Motor" button
- **Verify firmware**: Ensure Arduino has the vibration controller sketch uploaded
- **Check baud rate**: Must be 9600 baud

### Unity Not Connecting
- **Network issues**: Ensure both devices are on the same network
- **Firewall**: Allow UDP port 55001 and TCP port 55000
- **Unity not running**: Start Unity VR application first
- **Check implementation**: Verify Unity has discovery broadcast code

### Recording Issues
- **File permission**: Ensure write permission in application directory
- **Filename**: Use alphanumeric characters only (no special characters)
- **Disk space**: Ensure sufficient disk space for CSV files
- **VR disconnected**: Recording requires active VR connection

### Build Issues
- **MinGW not found**: Ensure MinGW-w64 is in PATH
- **SDL2 missing**: Run `download_sdl2.ps1`
- **ImGui backends missing**: Run `download_imgui_sdl2_backends.ps1`

## Technical Details

### Architecture
```
┌─────────────────────────────────────────┐
│         GUI Main Thread                 │
│  - ImGui rendering                      │
│  - Event handling                       │
│  - Status display                       │
└─────────────────────────────────────────┘
           │              │
           ├──────────────┼──────────────────┐
           ▼              ▼                  ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Arduino     │  │  Unity VR    │  │  Data Write  │
│  Thread      │  │  Thread      │  │  Thread      │
│              │  │              │  │              │
│ - COM scan   │  │ - Discovery  │  │ - Queue read │
│ - Handshake  │  │ - TCP recv   │  │ - CSV write  │
│ - Commands   │  │ - Data queue │  │ - Flush      │
└──────────────┘  └──────────────┘  └──────────────┘
```

### Thread Safety
- **Mutexes**: Protect shared data structures
- **Atomic variables**: For simple flags and counters
- **Lock guards**: RAII-based locking for safety
- **Queue**: Thread-safe data queue with mutex

### Performance
- **Non-blocking UI**: All I/O in separate threads
- **Buffered writing**: Queue prevents data loss during disk I/O
- **Efficient rendering**: 60 FPS with VSync
- **Low latency**: Immediate packet processing

## File Structure
```
MotorController/
├── main.cpp                          # Main application source
├── build.bat                         # Build script
├── AIMLAB_VR_DataStreamer.exe       # Compiled executable
├── SDL2.dll                         # SDL2 runtime library
├── OpenSans-Regular.ttf             # Font file
├── imgui.cpp                        # ImGui core
├── imgui_draw.cpp                   # ImGui rendering
├── imgui_widgets.cpp                # ImGui widgets
├── imgui_tables.cpp                 # ImGui tables
├── backends/
│   ├── imgui_impl_sdl2.cpp         # SDL2 backend
│   └── imgui_impl_sdlrenderer2.cpp # SDL2 renderer backend
└── SDL2/                            # SDL2 development files
```

## Credits

**Developed at:**  
AIMLAB (AI & Machine Learning Applied to Behavior)  
New York University Abu Dhabi

**Author:**  
Pi Ko (pi.ko@nyu.edu)

**Technologies:**
- Dear ImGui by Omar Cornut
- SDL2 by Sam Lantinga
- OpenSans font by Steve Matteson

## License

This project is for research use at NYU Abu Dhabi. Please contact the author for usage inquiries.

## Version History

- **v6.0** (04 November 2025): Complete VR data streaming integration
- **v5.2** (04 November 2025): Added OpenSans font support
- **v5.1** (04 November 2025): Serial port timeout fixes
- **v5.0** (04 November 2025): SDL2 implementation
- **v4.0** (04 November 2025): Migrated to SDL2 software renderer
- **v3.0** (02 November 2025): Full Arduino serial communication
- **v2.0** (02 November 2025): Win32 API implementation
- **v1.0** (02 November 2025): Initial ImGui + GLFW + OpenGL3

## Support

For issues, questions, or contributions, please contact:  
**Pi Ko** - pi.ko@nyu.edu

