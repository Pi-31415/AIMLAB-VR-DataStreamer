# AIMLAB-VR-DataStreamer API Reference

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 21 October 2025  
**Version:** v1.0

## Overview

This document provides a comprehensive API reference for the AIMLAB-VR-DataStreamer library.

## Core Classes

### VRDataStreamer

Main facade class for VR data streaming operations.

#### Constructor
```cpp
VRDataStreamer()
```

#### Methods

##### initialize
```cpp
bool initialize(const std::string& configPath = "", bool useMockDevice = false)
bool initialize(const StreamConfig& config, bool useMockDevice = false)
```
Initializes the streaming system with configuration.

**Parameters:**
- `configPath`: Path to configuration file (optional)
- `config`: Configuration object
- `useMockDevice`: Use mock VR device for testing

**Returns:** `true` if successful, `false` otherwise

##### startStreaming / stopStreaming
```cpp
bool startStreaming()
void stopStreaming()
```
Start or stop continuous VR data streaming.

##### sendFrame
```cpp
bool sendFrame()
```
Manually send one frame of VR data (for manual control mode).

##### getStatistics
```cpp
void getStatistics(uint64_t& totalPacketsSent, 
                   double& packetsPerSecond, 
                   double& uptime) const
```
Get streaming statistics.

##### Callbacks
```cpp
void setErrorCallback(ErrorCallback callback)
void setDataSentCallback(DataSentCallback callback)
```
Set callbacks for error handling and data sent events.

---

### Config

Configuration management class.

#### Methods

##### loadFromFile / saveToFile
```cpp
bool loadFromFile(const std::string& filePath)
bool saveToFile(const std::string& filePath) const
```
Load or save configuration from/to file.

##### getStreamConfig / setStreamConfig
```cpp
StreamConfig getStreamConfig() const
void setStreamConfig(const StreamConfig& config)
```
Get or set stream configuration.

##### Individual Setters
```cpp
void setServerAddress(const std::string& address)
void setServerPort(uint16_t port)
void setProtocol(NetworkProtocol protocol)
void setUpdateRate(uint32_t rate)
void setLoggingEnabled(bool enabled)
```

---

### DataPacket

Container for VR tracking data with serialization support.

#### Methods

##### addDevice
```cpp
void addDevice(const DeviceData& deviceData)
```
Add device data to the packet.

##### serialize / deserialize
```cpp
std::vector<uint8_t> serialize() const
bool deserialize(const std::vector<uint8_t>& data)
```
Serialize to or deserialize from byte array.

##### setFormat
```cpp
void setFormat(PacketFormat format)
```
Set packet format (Binary or JSON).

---

### NetworkManager

Network communication manager.

#### Methods

##### initialize
```cpp
bool initialize(const std::string& serverAddress, 
                uint16_t port, 
                NetworkProtocol protocol)
```
Initialize network manager.

##### connect / disconnect
```cpp
bool connect()
void disconnect()
```
Connect to or disconnect from server.

##### send / receive
```cpp
bool send(const DataPacket& packet)
bool receive(DataPacket& packet, uint32_t timeoutMs = 1000)
```
Send or receive data packets.

---

### VRDevice

Abstract interface for VR device integration.

#### Methods

##### initialize / shutdown
```cpp
virtual bool initialize() = 0
virtual void shutdown() = 0
```

##### update
```cpp
virtual bool update() = 0
```
Update device states (call once per frame).

##### getDevices
```cpp
virtual std::vector<DeviceData> getDevices() const = 0
```
Get all connected devices.

##### Device-Specific Getters
```cpp
virtual bool getHMD(DeviceData& deviceData) const = 0
virtual bool getLeftController(DeviceData& deviceData) const = 0
virtual bool getRightController(DeviceData& deviceData) const = 0
```

---

### Logger

Thread-safe logging system.

#### Methods

##### getInstance
```cpp
static Logger& getInstance()
```
Get singleton instance.

##### initialize
```cpp
void initialize(bool logToFile, 
                const std::string& logFilePath, 
                LogLevel minLevel)
```

##### Logging Methods
```cpp
void debug(const std::string& message, const char* function = "", int line = 0)
void info(const std::string& message, const char* function = "", int line = 0)
void warning(const std::string& message, const char* function = "", int line = 0)
void error(const std::string& message, const char* function = "", int line = 0)
void critical(const std::string& message, const char* function = "", int line = 0)
```

##### Convenience Macros
```cpp
LOG_DEBUG(msg)
LOG_INFO(msg)
LOG_WARNING(msg)
LOG_ERROR(msg)
LOG_CRITICAL(msg)
```

---

## Data Structures

### StreamConfig
```cpp
struct StreamConfig {
    std::string serverAddress;
    uint16_t serverPort;
    NetworkProtocol protocol;
    uint32_t updateRate;
    bool enableLogging;
    LogLevel logLevel;
    std::string logFilePath;
};
```

### DeviceData
```cpp
struct DeviceData {
    uint32_t deviceId;
    DeviceType deviceType;
    TrackingState trackingState;
    Pose pose;
    ButtonState buttons;
    bool isConnected;
    std::string deviceName;
};
```

### Pose
```cpp
struct Pose {
    Vector3 position;
    Quaternion rotation;
    uint64_t timestamp;
};
```

### Vector3 / Quaternion
```cpp
struct Vector3 {
    float x, y, z;
};

struct Quaternion {
    float w, x, y, z;
};
```

---

## Enumerations

### DeviceType
- `Unknown`
- `HeadMountedDisplay`
- `LeftController`
- `RightController`
- `Tracker`
- `BaseStation`

### NetworkProtocol
- `UDP`
- `TCP`
- `WebSocket`

### LogLevel
- `Debug`
- `Info`
- `Warning`
- `Error`
- `Critical`

### ConnectionStatus
- `Disconnected`
- `Connecting`
- `Connected`
- `Error`

---

## Example Usage

### Basic Streaming
```cpp
#include "aimlab-vr-datastreamer/VRDataStreamer.h"

using namespace AimlabVR;

int main() {
    // Configure
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 90;
    
    // Initialize
    VRDataStreamer streamer;
    if (!streamer.initialize(config, true)) {
        return 1;
    }
    
    // Stream
    streamer.startStreaming();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    streamer.stopStreaming();
    
    return 0;
}
```

---

## Platform Support

- **Windows**: Full support
- **Linux**: Full support
- **macOS**: Full support

## Threading

The library is thread-safe. Network operations and VR updates run on separate threads.

## Performance

Default configuration:
- Update rate: 90 Hz
- Typical packet size: 200-500 bytes
- Network overhead: ~5-10 KB/s per device

---

For more information, see the examples in the `examples/` directory.

