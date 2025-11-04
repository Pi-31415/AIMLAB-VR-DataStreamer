/**
 * @file udp_chat_node1_v2.cpp
 * @brief UDP Chat Application with Auto-Discovery - Node 1 (Improved Version)
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 2025
 * 
 * Improved version with better port handling and discovery mechanism
 * that works even when ports are in use.
 * 
 * Compilation: g++ -o node1_v2.exe udp_chat_node1_v2.cpp -lws2_32 -std=c++11
 * Usage: ./node1_v2.exe
 */

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <sstream>
#include <random>

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
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

// Configuration constants
const int DISCOVERY_BASE_PORT = 50001;  // Base port for discovery
const int CHAT_PORT = 50002;            // Port for chat communication  
const int BUFFER_SIZE = 1024;           // Message buffer size
const std::string NODE_ID = "NODE1";    // Unique identifier for this node
const std::string PEER_ID = "NODE2";    // Expected peer identifier

// Protocol message types
const std::string MSG_DISCOVER = "DISCOVER";
const std::string MSG_ACKNOWLEDGE = "ACK";
const std::string MSG_HANDSHAKE = "HANDSHAKE";
const std::string MSG_READY = "READY";
const std::string MSG_CHAT = "CHAT";
const std::string MSG_KEEPALIVE = "KEEPALIVE";

// Global state management
std::atomic<bool> peer_discovered(false);
std::atomic<bool> handshake_complete(false);
std::atomic<bool> running(true);
std::string peer_address;
int peer_port = 0;
int actual_discovery_port = DISCOVERY_BASE_PORT;

/**
 * @brief Initialize Windows Socket API (Windows only)
 * @return true if successful, false otherwise
 */
bool initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "[ERROR] WSAStartup failed: " << result << std::endl;
        return false;
    }
#endif
    return true;
}

/**
 * @brief Cleanup Windows Socket API (Windows only)
 */
void cleanupWinsock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

/**
 * @brief Enable broadcast capability on a socket
 * @param sock Socket descriptor
 * @return true if successful, false otherwise
 */
bool enableBroadcast(SOCKET sock) {
    int broadcast_enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, 
                   (const char*)&broadcast_enable, sizeof(broadcast_enable)) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

/**
 * @brief Set socket to non-blocking mode
 * @param sock Socket descriptor
 * @return true if successful, false otherwise
 */
bool setNonBlocking(SOCKET sock) {
#ifdef _WIN32
    u_long mode = 1;
    if (ioctlsocket(sock, FIONBIO, &mode) != 0) {
        return false;
    }
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        return false;
    }
#endif
    return true;
}

/**
 * @brief Try to bind to a discovery port, trying multiple ports if needed
 * @param sock Socket to bind
 * @return The port number if successful, -1 otherwise
 */
int tryBindDiscoveryPort(SOCKET sock) {
    sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Try a range of ports starting from the base
    for (int port_offset = 0; port_offset < 10; port_offset++) {
        int try_port = DISCOVERY_BASE_PORT + port_offset;
        local_addr.sin_port = htons(try_port);
        
        if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) != SOCKET_ERROR) {
            std::cout << "[INFO] Successfully bound to discovery port " << try_port << std::endl;
            return try_port;
        }
        
#ifdef _WIN32
        int error = WSAGetLastError();
        if (error != WSAEADDRINUSE && error != WSAEACCES) {
            // Different error, stop trying
            std::cerr << "[ERROR] Bind failed with error " << error << std::endl;
            break;
        }
#endif
    }
    
    return -1;  // Failed to bind to any port
}

/**
 * @brief Improved discovery thread with dual-socket approach
 * 
 * Uses separate sockets for sending and receiving to avoid port conflicts
 */
void discoveryThread() {
    // Create sending socket (for broadcasts)
    SOCKET send_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock == INVALID_SOCKET) {
        std::cerr << "[ERROR] Failed to create send socket" << std::endl;
        return;
    }
    
    enableBroadcast(send_sock);
    setNonBlocking(send_sock);
    
    // Create receiving socket
    SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_sock == INVALID_SOCKET) {
        std::cerr << "[ERROR] Failed to create receive socket" << std::endl;
        closesocket(send_sock);
        return;
    }
    
    // Set socket options for receiving socket
    int reuse = 1;
    setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, 
               (const char*)&reuse, sizeof(reuse));
    
    setNonBlocking(recv_sock);
    
    // Try to bind the receiving socket
    actual_discovery_port = tryBindDiscoveryPort(recv_sock);
    bool can_receive = (actual_discovery_port != -1);
    
    if (!can_receive) {
        std::cout << "[WARNING] Could not bind to any discovery port for receiving" << std::endl;
        std::cout << "[INFO] Will operate in broadcast-only mode" << std::endl;
        closesocket(recv_sock);
        recv_sock = INVALID_SOCKET;
    }
    
    std::cout << "[INFO] Discovery service started" << std::endl;
    std::cout << "[INFO] Node ID: " << NODE_ID << std::endl;
    std::cout << "[INFO] Looking for peer: " << PEER_ID << std::endl;
    std::cout << "[INFO] Broadcasting on ports " << DISCOVERY_BASE_PORT 
              << " to " << (DISCOVERY_BASE_PORT + 9) << std::endl;
    
    // Broadcast address setup
    sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    char buffer[BUFFER_SIZE];
    auto last_broadcast = std::chrono::steady_clock::now();
    
    while (running && !peer_discovered) {
        // Send discovery broadcast every 2 seconds to multiple ports
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_broadcast).count() >= 2) {
            std::string discover_msg = MSG_DISCOVER + ":" + NODE_ID + ":" + 
                                       std::to_string(CHAT_PORT) + ":" + 
                                       std::to_string(actual_discovery_port);
            
            // Broadcast to multiple ports to increase chances of discovery
            for (int port_offset = 0; port_offset < 10; port_offset++) {
                broadcast_addr.sin_port = htons(DISCOVERY_BASE_PORT + port_offset);
                sendto(send_sock, discover_msg.c_str(), discover_msg.length(), 0,
                       (sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
            }
            
            std::cout << "[DISCOVERY] Broadcasting presence..." << std::endl;
            last_broadcast = now;
        }
        
        // Listen for discovery messages if we have a receiving socket
        if (recv_sock != INVALID_SOCKET) {
            sockaddr_in sender_addr;
            socklen_t sender_len = sizeof(sender_addr);
            memset(buffer, 0, BUFFER_SIZE);
            
            int recv_len = recvfrom(recv_sock, buffer, BUFFER_SIZE - 1, 0,
                                    (sockaddr*)&sender_addr, &sender_len);
            
            if (recv_len > 0) {
                buffer[recv_len] = '\0';
                std::string received(buffer);
                
                // Parse discovery message
                std::istringstream iss(received);
                std::string msg_type, sender_id, sender_port_str, sender_disc_port;
                std::getline(iss, msg_type, ':');
                std::getline(iss, sender_id, ':');
                std::getline(iss, sender_port_str, ':');
                std::getline(iss, sender_disc_port, ':');
                
                if (msg_type == MSG_DISCOVER && sender_id == PEER_ID) {
                    // Peer found!
                    peer_address = inet_ntoa(sender_addr.sin_addr);
                    peer_port = std::stoi(sender_port_str);
                    
                    std::cout << "[DISCOVERY] Peer found at " << peer_address 
                              << ":" << peer_port << std::endl;
                    
                    // Send acknowledgment to peer's discovery port
                    if (!sender_disc_port.empty()) {
                        int peer_disc_port = std::stoi(sender_disc_port);
                        sockaddr_in peer_disc_addr;
                        memset(&peer_disc_addr, 0, sizeof(peer_disc_addr));
                        peer_disc_addr.sin_family = AF_INET;
                        peer_disc_addr.sin_addr = sender_addr.sin_addr;
                        peer_disc_addr.sin_port = htons(peer_disc_port);
                        
                        std::string ack_msg = MSG_ACKNOWLEDGE + ":" + NODE_ID + ":" + 
                                              std::to_string(CHAT_PORT);
                        sendto(send_sock, ack_msg.c_str(), ack_msg.length(), 0,
                               (sockaddr*)&peer_disc_addr, sizeof(peer_disc_addr));
                    }
                    
                    peer_discovered = true;
                }
                else if (msg_type == MSG_ACKNOWLEDGE && sender_id == PEER_ID) {
                    // Acknowledgment received
                    peer_address = inet_ntoa(sender_addr.sin_addr);
                    peer_port = std::stoi(sender_port_str);
                    
                    std::cout << "[DISCOVERY] Acknowledgment from peer at " 
                              << peer_address << ":" << peer_port << std::endl;
                    
                    peer_discovered = true;
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    closesocket(send_sock);
    if (recv_sock != INVALID_SOCKET) {
        closesocket(recv_sock);
    }
    std::cout << "[INFO] Discovery service stopped" << std::endl;
}

/**
 * @brief Perform handshake with discovered peer
 * @param sock Chat socket descriptor
 * @return true if handshake successful, false otherwise
 */
bool performHandshake(SOCKET sock) {
    std::cout << "[HANDSHAKE] Initiating handshake protocol..." << std::endl;
    
    sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peer_address.c_str());
    peer_addr.sin_port = htons(peer_port);

    // Send handshake request
    std::string handshake_msg = MSG_HANDSHAKE + ":" + NODE_ID;
    int attempts = 0;
    bool handshake_sent = false;
    bool handshake_received = false;

    char buffer[BUFFER_SIZE];
    
    while (attempts < 10 && (!handshake_sent || !handshake_received)) {
        // Send handshake if not acknowledged
        if (!handshake_sent) {
            sendto(sock, handshake_msg.c_str(), handshake_msg.length(), 0,
                   (sockaddr*)&peer_addr, sizeof(peer_addr));
            std::cout << "[HANDSHAKE] Sending handshake request (attempt " 
                      << (attempts + 1) << ")" << std::endl;
        }

        // Check for response
        sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);
        memset(buffer, 0, BUFFER_SIZE);
        
        int recv_len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                (sockaddr*)&sender_addr, &sender_len);
        
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            std::string received(buffer);
            
            if (received.find(MSG_HANDSHAKE) == 0) {
                handshake_received = true;
                // Send ready confirmation
                std::string ready_msg = MSG_READY + ":" + NODE_ID;
                sendto(sock, ready_msg.c_str(), ready_msg.length(), 0,
                       (sockaddr*)&peer_addr, sizeof(peer_addr));
            }
            else if (received.find(MSG_READY) == 0) {
                handshake_sent = true;
            }
        }

        attempts++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (handshake_sent && handshake_received) {
        std::cout << "[HANDSHAKE] Handshake complete! Connection established." << std::endl;
        return true;
    }

    std::cout << "[HANDSHAKE] Handshake failed after " << attempts << " attempts" << std::endl;
    return false;
}

/**
 * @brief Receive thread - handles incoming messages from peer
 * @param sock Chat socket descriptor
 */
void receiveThread(SOCKET sock) {
    char buffer[BUFFER_SIZE];
    sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);

    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                (sockaddr*)&sender_addr, &sender_len);
        
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            std::string received(buffer);
            
            // Parse message type
            if (received.find(MSG_CHAT) == 0) {
                std::string chat_content = received.substr(MSG_CHAT.length() + 1);
                std::cout << "\n[" << PEER_ID << "] " << chat_content << std::endl;
                std::cout << "[You] ";
                std::cout.flush();
            }
            else if (received.find(MSG_KEEPALIVE) == 0) {
                // Keepalive received - connection still active
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/**
 * @brief Send thread - handles outgoing messages and keepalives
 * @param sock Chat socket descriptor
 */
void sendThread(SOCKET sock) {
    sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peer_address.c_str());
    peer_addr.sin_port = htons(peer_port);

    std::string input;
    auto last_keepalive = std::chrono::steady_clock::now();

    std::cout << "\n[INFO] Chat ready! Type messages and press Enter to send." << std::endl;
    std::cout << "[INFO] Type 'quit' to exit.\n" << std::endl;

    while (running) {
        // Send keepalive every 5 seconds
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_keepalive).count() >= 5) {
            std::string keepalive = MSG_KEEPALIVE + ":" + NODE_ID;
            sendto(sock, keepalive.c_str(), keepalive.length(), 0,
                   (sockaddr*)&peer_addr, sizeof(peer_addr));
            last_keepalive = now;
        }

        // Check for user input (non-blocking)
        std::cout << "[You] ";
        std::getline(std::cin, input);
        
        if (input == "quit") {
            running = false;
            break;
        }
        
        if (!input.empty()) {
            std::string chat_msg = MSG_CHAT + ":" + input;
            int sent = sendto(sock, chat_msg.c_str(), chat_msg.length(), 0,
                              (sockaddr*)&peer_addr, sizeof(peer_addr));
            if (sent == SOCKET_ERROR) {
                std::cout << "[ERROR] Failed to send message" << std::endl;
            }
        }
    }
}

/**
 * @brief Main function - orchestrates the chat application
 * @return 0 on success, 1 on error
 */
int main() {
    std::cout << "=====================================\n";
    std::cout << "  UDP Chat Node 1 v2 - Auto Discovery\n";
    std::cout << "  Author: Pi Ko (pi.ko@nyu.edu)     \n";
    std::cout << "=====================================\n\n";

    // Initialize Winsock (Windows only)
    if (!initializeWinsock()) {
        return 1;
    }

    // Start discovery thread
    std::thread discovery(discoveryThread);

    // Wait for peer discovery
    while (!peer_discovered && running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!peer_discovered) {
        std::cout << "[ERROR] Peer discovery failed" << std::endl;
        discovery.join();
        cleanupWinsock();
        return 1;
    }

    // Wait a moment for peer to be ready
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Create chat socket
    SOCKET chat_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (chat_sock == INVALID_SOCKET) {
        std::cerr << "[ERROR] Failed to create chat socket" << std::endl;
        running = false;
        discovery.join();
        cleanupWinsock();
        return 1;
    }

    // Set non-blocking mode
    setNonBlocking(chat_sock);

    // Allow socket reuse
    int reuse = 1;
    setsockopt(chat_sock, SOL_SOCKET, SO_REUSEADDR, 
               (const char*)&reuse, sizeof(reuse));

    // Bind to chat port
    sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(CHAT_PORT);

    if (bind(chat_sock, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
        std::cerr << "[ERROR] Failed to bind chat socket to port " << CHAT_PORT << std::endl;
#ifdef _WIN32
        std::cerr << "[ERROR] WSA Error: " << WSAGetLastError() << std::endl;
#endif
        closesocket(chat_sock);
        running = false;
        discovery.join();
        cleanupWinsock();
        return 1;
    }

    // Perform handshake
    if (!performHandshake(chat_sock)) {
        std::cout << "[ERROR] Handshake failed" << std::endl;
        closesocket(chat_sock);
        running = false;
        discovery.join();
        cleanupWinsock();
        return 1;
    }

    handshake_complete = true;

    // Start receive thread
    std::thread receiver(receiveThread, chat_sock);

    // Handle sending in main thread
    sendThread(chat_sock);

    // Cleanup
    running = false;
    discovery.join();
    receiver.join();
    closesocket(chat_sock);
    cleanupWinsock();

    std::cout << "\n[INFO] Chat application terminated" << std::endl;
    return 0;
}
