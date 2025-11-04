# UDP Chat Application with Auto-Discovery
## Author: Pi Ko (pi.ko@nyu.edu)

### Overview
This is a pair of UDP-based chat applications that automatically discover each other on the local network and establish a bidirectional real-time chat connection. The programs use broadcast messages for discovery and implement a robust handshake protocol.

### Features
- **Auto-Discovery**: Programs automatically find each other using UDP broadcast
- **Order-Independent Launch**: Either program can be started first
- **Robust Handshake**: Three-way handshake ensures reliable connection
- **Real-Time Chat**: Bidirectional messaging with minimal latency
- **Keepalive Messages**: Connection monitoring with periodic heartbeats
- **Error Recovery**: Automatic retry mechanisms for network issues
- **Cross-Platform**: Works on Windows and Linux (Windows-optimized)

### Technical Details

#### Network Architecture
- **Discovery Port**: 50001 (UDP broadcast)
- **Chat Ports**: 
  - Node 1: 50002
  - Node 2: 50003
- **Protocol Messages**:
  - DISCOVER: Broadcast presence announcement
  - ACK: Discovery acknowledgment
  - HANDSHAKE: Connection establishment request
  - READY: Handshake confirmation
  - CHAT: User message
  - KEEPALIVE: Connection heartbeat

#### Discovery Process
1. Both nodes broadcast DISCOVER messages every 2 seconds
2. When a node receives a DISCOVER from its peer, it responds with ACK
3. Upon successful discovery, nodes exchange IP addresses and ports
4. A three-way handshake establishes the connection

### Compilation

#### Windows (using g++ from MinGW or similar)
```bash
g++ -o node1.exe udp_chat_node1.cpp -lws2_32 -std=c++11
g++ -o node2.exe udp_chat_node2.cpp -lws2_32 -std=c++11
```

#### Linux
```bash
g++ -o node1 udp_chat_node1.cpp -pthread -std=c++11
g++ -o node2 udp_chat_node2.cpp -pthread -std=c++11
```

### Usage

1. **Open two terminal windows**

2. **In Terminal 1:**
   ```bash
   ./node1.exe
   ```

3. **In Terminal 2:**
   ```bash
   ./node2.exe
   ```

4. **Wait for connection:**
   - Programs will automatically discover each other
   - You'll see "[HANDSHAKE] Handshake complete!" when connected

5. **Start chatting:**
   - Type messages and press Enter to send
   - Messages appear with sender identification
   - Type 'quit' to exit

### Example Session

**Terminal 1 (Node 1):**
```
=====================================
  UDP Chat Node 1 - Auto Discovery
  Author: Pi Ko (pi.ko@nyu.edu)
=====================================

[INFO] Discovery service started on port 50001
[INFO] Node ID: NODE1
[INFO] Looking for peer: NODE2
[DISCOVERY] Broadcasting presence...
[DISCOVERY] Peer found at 127.0.0.1:50003
[HANDSHAKE] Initiating handshake protocol...
[HANDSHAKE] Handshake complete! Connection established.

[INFO] Chat ready! Type messages and press Enter to send.
[INFO] Type 'quit' to exit.

[You] Hello from Node 1!
[NODE2] Hi there, Node 1!
[You] How are you today?
```

**Terminal 2 (Node 2):**
```
=====================================
  UDP Chat Node 2 - Auto Discovery
  Author: Pi Ko (pi.ko@nyu.edu)
=====================================

[INFO] Discovery service started on port 50001
[INFO] Node ID: NODE2
[INFO] Looking for peer: NODE1
[DISCOVERY] Broadcasting presence...
[DISCOVERY] Acknowledgment from peer at 127.0.0.1:50002
[HANDSHAKE] Initiating handshake protocol...
[HANDSHAKE] Handshake complete! Connection established.

[INFO] Chat ready! Type messages and press Enter to send.
[INFO] Type 'quit' to exit.

[NODE1] Hello from Node 1!
[You] Hi there, Node 1!
[NODE1] How are you today?
[You] I'm doing great, thanks!
```

### Robustness Features

1. **Automatic Retry**: Discovery broadcasts repeat every 2 seconds
2. **Handshake Attempts**: Up to 10 retries with 500ms intervals
3. **Non-Blocking I/O**: Prevents deadlocks and ensures responsiveness
4. **Socket Reuse**: Allows quick restart after program termination
5. **Error Handling**: Comprehensive error checking and reporting
6. **Keepalive**: Detects connection loss (5-second intervals)

### Troubleshooting

#### Programs don't find each other:
- Check firewall settings (allow UDP ports 50001-50003)
- Ensure both programs are on the same network
- Verify broadcast is enabled on your network

#### Connection drops:
- Check network stability
- Increase keepalive frequency if needed
- Review firewall logs for blocked packets

#### Compilation errors:
- Ensure g++ is installed and in PATH
- For Windows: Install MinGW-w64 or similar
- Check that ws2_32.lib is available (Windows)

### Architecture Notes

The application uses a multi-threaded architecture:
- **Main Thread**: Handles user input and sending
- **Discovery Thread**: Manages peer discovery
- **Receive Thread**: Processes incoming messages

All threads communicate through atomic variables and thread-safe operations to ensure data consistency.

### Security Considerations

This is a demonstration application. For production use, consider:
- Message encryption (AES, TLS-like protocols)
- Authentication mechanisms
- Rate limiting to prevent flooding
- Input validation and sanitization
- Configurable ports and addresses

### License
Educational use - created for demonstration purposes

### Contact
Pi Ko - pi.ko@nyu.edu
