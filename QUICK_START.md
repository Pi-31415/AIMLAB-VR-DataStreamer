# AIMLAB VR Data Streamer - Quick Start Guide

## 🚀 Quick Start (30 seconds)

### Option 1: Using Make (Linux/macOS)
```bash
# Build and run C++ version
make run

# In another terminal, run Python version
python3 aimlab_network_python.py
```

### Option 2: Direct Compilation
```bash
# Terminal 1 - C++ Node
g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network
./aimlab_network

# Terminal 2 - Python Node  
python3 aimlab_network_python.py
```

### Option 3: Using Demo Script
```bash
chmod +x run_demo.sh
./run_demo.sh
```

## 🧪 Testing Communication

Run the automated test suite:
```bash
python3 test_communication.py
```

This will send various types of VR data between nodes and verify communication.

## 📋 Basic Commands

Once running, use these commands in either application:

- `send <message>` - Send a message to all connected peers
- `peers` - List all connected peers  
- `quit` - Exit the application

## 🔍 What's Happening?

1. **Discovery Phase**: Nodes broadcast UDP packets to find each other
2. **Handshake**: Three-way handshake establishes reliable connection
3. **Data Exchange**: Bidirectional communication of VR data (JSON format)
4. **Heartbeat**: Keep-alive messages maintain connection health

## 📊 Example VR Data Format

The system exchanges VR data in JSON format:

```json
{
  "type": "head_position",
  "timestamp": 1234567890,
  "position": {
    "x": 0.5,
    "y": 1.6, 
    "z": -0.3
  }
}
```

## 🛠️ Common Issues

### Nodes don't connect?
- Check firewall settings (allow UDP ports 45000-45001)
- Ensure both nodes are on same network subnet
- Try disabling discovery and manually specify IP addresses

### Build errors?
- **Linux/macOS**: Install `build-essential` or Xcode Command Line Tools
- **Windows**: Add `-lws2_32` flag for Winsock library

### Permission errors?
- May need `sudo` for broadcast on some systems
- Check if ports 45000-45001 are available

## 📈 Performance Tips

- Keep messages under 4KB for optimal performance
- Default heartbeat interval is 5 seconds
- Each node uses 4 threads for parallel processing
- UDP provides low latency but no delivery guarantee

## 🔗 Integration Example

### C++ Integration
```cpp
// In your VR application
AIMLAB::VRDataStreamer streamer;
streamer.start();

// Send head tracking data
std::string vr_data = create_vr_json(head_position, head_rotation);
streamer.network.broadcast_to_peers(vr_data);
```

### Python Integration  
```python
# In your VR application
streamer = VRDataStreamer()
streamer.start()

# Send head tracking data
vr_data = {
    'type': 'head_position',
    'timestamp': time.time_ns(),
    'position': {'x': x, 'y': y, 'z': z}
}
streamer.send_vr_data(vr_data)
```

## 📚 Next Steps

1. Review the full README.md for detailed documentation
2. Examine the code comments for API details
3. Run the test suite to verify functionality
4. Integrate into your VR application
5. Customize message types for your specific needs

## 💡 Tips

- Use `--debug` flag with Python for verbose logging
- Monitor network traffic with Wireshark on ports 45000-45001
- The Makefile supports debug builds: `make debug`
- Both implementations are fully interoperable

Happy streaming! 🎮
