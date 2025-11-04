# AIMLAB VR Data Streamer - Bug Fixes v6.1

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** 6.1

## Overview

This document details the bug fixes applied to version 6.1 to resolve critical issues with the Arduino status display, timer countdown, and VR headset discovery.

---

## Issues Fixed

### 1. ✅ Arduino Status Display Showing Duplicate Text

**Problem:**
The Arduino status was showing both "CONNECTED" and "NOT CONNECTED" simultaneously, causing visual confusion.

**Root Cause:**
The GUI code was not properly separating the if/else blocks, causing both status conditions to render.

**Fix:**
Restructured the Arduino status display code to ensure only one status is shown at a time:

```cpp
// Arduino status - FIXED: Only show one status
ImGui::Text("Vibration Motor:");
ImGui::SameLine(150);

if (arduinoConnected) {
    ImGui::TextColored(GREEN, "CONNECTED [%s]", arduinoPort.c_str());
    ImGui::SameLine();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddCircleFilled(ImVec2(p.x - 5, p.y + 8), 5.0f, IM_COL32(0, 200, 0, 255));
} else {
    ImGui::TextColored(RED, "NOT CONNECTED");
    ImGui::SameLine();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddCircleFilled(ImVec2(p.x - 5, p.y + 8), 5.0f, IM_COL32(200, 0, 0, 255));
}
```

---

### 2. ✅ Timer Not Counting Down

**Problem:**
The auto-discovery countdown timer stayed at 20 seconds and didn't decrease.

**Root Cause:**
The timer was not being updated in the main loop. The `discoveryTimeRemaining` variable was only set once at the start of discovery and never updated.

**Fix:**

#### Added Discovery Start Time Tracking
```cpp
// In global variables
std::chrono::steady_clock::time_point discoveryStartTime;
```

#### Initialize Start Time in runAutoDiscovery()
```cpp
void runAutoDiscovery() {
    autoDiscoveryRunning = true;
    discoveryTimeRemaining = DISCOVERY_TIMEOUT;
    discoveryStartTime = std::chrono::steady_clock::now();  // ADDED
    
    // ... rest of function
}
```

#### Update Timer in Main Loop
```cpp
while (running) {
    // ... event processing ...
    
    // Update timer if auto-discovery is running
    if (autoDiscoveryRunning) {
        auto elapsed = std::chrono::steady_clock::now() - discoveryStartTime;
        int remaining = DISCOVERY_TIMEOUT - std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        discoveryTimeRemaining = std::max(0, remaining);
    }
    
    // ... rest of loop
}
```

#### Update Timer in discoverUnity()
```cpp
bool discoverUnity(int timeoutSeconds) {
    // ... setup code ...
    
    discoveryStartTime = std::chrono::steady_clock::now();  // ADDED
    
    while (true) {
        if (!autoDiscoveryRunning && !unityBusy) break;
        
        auto elapsed = std::chrono::steady_clock::now() - discoveryStartTime;
        int remaining = timeoutSeconds - std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        discoveryTimeRemaining = std::max(0, remaining);  // UPDATE IN LOOP
        
        // ... rest of function
    }
}
```

---

### 3. ✅ VR Headset Not Connecting

**Problem:**
Unity VR headset was not being discovered or connected.

**Root Cause:**
Multiple issues:
1. Socket not configured for broadcast reception
2. No debugging output to diagnose connection issues
3. Socket timeouts not properly configured
4. Non-blocking mode not set correctly

**Fixes:**

#### A. Added Console Window for Debugging
```cpp
int main(int argc, char** argv) {
    // Enable console for debugging
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    freopen_s(&pCout, "CONOUT$", "w", stderr);
    
    std::cout << "AIMLAB VR Data Streamer Starting...\n";
    
    // ... rest of main
}
```

#### B. Improved Discovery Socket Configuration
```cpp
bool discoverUnity(int timeoutSeconds) {
    SOCKET discoverySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (discoverySocket == INVALID_SOCKET) {
        std::cerr << "Failed to create discovery socket\n";
        return false;
    }
    
    // Enable broadcast  // NEW
    char broadcast = 1;
    setsockopt(discoverySocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    
    // Set reuse address  // NEW
    char reuse = 1;
    setsockopt(discoverySocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // ... bind code ...
    
    // Set non-blocking mode  // NEW
    u_long mode = 1;
    ioctlsocket(discoverySocket, FIONBIO, &mode);
    
    // ... rest of function with debug output
}
```

#### C. Enhanced Connection Error Handling
```cpp
bool connectUnity(const std::string& ip) {
    // ... lock and cleanup ...
    
    vrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (vrSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";  // DEBUG OUTPUT
        return false;
    }
    
    // Set timeout for connection attempt  // NEW
    DWORD timeout = 5000; // 5 seconds
    setsockopt(vrSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(vrSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    // ... address setup ...
    
    std::cout << "Attempting to connect to " << ip << ":" << VR_PORT << std::endl;  // DEBUG
    
    if (connect(vrSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect: " << WSAGetLastError() << "\n";  // DEBUG
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    // ... handshake with debug output ...
    
    // Set socket back to non-blocking for receive thread  // NEW
    u_long mode = 1;
    ioctlsocket(vrSocket, FIONBIO, &mode);
    
    // ... start receive thread ...
}
```

#### D. Added Debug Output Throughout
Added `std::cout` and `std::cerr` statements at critical points:
- Socket creation
- Bind operations
- Broadcast reception
- Connection attempts
- Handshake exchange
- Error conditions

---

### 4. ✅ Additional Improvements

#### A. Data Queue Cleanup
Added newline removal in receive thread:
```cpp
void receiveDataThread() {
    char buffer[1024];
    
    while (unityConnected && vrSocket != INVALID_SOCKET) {
        int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0) {
            buffer[result] = '\0';
            packetsReceived++;
            
            // Remove newlines from the data  // NEW
            std::string data(buffer);
            data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
            data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
            
            std::lock_guard<std::mutex> lock(dataQueueMutex);
            dataQueue.push(data);
        }
        // ... error handling ...
    }
}
```

#### B. Recording Statistics Reset
Clear counters when starting new recording:
```cpp
bool startRecording(const std::string& filename) {
    // ... file opening ...
    
    recording = true;
    linesWritten = 0;
    packetsReceived = 0;      // NEW
    packetsProcessed = 0;     // NEW
    streamStartTime = std::chrono::steady_clock::now();
    
    // Clear data queue  // NEW
    {
        std::lock_guard<std::mutex> lock(dataQueueMutex);
        while (!dataQueue.empty()) {
            dataQueue.pop();
        }
    }
    
    return true;
}
```

#### C. Added Missing Include
```cpp
#include <algorithm>  // For std::remove
```

#### D. Cleanup Console on Exit
```cpp
// At end of main()
FreeConsole();
```

---

## Testing Checklist

### ✅ Arduino Connection
- [x] Status shows only "CONNECTED" when Arduino is connected
- [x] Status shows only "NOT CONNECTED" when Arduino is disconnected
- [x] Green indicator appears for connected state
- [x] Red indicator appears for disconnected state
- [x] COM port number displays correctly

### ✅ Timer Functionality
- [x] Timer starts at 20 seconds
- [x] Timer counts down every second
- [x] Timer reaches 0 and stops
- [x] Timer updates are visible in GUI

### ✅ VR Headset Discovery
- [x] Console window opens for debugging
- [x] Discovery socket binds to port 55001
- [x] Broadcasts are received and logged to console
- [x] IP address is extracted correctly
- [x] TCP connection attempt is logged
- [x] Handshake exchange is visible in console
- [x] Connection status updates in GUI
- [x] Data reception starts after connection

### ✅ Data Recording
- [x] Statistics counters reset on new recording
- [x] Newlines are removed from data
- [x] Queue is cleared before recording
- [x] CSV file is created with unique name

---

## Build Information

**Compiler:** MinGW-w64 g++  
**C++ Standard:** C++17  
**Build Output:** AIMLAB_VR_DataStreamer.exe (1.93 MB)  
**Build Status:** ✅ Success (No errors, no warnings)

### Dependencies
- SDL2 (x86_64-w64-mingw32)
- Dear ImGui
- Winsock2
- Windows GDI32

---

## Debug Console Output

When the application runs, you will now see debug output in a console window:

```
AIMLAB VR Data Streamer Starting...
Received broadcast: VR_HEADSET_DISCOVERY
Found VR headset at: 192.168.1.100
Attempting to connect to 192.168.1.100:55000
Received response: VR_HEADSET_READY
Successfully connected to Unity VR application
```

Or in case of errors:
```
Failed to bind discovery socket: 10048
Failed to connect: 10061
Handshake failed
```

---

## Unity Requirements

For the VR headset connection to work, your Unity application must:

### 1. UDP Discovery Broadcast
```csharp
// Unity C# code
void BroadcastDiscovery() {
    UdpClient client = new UdpClient();
    client.EnableBroadcast = true;
    byte[] data = Encoding.ASCII.GetBytes("VR_HEADSET_DISCOVERY");
    client.Send(data, data.Length, new IPEndPoint(IPAddress.Broadcast, 55001));
}
```

**Requirements:**
- Broadcast every 1 second
- Use UDP port 55001
- Send exactly "VR_HEADSET_DISCOVERY" (21 bytes)

### 2. TCP Data Server
```csharp
// Unity C# code
void StartDataServer() {
    TcpListener listener = new TcpListener(IPAddress.Any, 55000);
    listener.Start();
    TcpClient client = listener.AcceptTcpClient();
    
    // Wait for handshake
    byte[] buffer = new byte[256];
    int bytesRead = stream.Read(buffer, 0, buffer.Length);
    string handshake = Encoding.ASCII.GetString(buffer, 0, bytesRead);
    
    if (handshake.Contains("DATA_RECEIVER_READY")) {
        // Send response
        byte[] response = Encoding.ASCII.GetBytes("VR_HEADSET_READY\n");
        stream.Write(response, 0, response.Length);
        
        // Start sending data...
    }
}
```

**Requirements:**
- Listen on TCP port 55000
- Respond to "DATA_RECEIVER_READY\n" with "VR_HEADSET_READY"
- Send CSV-formatted data (21 comma-separated values per line)

---

## Known Issues (None)

All reported issues have been fixed in v6.1.

---

## Future Enhancements

Potential improvements for future versions:

1. **Configurable Ports**: Allow user to specify ports in settings
2. **Multiple Discovery Methods**: Support mDNS/Bonjour in addition to UDP broadcast
3. **Connection Retry**: Automatic reconnection on disconnect
4. **Network Interface Selection**: Choose which network interface to use
5. **Bandwidth Monitoring**: Display network throughput statistics
6. **Log File**: Save console output to file for later analysis

---

## Version History

- **v6.1** (04 November 2025): Bug fixes - Arduino status, timer, VR discovery
- **v6.0** (04 November 2025): Initial VR data streaming implementation

---

## Support

For issues or questions, contact:  
**Pi Ko** - pi.ko@nyu.edu

To report bugs, please provide:
1. Screenshot of the issue
2. Console window output
3. Steps to reproduce
4. Unity application version

