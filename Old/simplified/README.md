# AIMLAB VR Data Streamer Console

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

## Overview

Console application for controlling VR experiments at NYU Abu Dhabi. This application manages:
- Arduino vibration motor control via serial communication
- Unity VR headset data streaming via TCP/IP
- CSV data recording with timestamps
- Simple console menu interface

## Features

- **Auto-Connect**: Automatically discovers and connects to Arduino and Unity on startup
- **Arduino Control**: Serial communication with vibration motor controller
- **Unity Integration**: TCP socket connection for real-time VR data streaming
- **Data Recording**: CSV export with timestamps and all VR tracking data
- **Simple Interface**: Easy-to-use console menu with number key navigation
- **Status Display**: Real-time connection and recording status
- **Clean Disconnect**: Proper cleanup of all resources on exit

## Requirements

- Windows OS
- MinGW-w64 (for g++ compiler)
- Arduino with vibration motor controller
- Unity VR application with network streaming enabled

## Build Instructions

### Option 1: Using build script
```batch
build.bat
```

### Option 2: Manual compilation
```batch
g++ main.cpp -o AIMLAB_Console.exe -lws2_32 -std=c++17
```

## Usage

1. **Connect your Arduino** to any USB port
2. **Start your Unity VR application** with network streaming enabled
3. **Run the application**:
   ```batch
   AIMLAB_Console.exe
   ```
4. The application will **auto-connect** to both devices on startup
5. Use the **menu options** to control the experiment

## Menu Options

- **1. Connect/Refresh Arduino** - Scan COM ports and connect to Arduino
- **2. Connect/Refresh Unity** - Discover or manually connect to Unity VR
- **3. Test Vibration** - Send test command to vibration motor
- **4. Send Message to Unity** - Send custom message to VR application
- **5. Start Recording** - Begin recording VR data to CSV file
- **6. Stop Recording** - Stop recording and save file
- **7. Auto-Connect Both** - Re-run auto-connect for both devices
- **8. Disconnect All** - Disconnect all devices and stop recording
- **0. Exit** - Clean exit with proper cleanup

## Network Configuration

- **VR Data Port**: 55000 (TCP)
- **Discovery Port**: 55001 (UDP)
- **Discovery Timeout**: 10 seconds

## CSV Data Format

Recorded files include the following columns:
- Timestamp (milliseconds since recording start)
- Head Position (X, Y, Z)
- Head Rotation (X, Y, Z, W quaternion)
- Left Hand Position (X, Y, Z)
- Left Hand Rotation (X, Y, Z, W quaternion)
- Right Hand Position (X, Y, Z)
- Right Hand Rotation (X, Y, Z, W quaternion)

## Arduino Protocol

- **Baud Rate**: 9600
- **Handshake**: Send "HELLO" → Receive "Vibration Motor Controller Ready"
- **Vibration Command**: Send '1' + newline

## Unity Protocol

- **Discovery**: Unity broadcasts "VR_HEADSET_DISCOVERY" on UDP port 55001
- **Handshake**: 
  - PC sends "DATA_RECEIVER_READY"
  - Unity responds "VR_HEADSET_READY"
- **Data Stream**: Comma-separated values over TCP

## Troubleshooting

### Arduino not found
- Check USB connection
- Verify Arduino is programmed with correct firmware
- Try disconnecting and reconnecting USB cable

### Unity not discovered
- Ensure Unity application is running
- Check firewall settings (allow UDP port 55001)
- Use manual IP entry if auto-discovery fails
- Verify both devices are on the same network

### Build errors
- Ensure MinGW-w64 is installed and in PATH
- Verify g++ compiler version supports C++17
- Check that ws2_32.lib is available

## License

Copyright © 2025 NYU Abu Dhabi. All rights reserved.

## Contact

For questions or issues, contact: Pi Ko (pi.ko@nyu.edu)

