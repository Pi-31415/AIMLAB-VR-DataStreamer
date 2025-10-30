# AIMLAB-VR-Data Streamer
## Author: Pi Ko (pi.ko@nyu.edu)
## Version: 1.0

### Overview
AIMLAB-VR-Data Streamer is a robust UDP-based communication system for streaming data from Unity VR applications to a C++ logging node. The system features automatic discovery, bidirectional communication, and real-time data logging with file management capabilities.

### Features
- **Auto-Discovery**: Automatic peer discovery using UDP broadcast
- **Real-Time Communication**: Low-latency data streaming between Unity and C++
- **File Management**: Dynamic file creation, data logging, and file closure
- **Command System**: Remote control of file operations from Unity
- **CSV Output**: Data saved in CSV format with timestamps
- **Robust Connection**: Automatic retry, handshake protocol, and keepalive
- **VR Optimized**: Designed for VR tracking data (position, rotation, events)

### System Architecture

```
Unity VR Application (C#)          C++ Data Logger
    AIMLABVRDataStreamer     <-->    aimlab_streamer.exe
         |                                  |
    [Discovery]                        [Discovery]
         |                                  |
    [Handshake]                        [Handshake]
         |                                  |
    [Commands]  ------------------>   [File Operations]
    [VR Data]   ------------------>   [CSV Logging]
    [Status]    <------------------   [Responses]
```

### Installation

#### C++ Node Setup

1. **Requirements**:
   - Windows with MinGW-w64 (g++ compiler)
   - Or Linux with g++ compiler

2. **Compilation**:
   ```bash
   # Windows
   compile_aimlab.bat
   
   # Or manually
   g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11
   
   # Linux
   g++ -o aimlab_streamer aimlab_vr_datastreamer.cpp -pthread -std=c++11
   ```

3. **Running**:
   ```bash
   ./aimlab_streamer.exe
   ```

#### Unity Setup

1. **Add Script to Project**:
   - Copy `AIMLABVRDataStreamer.cs` to your Unity project's Assets folder
   - Add the script to a GameObject in your scene

2. **Configure in Inspector**:
   - **Auto Connect**: Enable for automatic connection on start
   - **Max Discovery Attempts**: Number of discovery broadcasts (default: 30)
   - **Discovery Interval**: Time between broadcasts (default: 2 seconds)
   - **Enable Periodic Data**: Auto-send data at intervals
   - **Data Send Interval**: Frequency of automatic data sending

3. **Optional UI Setup**:
   - Create UI elements (Text, InputField, Buttons)
   - Link them to the script's UI references in Inspector

### Usage

#### Basic Workflow

1. **Start C++ Logger**:
   ```
   ./aimlab_streamer.exe
   ```

2. **Start Unity Application**:
   - Play your Unity scene with AIMLABVRDataStreamer component

3. **Auto-Connection**:
   - Applications automatically discover each other
   - Handshake establishes connection

4. **File Operations** (from Unity):
   ```csharp
   // Get reference to the streamer
   AIMLABVRDataStreamer streamer = GetComponent<AIMLABVRDataStreamer>();
   
   // Open a file with auto-generated name
   streamer.OpenFile();
   
   // Or specify a filename
   streamer.OpenFile("session_001.csv");
   
   // Send data
   streamer.SendData("EventType", "ButtonPressed");
   streamer.SendVector3("HeadPosition", transform.position);
   streamer.SendQuaternion("HeadRotation", transform.rotation);
   streamer.SendTransform("Controller", controllerTransform);
   
   // Close file when done
   streamer.CloseFile();
   ```

#### C++ Console Commands

- `help` - Show available commands
- `status` - Display current status
- `open` - Open a new data file (auto-named)
- `close` - Close current data file
- `quit` - Exit application

### Data Format

Data is saved in CSV format with three columns:
```csv
Timestamp,Type,Data
1698765432000,HeadPosition,1.234,0.567,2.890
1698765432010,HeadRotation,0.000,0.707,0.000,0.707
1698765432020,EventType,TriggerPressed
```

### Unity API Reference

#### Properties
- `bool autoConnect` - Auto-start discovery on awake
- `bool enablePeriodicData` - Send data at fixed intervals
- `float dataSendInterval` - Interval for periodic data

#### Methods
```csharp
// Connection
void StartConnection()
void StopConnection()
bool IsConnected()

// File Operations
void OpenFile(string filename = "")
void CloseFile()
bool IsFileOpen()

// Data Sending
void SendData(string dataType, string dataValue)
void SendVector3(string label, Vector3 vector)
void SendQuaternion(string label, Quaternion rotation)
void SendTransform(string label, Transform transform)
```

### Example Unity Integration

```csharp
using UnityEngine;
using AIMLAB.VR;

public class VRDataLogger : MonoBehaviour
{
    private AIMLABVRDataStreamer dataStreamer;
    
    void Start()
    {
        dataStreamer = GetComponent<AIMLABVRDataStreamer>();
        
        // Open file when scene starts
        if (dataStreamer.IsConnected())
        {
            dataStreamer.OpenFile($"vr_session_{System.DateTime.Now:yyyyMMdd_HHmmss}.csv");
        }
    }
    
    void Update()
    {
        if (!dataStreamer.IsFileOpen()) return;
        
        // Log head tracking
        if (Camera.main != null)
        {
            dataStreamer.SendTransform("Head", Camera.main.transform);
        }
        
        // Log controller input
        if (Input.GetButtonDown("Fire1"))
        {
            dataStreamer.SendData("Input", "TriggerPressed");
        }
        
        // Log custom metrics
        float performance = 1.0f / Time.deltaTime;
        dataStreamer.SendData("FPS", performance.ToString("F1"));
    }
    
    void OnApplicationQuit()
    {
        dataStreamer.CloseFile();
    }
}
```

### Network Configuration

#### Default Ports
- Discovery: 45000-45009 (UDP broadcast)
- C++ Data Port: 45100
- Unity Data Port: 45101

#### Firewall Settings
Allow UDP traffic on ports:
- 45000-45010 (Discovery)
- 45100-45101 (Data communication)

### File Storage

Data files are saved to:
- **Directory**: `aimlab_data/`
- **Format**: CSV with timestamps
- **Naming**: 
  - Auto: `aimlab_session_YYYYMMDD_HHMMSS_N.csv`
  - Custom: User-specified name

### Troubleshooting

#### Connection Issues
1. **Check Firewall**: Allow UDP ports 45000-45101
2. **Same Network**: Ensure both devices are on the same network
3. **Broadcast Support**: Verify network allows UDP broadcast
4. **Port Conflicts**: Applications try multiple ports automatically

#### Data Not Saving
1. **File Open**: Ensure file is opened before sending data
2. **Permissions**: Check write permissions for `aimlab_data` folder
3. **Status Check**: Use `status` command in C++ console

#### Unity Errors
1. **Missing Namespace**: Add `using AIMLAB.VR;`
2. **Null Reference**: Ensure component is attached to GameObject
3. **Thread Safety**: Use provided methods, don't access sockets directly

### Performance Considerations

- **Data Rate**: Default 10Hz (configurable via `dataSendInterval`)
- **Buffer Size**: 4KB per message
- **Network Load**: Keepalive every 5 seconds
- **File I/O**: Immediate flush for data integrity

### Advanced Features

#### Custom Data Types
```csharp
// Send complex data as JSON
string jsonData = JsonUtility.ToJson(customObject);
dataStreamer.SendData("CustomObject", jsonData);

// Send binary data as Base64
byte[] binaryData = GetBinaryData();
string base64 = System.Convert.ToBase64String(binaryData);
dataStreamer.SendData("BinaryData", base64);
```

#### Batch Operations
```csharp
// Send multiple data points efficiently
void SendBatchData(List<DataPoint> dataPoints)
{
    foreach (var point in dataPoints)
    {
        dataStreamer.SendData(point.Type, point.Value);
    }
}
```

### Security Notes

- No encryption (add TLS if needed for sensitive data)
- No authentication (implement token-based auth if required)
- Local network only (not internet-facing by default)

### License
Educational and research use
Created for AIMLAB VR research projects

### Support
Pi Ko - pi.ko@nyu.edu

### Version History
- v1.0 (2025): Initial release with core features
  - Auto-discovery
  - File management
  - Real-time streaming
  - Unity integration
