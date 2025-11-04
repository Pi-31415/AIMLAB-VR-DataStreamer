# AIMLAB VR Data Streamer - Complete Setup Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

---

## Table of Contents
1. [System Requirements](#system-requirements)
2. [PC Application Setup](#pc-application-setup)
3. [Arduino Setup](#arduino-setup)
4. [Unity VR Setup](#unity-vr-setup)
5. [Network Configuration](#network-configuration)
6. [Testing the System](#testing-the-system)
7. [Troubleshooting](#troubleshooting)

---

## System Requirements

### PC Requirements
- **OS**: Windows 10/11
- **Compiler**: MinGW-w64 (g++)
- **Network**: WiFi or Ethernet connection
- **Ports**: USB port for Arduino

### Arduino Requirements
- **Board**: Arduino Uno, Nano, or Mega
- **Firmware**: Arduino IDE 1.8.x or 2.x
- **Hardware**: Vibration motor + transistor/MOSFET

### Unity VR Requirements
- **Unity Version**: 2020.3 LTS or newer
- **XR Plugin**: Oculus, OpenXR, or SteamVR
- **Platform**: Quest, Vive, Index, or any XR headset
- **Network**: Same network as PC application

---

## PC Application Setup

### Step 1: Install MinGW-w64

**Option A: Via Chocolatey (Recommended)**
```powershell
choco install mingw
```

**Option B: Manual Installation**
1. Download from: https://sourceforge.net/projects/mingw-w64/
2. Install to: `C:\mingw64`
3. Add to PATH: `C:\mingw64\bin`

**Verify Installation:**
```powershell
g++ --version
```

### Step 2: Build the Application

1. Open PowerShell in the project directory
2. Run the build script:
```powershell
.\build.bat
```

3. Verify output:
```
Build SUCCESSFUL!
Run: AIMLAB_Console.exe
```

### Step 3: Configure Firewall

**Allow incoming connections:**
1. Open Windows Defender Firewall
2. Advanced Settings → Inbound Rules → New Rule
3. Port: TCP 55000, UDP 55001
4. Allow the connection
5. Apply to all profiles

**Quick PowerShell command (Run as Administrator):**
```powershell
New-NetFirewallRule -DisplayName "AIMLAB VR Streamer" -Direction Inbound -Protocol TCP -LocalPort 55000 -Action Allow
New-NetFirewallRule -DisplayName "AIMLAB VR Discovery" -Direction Inbound -Protocol UDP -LocalPort 55001 -Action Allow
```

---

## Arduino Setup

### Hardware Connection

**Simple Circuit:**
```
Arduino Pin 9 → Transistor Base (through 1kΩ resistor)
Transistor Collector → Vibration Motor (-)
Transistor Emitter → GND
Vibration Motor (+) → 5V or external power supply
```

**Recommended Transistor:** NPN (2N2222, BC547) or N-Channel MOSFET (2N7000, IRF520)

**Safety Notes:**
- Use a flyback diode across the motor
- For high-current motors, use external power supply
- Never connect motor directly to Arduino pin

### Upload Arduino Sketch

1. Open Arduino IDE
2. Open `Arduino_Vibration_Controller.ino`
3. Select your board: **Tools → Board → Arduino Uno**
4. Select COM port: **Tools → Port → COM#**
5. Upload: **Sketch → Upload** (Ctrl+U)
6. Open Serial Monitor (115200 baud) to verify:
   ```
   Vibration Motor Controller Ready
   ```

### Test Arduino

In Arduino IDE Serial Monitor:
1. Type: `HELLO` and press Enter
   - Response: `Vibration Motor Controller Ready`
2. Type: `1` and press Enter
   - Response: `Vibration activated`
   - Motor should vibrate for 500ms

---

## Unity VR Setup

### Step 1: Create VR Project

1. Create new Unity project (3D template)
2. Install XR Plugin Management:
   - **Edit → Project Settings → XR Plugin Management**
   - Enable your VR platform (Oculus, OpenXR, etc.)

### Step 2: Setup VR Scene

1. Delete default Main Camera
2. Add XR Rig:
   - **GameObject → XR → XR Origin (or XR Rig)**
3. The hierarchy should have:
   - XR Origin
     - Camera Offset
       - Main Camera (head)
       - LeftHand Controller
       - RightHand Controller

### Step 3: Add Data Streamer Script

1. Create empty GameObject: **GameObject → Create Empty**
2. Rename to: `VRDataStreamer`
3. Add script:
   - Copy `Unity_VRDataStreamer.cs` to `Assets/Scripts/`
   - Drag script onto `VRDataStreamer` GameObject

### Step 4: Configure Script

In the Inspector:
1. **Head Transform**: Drag `Main Camera` here
2. **Left Hand Transform**: Drag `LeftHand Controller` here
3. **Right Hand Transform**: Drag `RightHand Controller` here
4. **Data Port**: 55000 (default)
5. **Discovery Port**: 55001 (default)
6. **Stream Rate**: 90 (recommended for VR)

### Step 5: Build and Deploy

**For Quest:**
1. **File → Build Settings**
2. Switch Platform to **Android**
3. Add current scene
4. **Player Settings:**
   - Minimum API Level: 29
   - Install Location: Auto
5. Connect Quest via USB
6. **Build and Run**

**For PC VR:**
1. **File → Build Settings**
2. Platform: **Windows**
3. Add current scene
4. **Build and Run**

---

## Network Configuration

### Same Computer (Testing)
- PC Application: Will auto-detect at `127.0.0.1`
- No network configuration needed

### Local Network
1. **Find Unity Device IP:**
   - Quest: Settings → WiFi → Connected network → IP Address
   - PC VR: Open Command Prompt → `ipconfig` → Look for IPv4

2. **Ensure Same Network:**
   - Both devices must be on same WiFi/LAN
   - Check firewall allows local network traffic

3. **Test Connectivity:**
   ```powershell
   ping <UNITY_IP_ADDRESS>
   ```

---

## Testing the System

### Complete Test Procedure

**1. Test Arduino Independently**
```
1. Upload Arduino sketch
2. Open Arduino Serial Monitor
3. Send "HELLO" → Should receive "Ready" message
4. Send "1" → Motor should vibrate
```

**2. Test Unity Discovery**
```
1. Start Unity VR application
2. Check Unity GUI shows "Waiting for connection..."
3. Note the IP address if shown
```

**3. Test Complete System**
```
1. Connect Arduino to PC via USB
2. Start Unity VR application on headset
3. Run AIMLAB_Console.exe
4. Wait for auto-connect (10-15 seconds)
5. Check status display:
   - Arduino Motor: CONNECTED (COM#)
   - Unity VR: CONNECTED (IP)
```

**4. Test Vibration**
```
1. Press 3 in console menu
2. Motor should vibrate
3. Verify "Vibration command sent" message
```

**5. Test Recording**
```
1. Press 5 to start recording
2. Enter filename: "test_recording"
3. Move VR headset and controllers
4. Press 6 to stop recording
5. Open test_recording.csv in Excel
6. Verify data columns are populated
```

---

## Troubleshooting

### Arduino Issues

**Problem: "No Arduino found on any COM port"**
- Check USB cable connection
- Try different USB port
- Verify Arduino sketch is uploaded
- Check COM port in Device Manager
- Try manual Arduino reset (press reset button)

**Problem: "Handshake failed"**
- Verify baud rate is 9600
- Check serial monitor shows "Ready" message
- Ensure no other program is using COM port
- Replug USB cable and retry

**Problem: "Motor doesn't vibrate"**
- Check circuit connections
- Verify motor power supply
- Test motor with multimeter
- Check transistor is correct type
- Verify Pin 9 is connected

### Unity Issues

**Problem: "Discovery timeout"**
- Verify Unity application is running
- Check firewall allows UDP port 55001
- Ensure devices on same network
- Try manual IP connection (press 2, then y)

**Problem: "Handshake failed"**
- Check Unity script is attached to GameObject
- Verify transforms are assigned in Inspector
- Check Unity Console for errors
- Restart Unity application

**Problem: "Connection drops frequently"**
- Check network stability
- Reduce stream rate in Unity (try 30 instead of 90)
- Ensure WiFi signal is strong
- Check for network congestion

### Recording Issues

**Problem: "Failed to open file"**
- Check write permissions in directory
- Verify filename doesn't contain invalid characters
- Close any Excel files with same name
- Run as Administrator if needed

**Problem: "No data in CSV file"**
- Verify Unity is connected before recording
- Check Unity is sending data (packet count increases)
- Ensure transforms are assigned in Unity
- Move VR headset to generate data

### Network Issues

**Problem: "Cannot find Unity on network"**
```powershell
# Check firewall status
Get-NetFirewallRule -DisplayName "*AIMLAB*"

# Test UDP broadcast
Test-NetConnection -ComputerName <UNITY_IP> -Port 55001

# Check if ports are listening
netstat -an | findstr "55000 55001"
```

**Problem: "Connection refused"**
- Disable Windows Firewall temporarily for testing
- Check antivirus isn't blocking connections
- Verify both devices can ping each other
- Try connecting with manual IP entry

---

## Advanced Configuration

### Change Network Ports

**In main.cpp:**
```cpp
const int VR_PORT = 55000;        // Change TCP port
const int DISCOVERY_PORT = 55001; // Change UDP port
```

**In Unity_VRDataStreamer.cs:**
```csharp
public int dataPort = 55000;      // Match TCP port
public int discoveryPort = 55001; // Match UDP port
```

Remember to rebuild both applications after changes!

### Adjust Stream Rate

Lower rates = Less data, better stability
Higher rates = More data, higher CPU/network usage

**Recommended rates:**
- Testing: 30 Hz
- Normal recording: 60 Hz
- High precision: 90 Hz
- Maximum: 120 Hz

### Custom Vibration Patterns

Modify Arduino sketch for different patterns:
```cpp
// Short pulse
const int VIBRATION_DURATION = 250;

// Long pulse
const int VIBRATION_DURATION = 1000;

// Variable intensity
const int MOTOR_INTENSITY = 128;  // 50% power
```

---

## Getting Help

### Log Files
- Arduino: Serial Monitor output
- Unity: Console window / Player.log
- Console App: Screen output only (consider redirecting)

### Support
- Email: pi.ko@nyu.edu
- Include: Error messages, network setup, screenshots

### Additional Resources
- Arduino Reference: https://www.arduino.cc/reference/
- Unity XR Documentation: https://docs.unity3d.com/Manual/XR.html
- Windows Sockets: https://docs.microsoft.com/en-us/windows/win32/winsock/

---

**Document Version:** 1.0  
**Last Updated:** 04 November 2025  
**Author:** Pi Ko (pi.ko@nyu.edu)

