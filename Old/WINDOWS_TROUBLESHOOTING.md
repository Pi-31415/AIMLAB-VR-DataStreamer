# Windows Troubleshooting Guide - AIMLAB VR Data Streamer

## 🚀 Quick Start Options

Try these in order until one works:

### Option 1: Simple Batch File (Easiest)
```cmd
double-click quick_run.bat
```

### Option 2: Manual Compilation
```cmd
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32
aimlab_network.exe
```

### Option 3: PowerShell Simple Script
```powershell
powershell -ExecutionPolicy Bypass -File build_simple.ps1
```

## 🔧 Common Issues and Solutions

### Issue: "g++ is not recognized as an internal or external command"

**Solution**: Install MinGW-w64
1. Download from: https://github.com/niXman/mingw-builds-binaries/releases
2. Choose: `x86_64-posix-seh` version
3. Extract to `C:\mingw64`
4. Add `C:\mingw64\bin` to your PATH:
   ```cmd
   setx PATH "%PATH%;C:\mingw64\bin"
   ```
5. Restart Command Prompt and try again

### Issue: PowerShell Script Won't Run

**Solution**: Enable script execution
```powershell
# Run PowerShell as Administrator
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Or run directly without changing policy:
```powershell
powershell -ExecutionPolicy Bypass -File build_simple.ps1
```

### Issue: "ws2_32.lib not found" or Linker Errors

**Solution**: Ensure you're using the correct flag:
- MinGW: Use `-lws2_32` (lowercase L)
- Visual Studio: Use `ws2_32.lib`

### Issue: Nodes Don't Connect / Discovery Fails

**Solution 1**: Windows Firewall
```powershell
# Run as Administrator
New-NetFirewallRule -DisplayName "AIMLAB UDP 45000" -Direction Inbound -Protocol UDP -LocalPort 45000 -Action Allow
New-NetFirewallRule -DisplayName "AIMLAB UDP 45001" -Direction Inbound -Protocol UDP -LocalPort 45001 -Action Allow
```

**Solution 2**: Check Network Profile
```powershell
# Must be on Private network for discovery
Get-NetConnectionProfile
Set-NetConnectionProfile -NetworkCategory Private
```

**Solution 3**: Disable Windows Firewall temporarily (testing only)
```cmd
netsh advfirewall set allprofiles state off
REM Test your application
netsh advfirewall set allprofiles state on
```

### Issue: Python Script Won't Run

**Solution**: Install Python
1. Download from: https://www.python.org/downloads/
2. Check "Add Python to PATH" during installation
3. Verify: `python --version`

### Issue: Antivirus Blocking the Application

**Solution**: Add exclusions
- Windows Defender: Settings → Virus & threat protection → Exclusions
- Add folder containing your project files

## 📝 Manual Compilation Commands

### MinGW-w64 (Recommended)
```cmd
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32
```

### Visual Studio Command Prompt
```cmd
cl /std:c++17 /EHsc aimlab_network_cpp.cpp /Fe:aimlab_network.exe ws2_32.lib
```

### Clang (if installed)
```cmd
clang++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32
```

## 🧪 Testing Steps

1. **Compile C++ version**:
   ```cmd
   g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32
   ```

2. **Run in first terminal**:
   ```cmd
   aimlab_network.exe
   ```

3. **Run Python in second terminal**:
   ```cmd
   python aimlab_network_python.py
   ```

4. **Verify connection** - You should see:
   ```
   Discovered new peer: 127.0.0.1:45001
   Handshake complete with 127.0.0.1:45001
   Peer connected: 127.0.0.1:45001
   ```

5. **Test communication**:
   ```
   send Hello World
   ```

## 🔍 Debugging Tips

### Enable Verbose Output
Add debug prints to see what's happening:
```cpp
#define DEBUG_MODE 1
```

### Check Port Usage
See if ports are already in use:
```cmd
netstat -an | findstr 45000
netstat -an | findstr 45001
```

### Test Local UDP
Use PowerShell to test UDP:
```powershell
# Listener
$endpoint = New-Object System.Net.IPEndPoint([System.Net.IPAddress]::Any, 45000)
$udpClient = New-Object System.Net.Sockets.UdpClient(45000)
$data = $udpClient.Receive([ref]$endpoint)
[System.Text.Encoding]::ASCII.GetString($data)
```

## 📊 System Requirements

### Minimum
- Windows 10 version 1903 or later
- 4GB RAM
- MinGW-w64 or Visual Studio 2017+
- Python 3.7+

### Recommended
- Windows 10/11 latest version
- 8GB RAM  
- MinGW-w64 latest version
- Python 3.9+

## 🆘 Still Having Issues?

1. **Check all prerequisites**:
   ```cmd
   g++ --version
   python --version
   ```

2. **Try the simplest approach**:
   ```cmd
   quick_run.bat
   ```

3. **Run as Administrator** if permission issues occur

4. **Restart your computer** after installing MinGW or changing PATH

5. **Use Command Prompt instead of PowerShell** if PowerShell has issues

## ✅ Success Indicators

Your setup is working when:
- Compilation completes without errors
- Application starts and shows "Network manager started on port 45001"
- Discovery messages appear when second node starts
- "Peer connected" message confirms connection
- Messages can be sent between nodes

## 💡 Pro Tips

- Use `run_both.bat` to quickly start both nodes for testing
- Keep Windows Defender exceptions for development folders
- Use `netstat -an | findstr LISTENING` to check port availability
- Run from an elevated Command Prompt if you encounter permission issues
- Consider using Windows Terminal for better multi-tab support