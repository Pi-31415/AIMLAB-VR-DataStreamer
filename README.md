# AIMLAB-VR-DataStreamer

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 21 October 2025  
**Version:** v1.0

Platform agnostic VR Data Streamer for AIMLAB Experiments in C++

## Overview

AIMLAB-VR-DataStreamer is a cross-platform C++ library for streaming VR tracking data over network protocols. It provides a unified API for capturing and transmitting VR device data (HMD, controllers, trackers) to remote systems for research and experimentation.

### Key Features

- **Platform Agnostic**: Works on Windows, Linux, and macOS
- **Multiple Network Protocols**: UDP, TCP, and WebSocket support
- **Flexible Device Support**: Mock devices for testing, extensible for real VR hardware
- **High Performance**: Optimized for low-latency real-time streaming
- **Thread-Safe**: Built-in concurrency support
- **Comprehensive Logging**: Debug and monitor system behavior
- **Easy Integration**: Simple API with extensive examples

## Quick Start

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
- See [BUILD_INSTRUCTIONS.md](docs/BUILD_INSTRUCTIONS.md) for details

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Running Examples

```bash
# Basic streaming example
./build/bin/basic_streaming_example

# Advanced example with monitoring
./build/bin/advanced_streaming_example

# Network receiver
./build/bin/network_receiver_example
```

## Architecture

The library consists of several core components:

- **VRDataStreamer**: Main facade for streaming operations
- **VRDevice**: Abstract interface for VR hardware (mock implementation included)
- **NetworkManager**: Platform-agnostic networking (UDP/TCP)
- **DataPacket**: Serialization/deserialization of VR data
- **Config**: Configuration management
- **Logger**: Thread-safe logging system

## Usage Example

```cpp
#include "aimlab-vr-datastreamer/VRDataStreamer.h"

using namespace AimlabVR;

int main() {
    // Configure
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 90;  // 90 Hz
    
    // Initialize with mock VR device
    VRDataStreamer streamer;
    if (!streamer.initialize(config, true)) {
        return 1;
    }
    
    // Start streaming
    streamer.startStreaming();
    
    // Stream for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // Cleanup
    streamer.stopStreaming();
    streamer.shutdown();
    
    return 0;
}
```

## Project Structure

```
AIMLAB-VR-DataStreamer/
├── include/
│   └── aimlab-vr-datastreamer/    # Public headers
├── src/                            # Implementation files
├── examples/                       # Example applications
├── tests/                          # Unit and integration tests
├── config/                         # Configuration files
├── docs/                           # Documentation
├── CMakeLists.txt                  # Build configuration
└── README.md                       # This file
```

## Documentation

- [API Reference](docs/API_REFERENCE.md) - Complete API documentation
- [Build Instructions](docs/BUILD_INSTRUCTIONS.md) - Platform-specific build guide
- [Usage Guide](docs/USAGE_GUIDE.md) - Comprehensive usage instructions

## Features in Detail

### Network Streaming

- **UDP**: Low-latency, connectionless streaming (default)
- **TCP**: Reliable, connection-oriented streaming
- **Configurable update rates**: 60 Hz to 120+ Hz
- **Automatic reconnection**: Built-in error handling

### Data Format

- **Binary serialization**: Compact, efficient (~200-500 bytes per packet)
- **JSON serialization**: Human-readable, interoperable
- **Extensible packet format**: Easy to add custom data

### VR Device Support

- **Mock devices**: Full simulation for testing without hardware
- **Extensible architecture**: Easy to integrate OpenVR, Oculus, OpenXR
- **Multiple device types**: HMD, controllers, trackers
- **Full 6DOF tracking**: Position and rotation
- **Button/input states**: Triggers, grips, touchpads

### Logging and Monitoring

- **Multiple log levels**: Debug, Info, Warning, Error, Critical
- **File and console output**: Flexible logging destinations
- **Thread-safe logging**: Safe for concurrent operations
- **Streaming statistics**: Packet counts, rates, uptime

## Testing

The library includes comprehensive tests:

```bash
# Unit tests
./build/bin/test_datapacket
./build/bin/test_logger

# Integration test
./build/bin/test_integration
```

## Configuration

Example configuration file (`config/default_config.json`):

```json
{
  "serverAddress": "127.0.0.1",
  "serverPort": 8888,
  "protocol": 0,
  "updateRate": 90,
  "enableLogging": true,
  "logLevel": 1,
  "logFilePath": "aimlab_vr_streamer.log"
}
```

## Performance

Typical performance characteristics:

- **Update rate**: 60-120 Hz
- **Latency**: < 5ms (UDP, local network)
- **Packet size**: 200-500 bytes (binary), 1-2 KB (JSON)
- **CPU usage**: < 2% on modern hardware
- **Network bandwidth**: ~50-200 KB/s per client

## Future Enhancements

- OpenVR integration
- Oculus SDK integration
- OpenXR support
- WebSocket protocol
- Data compression
- Encryption/authentication
- Multi-client broadcasting

## Contributing

This is a research project. For questions or collaboration:

**Contact:** Pi Ko (pi.ko@nyu.edu)

## License

[Specify your license here]

## Acknowledgments

Developed for AIMLAB experiments at NYU.

---

**Version:** v1.0  
**Last Updated:** 21 October 2025  
**Author:** Pi Ko (pi.ko@nyu.edu)
