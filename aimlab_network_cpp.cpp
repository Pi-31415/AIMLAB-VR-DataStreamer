/**
 * AIMLAB VR Data Streamer - C++ Network Module
 * 
 * This module provides a robust UDP-based network communication system with:
 * - Automatic peer discovery on local network
 * - Reliable handshake protocol
 * - Bidirectional communication
 * - Heartbeat mechanism for connection monitoring
 * - Thread-safe message queuing
 * 
 * Author: AIMLAB Development Team
 * Version: 1.0.0
 * 
 * Compilation:
 * Linux/Mac: g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network
 * Windows: cl /std:c++17 aimlab_network_cpp.cpp /Fe:aimlab_network.exe ws2_32.lib
 */

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstring>
#include <memory>
#include <sstream>
#include <iomanip>
#include <functional>
#include <map>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    typedef int SOCKET;
#endif

// Undefine Windows macros that conflict with our code
#ifdef ERROR
#undef ERROR
#endif

namespace AIMLAB {

/**
 * @class NetworkConfig
 * @brief Configuration parameters for the network module
 */
class NetworkConfig {
public:
    static constexpr int DISCOVERY_PORT = 45000;
    static constexpr int DEFAULT_DATA_PORT = 45001;
    static constexpr int BUFFER_SIZE = 4096;
    static constexpr int DISCOVERY_INTERVAL_MS = 1000;
    static constexpr int HEARTBEAT_INTERVAL_MS = 5000;
    static constexpr int CONNECTION_TIMEOUT_MS = 15000;
    static constexpr const char* PROTOCOL_VERSION = "1.0";
    static constexpr const char* APP_IDENTIFIER = "AIMLAB_VR";
};

/**
 * @enum MessageType
 * @brief Types of messages in the protocol
 */
enum class MessageType {
    DISCOVER,
    ACKNOWLEDGE,
    HANDSHAKE_START,
    HANDSHAKE_ACK,
    HANDSHAKE_COMPLETE,
    DATA,
    HEARTBEAT,
    DISCONNECT,
    MSG_ERROR  // Renamed from ERROR to avoid Windows macro conflict
};

/**
 * @class Message
 * @brief Represents a network message with type and payload
 */
class Message {
public:
    MessageType type;
    std::string payload;
    std::string sender_ip;
    int sender_port;
    
    Message(MessageType t = MessageType::DATA, 
            const std::string& p = "", 
            const std::string& ip = "", 
            int port = 0)
        : type(t), payload(p), sender_ip(ip), sender_port(port) {}
    
    /**
     * @brief Serialize message to string for transmission
     * @return Serialized message string
     */
    std::string serialize() const {
        std::stringstream ss;
        ss << static_cast<int>(type) << "|" << payload;
        return ss.str();
    }
    
    /**
     * @brief Deserialize message from string
     * @param data Raw message string
     * @return Deserialized Message object
     */
    static Message deserialize(const std::string& data) {
        Message msg;
        size_t delimiter_pos = data.find('|');
        if (delimiter_pos != std::string::npos) {
            msg.type = static_cast<MessageType>(std::stoi(data.substr(0, delimiter_pos)));
            msg.payload = data.substr(delimiter_pos + 1);
        }
        return msg;
    }
};

/**
 * @class UDPSocket
 * @brief Wrapper class for UDP socket operations
 */
class UDPSocket {
private:
    SOCKET socket_fd;
    struct sockaddr_in local_addr;
    bool is_valid;
    
public:
    UDPSocket() : socket_fd(INVALID_SOCKET), is_valid(false) {
        memset(&local_addr, 0, sizeof(local_addr));
    }
    
    ~UDPSocket() {
        close();
    }
    
    /**
     * @brief Initialize socket and bind to specified port
     * @param port Port number to bind (0 for auto-assignment)
     * @return true if successful, false otherwise
     */
    bool initialize(int port = 0) {
        // Create UDP socket
        socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socket_fd == INVALID_SOCKET) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }
        
        // Enable broadcast
        int broadcast_enable = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, 
                      (const char*)&broadcast_enable, sizeof(broadcast_enable)) < 0) {
            std::cerr << "Failed to enable broadcast" << std::endl;
            ::closesocket(socket_fd);
            return false;
        }
        
        // Enable address reuse
        int reuse_enable = 1;
        setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, 
                  (const char*)&reuse_enable, sizeof(reuse_enable));
        
        // Set non-blocking mode
        set_non_blocking();
        
        // Bind to port
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port = htons(port);
        
        if (bind(socket_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
            std::cerr << "Failed to bind to port " << port << std::endl;
            ::closesocket(socket_fd);
            return false;
        }
        
        // Get actual port if auto-assigned
        if (port == 0) {
            socklen_t addr_len = sizeof(local_addr);
            getsockname(socket_fd, (struct sockaddr*)&local_addr, &addr_len);
        }
        
        is_valid = true;
        return true;
    }
    
    /**
     * @brief Send data to specified address
     * @param data Data to send
     * @param ip Target IP address
     * @param port Target port
     * @return Number of bytes sent, -1 on error
     */
    int send_to(const std::string& data, const std::string& ip, int port) {
        if (!is_valid) return -1;
        
        struct sockaddr_in dest_addr;
        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &dest_addr.sin_addr);
        
        return sendto(socket_fd, data.c_str(), data.length(), 0,
                     (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    }
    
    /**
     * @brief Broadcast data to all hosts on local network
     * @param data Data to broadcast
     * @param port Target port
     * @return Number of bytes sent, -1 on error
     */
    int broadcast(const std::string& data, int port) {
        return send_to(data, "255.255.255.255", port);
    }
    
    /**
     * @brief Receive data from socket (non-blocking)
     * @param buffer Buffer to store received data
     * @param buffer_size Size of buffer
     * @param sender_ip Output parameter for sender's IP
     * @param sender_port Output parameter for sender's port
     * @return Number of bytes received, -1 on error or no data
     */
    int receive_from(char* buffer, int buffer_size, 
                    std::string& sender_ip, int& sender_port) {
        if (!is_valid) return -1;
        
        struct sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);
        
        int bytes_received = recvfrom(socket_fd, buffer, buffer_size - 1, 0,
                                     (struct sockaddr*)&sender_addr, &sender_addr_len);
        
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sender_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
            sender_ip = ip_str;
            sender_port = ntohs(sender_addr.sin_port);
        }
        
        return bytes_received;
    }
    
    /**
     * @brief Get the local port number
     * @return Local port number
     */
    int get_local_port() const {
        return ntohs(local_addr.sin_port);
    }
    
    /**
     * @brief Close the socket
     */
    void close() {
        if (is_valid && socket_fd != INVALID_SOCKET) {
            ::closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
            is_valid = false;
        }
    }
    
private:
    void set_non_blocking() {
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(socket_fd, FIONBIO, &mode);
#else
        int flags = fcntl(socket_fd, F_GETFL, 0);
        fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
#endif
    }
};

/**
 * @class PeerInfo
 * @brief Information about a connected peer
 */
class PeerInfo {
public:
    std::string ip_address;
    int port;
    std::chrono::steady_clock::time_point last_heartbeat;
    bool is_connected;
    
    PeerInfo(const std::string& ip = "", int p = 0)
        : ip_address(ip), port(p), 
          last_heartbeat(std::chrono::steady_clock::now()),
          is_connected(false) {}
    
    /**
     * @brief Check if peer connection has timed out
     * @return true if timed out, false otherwise
     */
    bool is_timeout() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_heartbeat).count();
        return elapsed > NetworkConfig::CONNECTION_TIMEOUT_MS;
    }
    
    void update_heartbeat() {
        last_heartbeat = std::chrono::steady_clock::now();
    }
};

/**
 * @class NetworkManager
 * @brief Main class managing network operations and peer connections
 */
class NetworkManager {
private:
    UDPSocket discovery_socket;
    UDPSocket data_socket;
    std::map<std::string, PeerInfo> peers;
    std::queue<Message> incoming_messages;
    std::queue<Message> outgoing_messages;
    
    std::mutex peers_mutex;
    std::mutex incoming_mutex;
    std::mutex outgoing_mutex;
    std::condition_variable message_cv;
    
    std::atomic<bool> running;
    std::atomic<bool> discovering;
    
    std::thread discovery_thread;
    std::thread receive_thread;
    std::thread send_thread;
    std::thread heartbeat_thread;
    
    std::function<void(const Message&)> message_handler;
    
public:
    NetworkManager() : running(false), discovering(false) {}
    
    ~NetworkManager() {
        stop();
    }
    
    /**
     * @brief Start the network manager
     * @param enable_discovery Enable automatic peer discovery
     * @return true if successful, false otherwise
     */
    bool start(bool enable_discovery = true) {
        if (running) return false;
        
        // Initialize platform-specific networking
#ifdef _WIN32
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            std::cerr << "Failed to initialize Winsock" << std::endl;
            return false;
        }
#endif
        
        // Initialize sockets
        if (!discovery_socket.initialize(NetworkConfig::DISCOVERY_PORT)) {
            std::cerr << "Failed to initialize discovery socket" << std::endl;
            return false;
        }
        
        if (!data_socket.initialize(NetworkConfig::DEFAULT_DATA_PORT)) {
            std::cerr << "Failed to initialize data socket" << std::endl;
            return false;
        }
        
        running = true;
        discovering = enable_discovery;
        
        // Start worker threads
        if (enable_discovery) {
            discovery_thread = std::thread(&NetworkManager::discovery_worker, this);
        }
        receive_thread = std::thread(&NetworkManager::receive_worker, this);
        send_thread = std::thread(&NetworkManager::send_worker, this);
        heartbeat_thread = std::thread(&NetworkManager::heartbeat_worker, this);
        
        std::cout << "Network manager started on port " 
                  << data_socket.get_local_port() << std::endl;
        
        return true;
    }
    
    /**
     * @brief Stop the network manager
     */
    void stop() {
        if (!running) return;
        
        running = false;
        discovering = false;
        message_cv.notify_all();
        
        // Send disconnect messages to all peers
        broadcast_disconnect();
        
        // Wait for threads to finish
        if (discovery_thread.joinable()) discovery_thread.join();
        if (receive_thread.joinable()) receive_thread.join();
        if (send_thread.joinable()) send_thread.join();
        if (heartbeat_thread.joinable()) heartbeat_thread.join();
        
        // Close sockets
        discovery_socket.close();
        data_socket.close();
        
        // Cleanup platform-specific networking
#ifdef _WIN32
        WSACleanup();
#endif
        
        std::cout << "Network manager stopped" << std::endl;
    }
    
    /**
     * @brief Send a message to a specific peer
     * @param ip_address Peer's IP address
     * @param port Peer's port
     * @param data Data to send
     */
    void send_to_peer(const std::string& ip_address, int port, const std::string& data) {
        Message msg(MessageType::DATA, data, ip_address, port);
        {
            std::lock_guard<std::mutex> lock(outgoing_mutex);
            outgoing_messages.push(msg);
        }
        message_cv.notify_one();
    }
    
    /**
     * @brief Broadcast a message to all connected peers
     * @param data Data to broadcast
     */
    void broadcast_to_peers(const std::string& data) {
        std::lock_guard<std::mutex> lock(peers_mutex);
        for (const auto& [peer_id, peer_info] : peers) {
            if (peer_info.is_connected) {
                send_to_peer(peer_info.ip_address, peer_info.port, data);
            }
        }
    }
    
    /**
     * @brief Get next message from incoming queue
     * @param msg Output parameter for the message
     * @return true if message available, false otherwise
     */
    bool get_message(Message& msg) {
        std::lock_guard<std::mutex> lock(incoming_mutex);
        if (incoming_messages.empty()) return false;
        
        msg = incoming_messages.front();
        incoming_messages.pop();
        return true;
    }
    
    /**
     * @brief Set custom message handler callback
     * @param handler Function to handle incoming messages
     */
    void set_message_handler(std::function<void(const Message&)> handler) {
        message_handler = handler;
    }
    
    /**
     * @brief Get list of connected peers
     * @return Vector of peer information
     */
    std::vector<PeerInfo> get_connected_peers() {
        std::vector<PeerInfo> connected;
        std::lock_guard<std::mutex> lock(peers_mutex);
        for (const auto& [peer_id, peer_info] : peers) {
            if (peer_info.is_connected && !peer_info.is_timeout()) {
                connected.push_back(peer_info);
            }
        }
        return connected;
    }
    
private:
    /**
     * @brief Worker thread for peer discovery
     */
    void discovery_worker() {
        std::cout << "Discovery worker started" << std::endl;
        
        while (discovering) {
            // Send discovery broadcast
            std::string discovery_msg = std::string(NetworkConfig::APP_IDENTIFIER) + 
                                       ":" + std::to_string(data_socket.get_local_port());
            Message msg(MessageType::DISCOVER, discovery_msg);
            discovery_socket.broadcast(msg.serialize(), NetworkConfig::DISCOVERY_PORT);
            
            // Check for discovery responses
            char buffer[NetworkConfig::BUFFER_SIZE];
            std::string sender_ip;
            int sender_port;
            
            while (discovery_socket.receive_from(buffer, sizeof(buffer), 
                                                sender_ip, sender_port) > 0) {
                handle_discovery_message(buffer, sender_ip);
            }
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(NetworkConfig::DISCOVERY_INTERVAL_MS));
        }
        
        std::cout << "Discovery worker stopped" << std::endl;
    }
    
    /**
     * @brief Worker thread for receiving messages
     */
    void receive_worker() {
        std::cout << "Receive worker started" << std::endl;
        char buffer[NetworkConfig::BUFFER_SIZE];
        
        while (running) {
            std::string sender_ip;
            int sender_port;
            
            int bytes_received = data_socket.receive_from(buffer, sizeof(buffer),
                                                         sender_ip, sender_port);
            
            if (bytes_received > 0) {
                Message msg = Message::deserialize(std::string(buffer));
                msg.sender_ip = sender_ip;
                msg.sender_port = sender_port;
                
                handle_incoming_message(msg);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        std::cout << "Receive worker stopped" << std::endl;
    }
    
    /**
     * @brief Worker thread for sending messages
     */
    void send_worker() {
        std::cout << "Send worker started" << std::endl;
        
        while (running) {
            std::unique_lock<std::mutex> lock(outgoing_mutex);
            message_cv.wait_for(lock, std::chrono::milliseconds(100),
                               [this] { return !outgoing_messages.empty() || !running; });
            
            while (!outgoing_messages.empty()) {
                Message msg = outgoing_messages.front();
                outgoing_messages.pop();
                lock.unlock();
                
                std::string serialized = msg.serialize();
                data_socket.send_to(serialized, msg.sender_ip, msg.sender_port);
                
                lock.lock();
            }
        }
        
        std::cout << "Send worker stopped" << std::endl;
    }
    
    /**
     * @brief Worker thread for sending heartbeats
     */
    void heartbeat_worker() {
        std::cout << "Heartbeat worker started" << std::endl;
        
        while (running) {
            {
                std::lock_guard<std::mutex> lock(peers_mutex);
                
                // Send heartbeats to connected peers
                for (auto& [peer_id, peer_info] : peers) {
                    if (peer_info.is_connected) {
                        if (peer_info.is_timeout()) {
                            std::cout << "Peer timeout: " << peer_id << std::endl;
                            peer_info.is_connected = false;
                        } else {
                            Message heartbeat(MessageType::HEARTBEAT, "");
                            heartbeat.sender_ip = peer_info.ip_address;
                            heartbeat.sender_port = peer_info.port;
                            
                            std::lock_guard<std::mutex> out_lock(outgoing_mutex);
                            outgoing_messages.push(heartbeat);
                        }
                    }
                }
            }
            
            message_cv.notify_one();
            std::this_thread::sleep_for(
                std::chrono::milliseconds(NetworkConfig::HEARTBEAT_INTERVAL_MS));
        }
        
        std::cout << "Heartbeat worker stopped" << std::endl;
    }
    
    /**
     * @brief Handle discovery messages
     */
    void handle_discovery_message(const std::string& data, const std::string& sender_ip) {
        Message msg = Message::deserialize(data);
        
        // Parse discovery message
        if (msg.payload.find(NetworkConfig::APP_IDENTIFIER) == 0) {
            size_t colon_pos = msg.payload.find_last_of(':');
            if (colon_pos != std::string::npos) {
                int peer_port = std::stoi(msg.payload.substr(colon_pos + 1));
                std::string peer_id = sender_ip + ":" + std::to_string(peer_port);
                
                std::lock_guard<std::mutex> lock(peers_mutex);
                
                // Check if peer is new
                if (peers.find(peer_id) == peers.end()) {
                    std::cout << "Discovered new peer: " << peer_id << std::endl;
                    
                    // Add peer
                    peers[peer_id] = PeerInfo(sender_ip, peer_port);
                    
                    // Send acknowledge
                    if (msg.type == MessageType::DISCOVER) {
                        std::string ack_msg = std::string(NetworkConfig::APP_IDENTIFIER) + 
                                             ":" + std::to_string(data_socket.get_local_port());
                        Message ack(MessageType::ACKNOWLEDGE, ack_msg);
                        discovery_socket.send_to(ack.serialize(), sender_ip, 
                                                NetworkConfig::DISCOVERY_PORT);
                        
                        // Initiate handshake
                        initiate_handshake(sender_ip, peer_port);
                    }
                }
            }
        }
    }
    
    /**
     * @brief Handle incoming data messages
     */
    void handle_incoming_message(const Message& msg) {
        std::string peer_id = msg.sender_ip + ":" + std::to_string(msg.sender_port);
        
        // Update peer heartbeat
        {
            std::lock_guard<std::mutex> lock(peers_mutex);
            if (peers.find(peer_id) != peers.end()) {
                peers[peer_id].update_heartbeat();
            }
        }
        
        // Process message based on type
        switch (msg.type) {
            case MessageType::HANDSHAKE_START:
                handle_handshake_start(msg);
                break;
                
            case MessageType::HANDSHAKE_ACK:
                handle_handshake_ack(msg);
                break;
                
            case MessageType::HANDSHAKE_COMPLETE:
                handle_handshake_complete(msg);
                break;
                
            case MessageType::DATA:
                // Add to incoming queue
                {
                    std::lock_guard<std::mutex> lock(incoming_mutex);
                    incoming_messages.push(msg);
                }
                
                // Call custom handler if set
                if (message_handler) {
                    message_handler(msg);
                }
                break;
                
            case MessageType::HEARTBEAT:
                // Heartbeat already handled above
                break;
                
            case MessageType::DISCONNECT:
                handle_disconnect(msg);
                break;
                
            default:
                break;
        }
    }
    
    /**
     * @brief Initiate handshake with peer
     */
    void initiate_handshake(const std::string& ip, int port) {
        std::cout << "Initiating handshake with " << ip << ":" << port << std::endl;
        
        Message handshake(MessageType::HANDSHAKE_START, NetworkConfig::PROTOCOL_VERSION);
        handshake.sender_ip = ip;
        handshake.sender_port = port;
        
        std::lock_guard<std::mutex> lock(outgoing_mutex);
        outgoing_messages.push(handshake);
        message_cv.notify_one();
    }
    
    /**
     * @brief Handle handshake start message
     */
    void handle_handshake_start(const Message& msg) {
        std::cout << "Received handshake start from " << msg.sender_ip 
                  << ":" << msg.sender_port << std::endl;
        
        // Send handshake acknowledgment
        Message ack(MessageType::HANDSHAKE_ACK, NetworkConfig::PROTOCOL_VERSION);
        ack.sender_ip = msg.sender_ip;
        ack.sender_port = msg.sender_port;
        
        std::lock_guard<std::mutex> lock(outgoing_mutex);
        outgoing_messages.push(ack);
        message_cv.notify_one();
    }
    
    /**
     * @brief Handle handshake acknowledgment
     */
    void handle_handshake_ack(const Message& msg) {
        std::cout << "Received handshake ack from " << msg.sender_ip 
                  << ":" << msg.sender_port << std::endl;
        
        // Send handshake complete
        Message complete(MessageType::HANDSHAKE_COMPLETE, "");
        complete.sender_ip = msg.sender_ip;
        complete.sender_port = msg.sender_port;
        
        std::lock_guard<std::mutex> lock(outgoing_mutex);
        outgoing_messages.push(complete);
        message_cv.notify_one();
        
        // Mark peer as connected
        mark_peer_connected(msg.sender_ip, msg.sender_port);
    }
    
    /**
     * @brief Handle handshake complete message
     */
    void handle_handshake_complete(const Message& msg) {
        std::cout << "Handshake complete with " << msg.sender_ip 
                  << ":" << msg.sender_port << std::endl;
        
        // Mark peer as connected
        mark_peer_connected(msg.sender_ip, msg.sender_port);
    }
    
    /**
     * @brief Mark peer as connected
     */
    void mark_peer_connected(const std::string& ip, int port) {
        std::string peer_id = ip + ":" + std::to_string(port);
        
        std::lock_guard<std::mutex> lock(peers_mutex);
        if (peers.find(peer_id) != peers.end()) {
            peers[peer_id].is_connected = true;
            peers[peer_id].update_heartbeat();
            std::cout << "Peer connected: " << peer_id << std::endl;
        }
    }
    
    /**
     * @brief Handle disconnect message
     */
    void handle_disconnect(const Message& msg) {
        std::string peer_id = msg.sender_ip + ":" + std::to_string(msg.sender_port);
        
        std::lock_guard<std::mutex> lock(peers_mutex);
        peers.erase(peer_id);
        
        std::cout << "Peer disconnected: " << peer_id << std::endl;
    }
    
    /**
     * @brief Broadcast disconnect message to all peers
     */
    void broadcast_disconnect() {
        Message disconnect(MessageType::DISCONNECT, "");
        
        std::lock_guard<std::mutex> lock(peers_mutex);
        for (const auto& [peer_id, peer_info] : peers) {
            if (peer_info.is_connected) {
                disconnect.sender_ip = peer_info.ip_address;
                disconnect.sender_port = peer_info.port;
                
                std::string serialized = disconnect.serialize();
                data_socket.send_to(serialized, peer_info.ip_address, peer_info.port);
            }
        }
    }
};

/**
 * @class VRDataStreamer
 * @brief High-level interface for VR data streaming
 */
class VRDataStreamer {
private:
    NetworkManager network;
    std::thread input_thread;
    std::atomic<bool> running;
    
public:
    VRDataStreamer() : running(false) {}
    
    /**
     * @brief Start the VR data streamer
     * @param enable_discovery Enable automatic peer discovery
     * @return true if successful, false otherwise
     */
    bool start(bool enable_discovery = true) {
        if (running) return false;
        
        // Set up message handler
        network.set_message_handler([this](const Message& msg) {
            this->handle_vr_data(msg);
        });
        
        // Start network manager
        if (!network.start(enable_discovery)) {
            return false;
        }
        
        running = true;
        input_thread = std::thread(&VRDataStreamer::input_worker, this);
        
        std::cout << "\n=== AIMLAB VR Data Streamer Started ===" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  'send <message>' - Send message to all peers" << std::endl;
        std::cout << "  'peers' - List connected peers" << std::endl;
        std::cout << "  'quit' - Exit application" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        return true;
    }
    
    /**
     * @brief Stop the VR data streamer
     */
    void stop() {
        if (!running) return;
        
        running = false;
        if (input_thread.joinable()) {
            input_thread.join();
        }
        
        network.stop();
        std::cout << "\nVR Data Streamer stopped" << std::endl;
    }
    
    /**
     * @brief Run the main event loop
     */
    void run() {
        while (running) {
            // Process incoming messages
            Message msg;
            while (network.get_message(msg)) {
                // Messages are already handled by the callback
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
private:
    /**
     * @brief Handle incoming VR data
     */
    void handle_vr_data(const Message& msg) {
        if (msg.type == MessageType::DATA) {
            std::cout << "\n[VR Data from " << msg.sender_ip << ":" 
                     << msg.sender_port << "] " << msg.payload << std::endl;
                     
            // Here you would process VR-specific data
            // For example: parse position/rotation data, hand tracking, etc.
        }
    }
    
    /**
     * @brief Worker thread for handling user input
     */
    void input_worker() {
        std::string input;
        while (running) {
            std::getline(std::cin, input);
            
            if (input == "quit") {
                running = false;
                break;
            } else if (input == "peers") {
                auto peers = network.get_connected_peers();
                std::cout << "\nConnected peers (" << peers.size() << "):" << std::endl;
                for (const auto& peer : peers) {
                    std::cout << "  - " << peer.ip_address << ":" 
                             << peer.port << std::endl;
                }
            } else if (input.find("send ") == 0) {
                std::string message = input.substr(5);
                if (!message.empty()) {
                    // Example VR data format (JSON-like)
                    std::stringstream vr_data;
                    vr_data << "{"
                           << "\"type\":\"vr_transform\","
                           << "\"timestamp\":" << std::chrono::system_clock::now().time_since_epoch().count() << ","
                           << "\"data\":\"" << message << "\""
                           << "}";
                    
                    network.broadcast_to_peers(vr_data.str());
                    std::cout << "Sent: " << vr_data.str() << std::endl;
                }
            }
        }
    }
};

} // namespace AIMLAB

/**
 * @brief Main entry point
 */
int main(int argc, char* argv[]) {
    AIMLAB::VRDataStreamer streamer;
    
    // Parse command line arguments
    bool enable_discovery = true;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--no-discovery") {
            enable_discovery = false;
        }
    }
    
    // Start the streamer
    if (!streamer.start(enable_discovery)) {
        std::cerr << "Failed to start VR Data Streamer" << std::endl;
        return 1;
    }
    
    // Run main event loop
    streamer.run();
    
    // Cleanup
    streamer.stop();
    
    return 0;
}
