# Windows Build Guide - AIMLAB VR Data Streamer

## ✅ Fixed Windows Compatibility Issues

The code has been updated to resolve Windows-specific compilation errors:

### Issues Fixed:
1. **ERROR Macro Conflict**: Windows headers define `ERROR` as a macro, which conflicted with our `MessageType::ERROR` enum value.
   - **Solution**: Renamed `ERROR` to `MSG_ERROR` in both C++ and Python implementations
   - Added `#undef ERROR` directive to prevent conflicts

2. **Namespace Resolution**: Ensured proper namespace qualification for all symbols

3. **Socket Function Conflicts**: Used global scope operator `::` for socket functions to avoid conflicts with member functions

## 🔨 Build Instructions for Windows

### Prerequisites
- **MinGW-w64** or **Visual Studio** with C++17 support
- **Python 3.7+** (for Python version)
- Windows 10 or later

### Option 1: MinGW-w64 (Recommended)

```bash
# Open PowerShell or Command Prompt
cd C:\path\to\project

# Compile with MinGW
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32

# Run the application
.\aimlab_network.exe
```

### Option 2: Visual Studio Developer Command Prompt

```cmd
# Open "Developer Command Prompt for Visual Studio"
cd C:\path\to\project

# Compile with MSVC
cl /std:c++17 /EHsc aimlab_network_cpp.cpp /Fe:aimlab_network.exe ws2_32.lib

# Run the application
aimlab_network.exe
```

### Option 3: Visual Studio IDE

1. Create a new C++ Console Application project
2. Replace the default code with `aimlab_network_cpp.cpp`
3. Set project properties:
   - C++ Language Standard: ISO C++17 or later
   - Add `ws2_32.lib` to Linker > Input > Additional Dependencies
4. Build and run (F5)

## 🐍 Python Version

The Python version works on Windows without modification:

```bash
# Run Python version
python aimlab_network_python.py

# Or with Python 3 explicitly
python3 aimlab_network_python.py
```

## 🧪 Testing Communication

### Step 1: Start C++ Node
```powershell
# Terminal 1
.\aimlab_network.exe
```

### Step 2: Start Python Node
```powershell
# Terminal 2
python aimlab_network_python.py
```

### Step 3: Verify Connection
Both applications should display:
- "Discovered new peer: [IP:PORT]"
- "Handshake complete with [IP:PORT]"
- "Peer connected: [IP:PORT]"

### Step 4: Test Communication
In either terminal, type:
```
send Hello from Windows!
```

The message should appear in the other terminal.

## 🔥 Windows Firewall Configuration

If nodes can't discover each other:

1. **Allow through Windows Firewall**:
   ```powershell
   # Run as Administrator
   New-NetFirewallRule -DisplayName "AIMLAB VR Discovery" -Direction Inbound -Protocol UDP -LocalPort 45000 -Action Allow
   New-NetFirewallRule -DisplayName "AIMLAB VR Data" -Direction Inbound -Protocol UDP -LocalPort 45001 -Action Allow
   ```

2. **Or temporarily disable Windows Firewall** (for testing only):
   - Windows Security → Firewall & network protection
   - Turn off firewall for your network type

## 🛠️ Troubleshooting Windows-Specific Issues

### "ws2_32.lib not found"
- Ensure Windows SDK is installed
- For MinGW: Use `-lws2_32` (lowercase 'l')
- For MSVC: Use `ws2_32.lib`

### "pthread not found"
- MinGW-w64 includes pthread support
- For MSVC: Consider using std::thread instead or install pthread-win32

### Network Discovery Not Working
1. Check Windows network profile:
   ```powershell
   Get-NetConnectionProfile
   ```
2. Set to Private network (allows discovery):
   ```powershell
   Set-NetConnectionProfile -NetworkCategory Private
   ```

### Antivirus Blocking
- Add exceptions for `aimlab_network.exe` and Python
- Temporarily disable real-time protection for testing

## 📊 Performance on Windows

- **UDP Performance**: Excellent on Windows 10/11
- **Thread Performance**: Native Windows threads via std::thread
- **Network Stack**: Winsock2 provides low-latency communication

## 🔄 Cross-Platform Testing

Test communication between Windows and other platforms:

```bash
# Windows machine
.\aimlab_network.exe

# Linux/Mac machine (same network)
./aimlab_network

# They will auto-discover and connect!
```

## 📝 Code Changes Summary

The following changes were made for Windows compatibility:

1. **MessageType enum**:
   ```cpp
   // Before: ERROR
   // After:  MSG_ERROR
   ```

2. **Socket functions**:
   ```cpp
   // Before: closesocket(socket_fd)
   // After:  ::closesocket(socket_fd)
   ```

3. **Preprocessor directive**:
   ```cpp
   #ifdef ERROR
   #undef ERROR
   #endif
   ```

## ✅ Verification

Your build is successful when you see:
```
Network manager started on port 45001
Discovery worker started
Receive worker started
Send worker started
Heartbeat worker started

=== AIMLAB VR Data Streamer Started ===
```

## 📚 Additional Resources

- [MinGW-w64 Download](https://www.mingw-w64.org/downloads/)
- [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
- [Python for Windows](https://www.python.org/downloads/windows/)
- [Windows Sockets 2 Documentation](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)

## 🎮 Happy Streaming on Windows!

The AIMLAB VR Data Streamer is now fully compatible with Windows and ready for your VR applications!
