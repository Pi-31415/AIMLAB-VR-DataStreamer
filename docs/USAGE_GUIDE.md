# Usage Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 21 October 2025  
**Version:** v1.0

## Introduction

This guide provides comprehensive instructions for using the AIMLAB-VR-DataStreamer library in your applications.

## Quick Start

### Minimal Example

```cpp
#include "aimlab-vr-datastreamer/VRDataStreamer.h"

using namespace AimlabVR;

int main() {
    // Create configuration
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 90;
    
    // Create and initialize streamer
    VRDataStreamer streamer;
    if (!streamer.initialize(config, true)) {
        std::cerr << "Failed to initialize" << std::endl;
        return 1;
    }
    
    // Start streaming
    streamer.startStreaming();
    
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // Stop and cleanup
    streamer.stopStreaming();
    streamer.shutdown();
    
    return 0;
}
```

## Configuration

### Using Configuration Files

Create a JSON configuration file:

```json
{
  "serverAddress": "127.0.0.1",
  "serverPort": 8888,
  "protocol": 0,
  "updateRate": 90,
  "enableLogging": true,
  "logLevel": 1,
  "logFilePath": "streamer.log"
}
```

Load it in your application:

```cpp
VRDataStreamer streamer;
streamer.initialize("config.json", true);
```

### Programmatic Configuration

```cpp
StreamConfig config;
config.serverAddress = "192.168.1.100";
config.serverPort = 9999;
config.protocol = NetworkProtocol::UDP;
config.updateRate = 120;  // 120 Hz
config.enableLogging = true;
config.logLevel = LogLevel::Debug;
config.logFilePath = "my_app.log";

streamer.initialize(config, false);
```

## Streaming Modes

### Automatic Streaming

Continuously streams VR data at specified update rate:

```cpp
streamer.startStreaming();
// Streaming happens automatically in background
std::this_thread::sleep_for(std::chrono::seconds(10));
streamer.stopStreaming();
```

### Manual Streaming

Control when each frame is sent:

```cpp
// Don't call startStreaming()
while (running) {
    streamer.sendFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(11));  // ~90 Hz
}
```

## Using VR Devices

### Mock Device (Testing)

For testing without VR hardware:

```cpp
streamer.initialize(config, true);  // true = use mock device
```

### Real VR Device (Future)

```cpp
streamer.initialize(config, false);  // false = use real device
```

## Callbacks and Events

### Error Handling

```cpp
streamer.setErrorCallback([](const std::string& error) {
    std::cerr << "Streaming error: " << error << std::endl;
    // Handle error (log, retry, notify user, etc.)
});
```

### Data Sent Events

```cpp
streamer.setDataSentCallback([](const DataPacket& packet) {
    std::cout << "Sent packet: " << packet.getSequenceNumber() 
              << " with " << packet.getDevices().size() << " devices" 
              << std::endl;
});
```

## Accessing Device Data

### Get Current Devices

```cpp
std::vector<DeviceData> devices = streamer.getCurrentDevices();

for (const auto& device : devices) {
    std::cout << "Device: " << device.deviceName << std::endl;
    std::cout << "  Position: (" 
              << device.pose.position.x << ", "
              << device.pose.position.y << ", "
              << device.pose.position.z << ")" << std::endl;
}
```

### Access Specific Devices

```cpp
// Using VRDevice interface directly
auto vrDevice = createVRDevice(true);
vrDevice->initialize();

DeviceData hmd;
if (vrDevice->getHMD(hmd)) {
    std::cout << "HMD position: " << hmd.pose.position.y << std::endl;
}

DeviceData leftController;
if (vrDevice->getLeftController(leftController)) {
    if (leftController.buttons.trigger) {
        std::cout << "Left trigger pressed!" << std::endl;
    }
}
```

## Monitoring and Statistics

### Get Streaming Statistics

```cpp
uint64_t totalPackets;
double packetsPerSec;
double uptime;

streamer.getStatistics(totalPackets, packetsPerSec, uptime);

std::cout << "Total packets: " << totalPackets << std::endl;
std::cout << "Rate: " << packetsPerSec << " pps" << std::endl;
std::cout << "Uptime: " << uptime << " seconds" << std::endl;
```

### Check Connection Status

```cpp
ConnectionStatus status = streamer.getConnectionStatus();

switch (status) {
    case ConnectionStatus::Connected:
        std::cout << "Connected" << std::endl;
        break;
    case ConnectionStatus::Disconnected:
        std::cout << "Disconnected" << std::endl;
        break;
    case ConnectionStatus::Error:
        std::cout << "Error: " << streamer.getLastError() << std::endl;
        break;
}
```

## Logging

### Using the Logger

```cpp
#include "aimlab-vr-datastreamer/Logger.h"

// Initialize logger
Logger::getInstance().initialize(
    true,                    // log to file
    "my_app.log",           // log file path
    LogLevel::Debug         // minimum level
);

// Log messages
LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARNING("Warning message");
LOG_ERROR("Error message");
LOG_CRITICAL("Critical message");

// Cleanup
Logger::getInstance().shutdown();
```

### Log Level Filtering

```cpp
// Only log warnings and above
Logger::getInstance().setLogLevel(LogLevel::Warning);

LOG_DEBUG("Won't appear");
LOG_INFO("Won't appear");
LOG_WARNING("Will appear");
LOG_ERROR("Will appear");
```

## Network Protocols

### UDP (Default)

Fast, connectionless protocol suitable for real-time streaming:

```cpp
config.protocol = NetworkProtocol::UDP;
```

**Pros:**
- Low latency
- No connection overhead
- Good for real-time data

**Cons:**
- No delivery guarantee
- Packets may arrive out of order

### TCP

Reliable, connection-oriented protocol:

```cpp
config.protocol = NetworkProtocol::TCP;
```

**Pros:**
- Guaranteed delivery
- Ordered packets

**Cons:**
- Higher latency
- Connection overhead

## Data Packet Format

### Binary Format (Default)

Compact binary serialization:

```cpp
DataPacket packet(PacketFormat::Binary);
```

**Advantages:**
- Small packet size (~200-500 bytes)
- Fast serialization
- Efficient for network transmission

### JSON Format

Human-readable JSON serialization:

```cpp
DataPacket packet(PacketFormat::JSON);
```

**Advantages:**
- Human-readable
- Easy to debug
- Interoperable with other systems

**Disadvantages:**
- Larger packet size
- Slower serialization

## Performance Tuning

### Update Rate

Choose appropriate update rate based on your needs:

```cpp
config.updateRate = 60;   // 60 Hz - moderate
config.updateRate = 90;   // 90 Hz - standard VR
config.updateRate = 120;  // 120 Hz - high refresh rate VR
```

### Network Optimization

For high-frequency streaming:

1. Use UDP protocol
2. Use Binary packet format
3. Reduce update rate if bandwidth is limited
4. Use local network (avoid internet routing)

## Error Handling

### Initialization Errors

```cpp
if (!streamer.initialize(config, true)) {
    std::string error = streamer.getLastError();
    std::cerr << "Initialization failed: " << error << std::endl;
    
    // Try with different configuration
    config.serverPort = 9999;
    if (!streamer.initialize(config, true)) {
        return 1;  // Give up
    }
}
```

### Runtime Errors

```cpp
streamer.setErrorCallback([](const std::string& error) {
    LOG_ERROR(error);
    
    // Implement recovery logic
    // - Reconnect
    // - Switch to backup server
    // - Notify user
});
```

## Best Practices

1. **Always check initialization**: Verify `initialize()` returns `true`
2. **Handle errors gracefully**: Set error callbacks
3. **Clean shutdown**: Call `stopStreaming()` and `shutdown()`
4. **Use appropriate update rate**: Don't exceed what your network can handle
5. **Log important events**: Enable logging for production
6. **Test with mock device first**: Verify your integration before using real VR
7. **Monitor statistics**: Track packet rates and connection status

## Integration Example

Complete example integrating all features:

```cpp
#include "aimlab-vr-datastreamer/VRDataStreamer.h"
#include "aimlab-vr-datastreamer/Logger.h"

using namespace AimlabVR;

class VRApplication {
private:
    VRDataStreamer streamer;
    bool isRunning;

public:
    bool initialize() {
        // Setup logging
        Logger::getInstance().initialize(true, "app.log", LogLevel::Info);
        
        // Load configuration
        StreamConfig config;
        config.serverAddress = "127.0.0.1";
        config.serverPort = 8888;
        config.protocol = NetworkProtocol::UDP;
        config.updateRate = 90;
        
        // Initialize streamer
        if (!streamer.initialize(config, true)) {
            LOG_ERROR("Failed to initialize: " + streamer.getLastError());
            return false;
        }
        
        // Setup callbacks
        streamer.setErrorCallback([this](const std::string& error) {
            LOG_ERROR("Streaming error: " + error);
            this->handleError(error);
        });
        
        LOG_INFO("Application initialized successfully");
        return true;
    }
    
    void run() {
        isRunning = true;
        streamer.startStreaming();
        
        LOG_INFO("Application started");
        
        while (isRunning) {
            // Your application logic here
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Optionally check statistics
            uint64_t packets;
            double rate, uptime;
            streamer.getStatistics(packets, rate, uptime);
        }
    }
    
    void shutdown() {
        LOG_INFO("Shutting down application");
        streamer.stopStreaming();
        streamer.shutdown();
        Logger::getInstance().shutdown();
    }
    
private:
    void handleError(const std::string& error) {
        // Implement error recovery
    }
};

int main() {
    VRApplication app;
    
    if (!app.initialize()) {
        return 1;
    }
    
    app.run();
    app.shutdown();
    
    return 0;
}
```

## Troubleshooting

### No Data Being Sent

1. Check connection status
2. Verify server address and port
3. Check firewall settings
4. Enable logging to see detailed errors

### High Latency

1. Use UDP instead of TCP
2. Reduce update rate
3. Use local network
4. Check network congestion

### Packet Loss

1. Use TCP for guaranteed delivery
2. Reduce update rate
3. Check network quality

---

For API details, see `API_REFERENCE.md`  
For build instructions, see `BUILD_INSTRUCTIONS.md`

Contact: Pi Ko (pi.ko@nyu.edu)

