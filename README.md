# AIMLAB VR Data Streamer

A robust, cross-platform network communication system for VR data streaming with automatic peer discovery, reliable handshake protocols, and bidirectional communication.

## Features

- **Automatic Peer Discovery**: Nodes automatically discover each other on the local network using UDP broadcast
- **Reliable Handshake Protocol**: Three-way handshake ensures reliable connection establishment
- **Bidirectional Communication**: Full-duplex data exchange between peers
- **Heartbeat Mechanism**: Monitors connection health and detects disconnections
- **Thread-Safe Message Queuing**: Efficient message handling with concurrent operations
- **Cross-Platform Support**: Works on Windows, Linux, and macOS
- **Language Interoperability**: C++ and Python implementations can communicate seamlessly

## Architecture

### Protocol Stack

```
Application Layer    : VR Data (JSON formatted)
Protocol Layer      : Custom AIMLAB Protocol
Transport Layer     : UDP with reliability features
Network Layer       : IPv4
```

### Message Types

1. **DISCOVER** - Broadcast to find peers on network
2. **ACKNOWLEDGE** - Response to discovery
3. **HANDSHAKE_START** - Initiate connection
4. **HANDSHAKE_ACK** - Acknowledge handshake
5. **HANDSHAKE_COMPLETE** - Finalize connection
6. **DATA** - Application data transfer
7. **HEARTBEAT** - Keep-alive messages
8. **DISCONNECT** - Graceful disconnection

### Network Flow

```
Node A                          Node B
   |                               |
   |----DISCOVER (broadcast)------>|
   |<---ACKNOWLEDGE----------------|
   |----HANDSHAKE_START----------->|
   |<---HANDSHAKE_ACK--------------|
   |----HANDSHAKE_COMPLETE-------->|
   |                               |
   |<===== DATA EXCHANGE =========>|
   |                               |
   |----HEARTBEAT----------------->|
   |<---HEARTBEAT------------------|
   |                               |
   |----DISCONNECT---------------->|
```

## Requirements

### C++ Implementation
- C++17 or later
- POSIX sockets (Linux/macOS) or Winsock2 (Windows)
- pthread library (Linux/macOS)

### Python Implementation
- Python 3.7 or later
- Standard library only (no external dependencies)

## Building and Running

### C++ Version

#### Linux/macOS
```bash
# Compile
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network

# Run with discovery enabled (default)
./aimlab_network

# Run without discovery
./aimlab_network --no-discovery
```

#### Windows
```cmd
# Compile with Visual Studio
cl /std:c++17 aimlab_network_cpp.cpp /Fe:aimlab_network.exe ws2_32.lib

# Or with MinGW
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network.exe -lws2_32

# Run
aimlab_network.exe
```

### Python Version

```bash
# Make executable (Linux/macOS)
chmod +x aimlab_network_python.py

# Run with discovery enabled (default)
python3 aimlab_network_python.py

# Run without discovery
python3 aimlab_network_python.py --no-discovery

# Run with debug logging
python3 aimlab_network_python.py --debug

# Run on custom port
python3 aimlab_network_python.py --port 45002
```

## Usage

### Interactive Commands

Both implementations support the following commands:

- `send <message>` - Send a message to all connected peers
- `peers` - List all connected peers
- `status` - Show system status (Python only)
- `simulate` - Toggle VR data simulation (Python only)
- `help` - Display available commands
- `quit` or `exit` - Gracefully shutdown the application

### Example Session

Terminal 1 (C++ Node):
```
$ ./aimlab_network
Network manager started on port 45001
Discovery worker started
Receive worker started
Send worker started
Heartbeat worker started

=== AIMLAB VR Data Streamer Started ===
Commands:
  'send <message>' - Send message to all peers
  'peers' - List connected peers
  'quit' - Exit application
========================================

Discovered new peer: 192.168.1.100:45001
Initiating handshake with 192.168.1.100:45001
Handshake complete with 192.168.1.100:45001
Peer connected: 192.168.1.100:45001

send Hello from C++
Sent: {"type":"vr_transform","timestamp":1234567890,"data":"Hello from C++"}
```

Terminal 2 (Python Node):
```
$ python3 aimlab_network_python.py
2024-01-01 12:00:00 - AIMLAB_VR - INFO - Socket initialized on port 45001
2024-01-01 12:00:00 - AIMLAB_VR - INFO - Network manager started on port 45001

==================================================
          AIMLAB VR Data Streamer
               Python Edition
==================================================
Port: 45001
Protocol: v1.0
==================================================

Commands:
  'send <message>' - Send message to all peers
  'peers'         - List connected peers
  'simulate'      - Toggle VR data simulation
  'status'        - Show system status
  'help'          - Show this help message
  'quit'          - Exit application

2024-01-01 12:00:01 - AIMLAB_VR - INFO - Discovered new peer: 192.168.1.101:45001
2024-01-01 12:00:01 - AIMLAB_VR - INFO - Peer connected: 192.168.1.101:45001

[VR Data from 192.168.1.101:45001]
  Type: vr_transform
  Timestamp: 2024-01-01 12:00:02
  Data: Hello from C++
```

## Integration Guide

### Sending Custom VR Data

#### C++ Example
```cpp
// Create custom VR data
std::stringstream vr_data;
vr_data << "{"
        << "\"type\":\"head_position\","
        << "\"x\":" << head_x << ","
        << "\"y\":" << head_y << ","
        << "\"z\":" << head_z << ","
        << "\"timestamp\":" << get_timestamp()
        << "}";

// Send to all peers
network.broadcast_to_peers(vr_data.str());
```

#### Python Example
```python
# Create custom VR data
vr_data = {
    'type': 'head_position',
    'x': head_x,
    'y': head_y,
    'z': head_z,
    'timestamp': time.time_ns()
}

# Send to all peers
streamer.send_vr_data(vr_data)
```

### Handling Incoming Data

#### C++ Example
```cpp
// Set custom message handler
network.set_message_handler([](const AIMLAB::Message& msg) {
    if (msg.type == AIMLAB::MessageType::DATA) {
        // Parse JSON data
        auto vr_data = parse_json(msg.payload);
        // Process VR data
        update_vr_state(vr_data);
    }
});
```

#### Python Example
```python
# Custom message handler
def handle_vr_update(msg: Message):
    if msg.type == MessageType.DATA:
        vr_data = json.loads(msg.payload)
        # Process VR data
        update_vr_state(vr_data)

# Set handler
network.set_message_handler(handle_vr_update)
```

## Configuration

### Network Parameters

Edit the `NetworkConfig` class in either implementation:

```cpp
// C++
class NetworkConfig {
    static constexpr int DISCOVERY_PORT = 45000;
    static constexpr int DEFAULT_DATA_PORT = 45001;
    static constexpr int BUFFER_SIZE = 4096;
    static constexpr int DISCOVERY_INTERVAL_MS = 1000;
    static constexpr int HEARTBEAT_INTERVAL_MS = 5000;
    static constexpr int CONNECTION_TIMEOUT_MS = 15000;
};
```

```python
# Python
class NetworkConfig:
    DISCOVERY_PORT: int = 45000
    DEFAULT_DATA_PORT: int = 45001
    BUFFER_SIZE: int = 4096
    DISCOVERY_INTERVAL_MS: int = 1000
    HEARTBEAT_INTERVAL_MS: int = 5000
    CONNECTION_TIMEOUT_MS: int = 15000
```

## Troubleshooting

### Common Issues

1. **Peers not discovering each other**
   - Check firewall settings (allow UDP ports 45000-45001)
   - Ensure broadcast is enabled on the network
   - Verify nodes are on the same subnet

2. **Connection timeouts**
   - Check network latency
   - Increase `CONNECTION_TIMEOUT_MS` value
   - Verify no packet loss on the network

3. **Build errors on Windows**
   - Ensure `ws2_32.lib` is linked
   - Use appropriate compiler flags for your toolchain

4. **Python permission errors**
   - May need to run with elevated privileges for broadcast
   - Check socket permissions on the system

### Debug Mode

Enable detailed logging:

**C++**: Add debug output in relevant functions
```cpp
#define DEBUG_MODE 1
#if DEBUG_MODE
    std::cout << "[DEBUG] " << message << std::endl;
#endif
```

**Python**: Use the debug flag
```bash
python3 aimlab_network_python.py --debug
```

## Performance Considerations

- **Message Size**: Keep messages under 4KB for optimal performance
- **Heartbeat Frequency**: Default 5 seconds, adjust based on network reliability
- **Discovery Interval**: 1 second default, increase to reduce network traffic
- **Thread Count**: 4 worker threads per node (discovery, receive, send, heartbeat)
- **Queue Size**: Unbounded queues, implement limits for production use

## Security Notes

This implementation focuses on functionality and does not include:
- Encryption (add TLS/DTLS for production)
- Authentication (implement token-based auth)
- Message validation (add checksums/signatures)
- Rate limiting (implement to prevent DoS)

## License

This project is provided as-is for educational and development purposes.

## Contributing

To extend the functionality:

1. **Add new message types**: Extend the `MessageType` enum
2. **Implement encryption**: Add encryption layer in serialize/deserialize
3. **Add compression**: Compress large VR data payloads
4. **Implement reliable UDP**: Add sequence numbers and retransmission
5. **Add metrics**: Track latency, packet loss, throughput

## Contact

For questions and support regarding the AIMLAB VR Data Streamer, please refer to the inline documentation and code comments.
