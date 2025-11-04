/**
 * @file aimlab_vr_datastreamer.cpp
 * @brief AIMLAB-VR-Data Streamer - C++ Node
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 2025
 * 
 * C++ application that receives data from Unity VR application via UDP,
 * handles file operations, and logs received data to files.
 * Features auto-discovery, command processing, and real-time data streaming.
 * 
 * Compilation: g++ -o aimlab_streamer.exe aimlab_vr_datastreamer.cpp -lws2_32 -std=c++11
 * Usage: ./aimlab_streamer.exe
 */

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <vector>
#include <mutex>
#include <queue>
#include <ctime>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <direct.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define mkdir(dir, mode) _mkdir(dir)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

// Configuration constants
const int DISCOVERY_BASE_PORT = 45000;     // Base port for discovery
const int DATA_PORT = 45100;               // Port for data communication
const int BUFFER_SIZE = 4096;              // Message buffer size
const std::string NODE_ID = "AIMLAB_CPP";  // C++ node identifier
const std::string PEER_ID = "AIMLAB_UNITY"; // Unity node identifier
const std::string DATA_DIR = "aimlab_data"; // Directory for data files

// Protocol message types
const std::string MSG_DISCOVER = "DISCOVER";
const std::string MSG_ACKNOWLEDGE = "ACK";
const std::string MSG_HANDSHAKE = "HANDSHAKE";
const std::string MSG_READY = "READY";
const std::string MSG_COMMAND = "CMD";
const std::string MSG_DATA = "DATA";
const std::string MSG_STATUS = "STATUS";
const std::string MSG_KEEPALIVE = "KEEPALIVE";

// Command types
const std::string CMD_OPEN_FILE = "OPEN_FILE";
const std::string CMD_CLOSE_FILE = "CLOSE_FILE";
const std::string CMD_STATUS = "GET_STATUS";
const std::string CMD_SHUTDOWN = "SHUTDOWN";

// Global state management
std::atomic<bool> peer_discovered(false);
std::atomic<bool> handshake_complete(false);
std::atomic<bool> running(true);
std::atomic<bool> file_open(false);
std::atomic<int> data_count(0);
std::atomic<int> file_count(0);

// Network information
std::string peer_address;
int peer_port = 0;
int actual_discovery_port = DISCOVERY_BASE_PORT;

// File handling
std::ofstream current_file;
std::string current_filename;
std::mutex file_mutex;
std::mutex console_mutex;

// Message queue for thread-safe logging
std::queue<std::string> message_queue;
std::mutex queue_mutex;

/**
 * @brief Log message with timestamp
 * @param level Log level (INFO, WARNING, ERROR, DATA)
 * @param message Message to log
 */
void logMessage(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> lock(console_mutex);
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_time;
#ifdef _WIN32
    localtime_s(&tm_time, &time_t);
#else
    localtime_r(&time_t, &tm_time);
#endif
    
    std::cout << "[" << std::put_time(&tm_time, "%H:%M:%S") << "] "
              << "[" << level << "] " << message << std::endl;
}

/**
 * @brief Initialize Windows Socket API (Windows only)
 * @return true if successful, false otherwise
 */
bool initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        logMessage("ERROR", "WSAStartup failed: " + std::to_string(result));
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
 * @brief Create data directory if it doesn't exist
 */
void createDataDirectory() {
    struct stat info;
    if (stat(DATA_DIR.c_str(), &info) != 0) {
        mkdir(DATA_DIR.c_str(), 0777);
        logMessage("INFO", "Created data directory: " + DATA_DIR);
    }
}

/**
 * @brief Generate automatic filename with timestamp
 * @return Generated filename
 */
std::string generateFilename() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_time;
#ifdef _WIN32
    localtime_s(&tm_time, &time_t);
#else
    localtime_r(&time_t, &tm_time);
#endif
    
    std::stringstream ss;
    ss << DATA_DIR << "/aimlab_session_"
       << std::put_time(&tm_time, "%Y%m%d_%H%M%S")
       << "_" << file_count.fetch_add(1) << ".csv";
    
    return ss.str();
}

/**
 * @brief Open a new data file for writing
 * @param filename Optional filename, auto-generates if empty
 * @return true if successful, false otherwise
 */
bool openDataFile(const std::string& filename = "") {
    std::lock_guard<std::mutex> lock(file_mutex);
    
    if (file_open) {
        logMessage("WARNING", "File already open: " + current_filename);
        return false;
    }
    
    createDataDirectory();
    
    current_filename = filename.empty() ? generateFilename() : DATA_DIR + "/" + filename;
    current_file.open(current_filename, std::ios::out | std::ios::app);
    
    if (!current_file.is_open()) {
        logMessage("ERROR", "Failed to open file: " + current_filename);
        return false;
    }
    
    // Write CSV header
    current_file << "Timestamp,Type,Data" << std::endl;
    
    file_open = true;
    data_count = 0;
    logMessage("INFO", "Opened data file: " + current_filename);
    
    return true;
}

/**
 * @brief Close the current data file
 */
void closeDataFile() {
    std::lock_guard<std::mutex> lock(file_mutex);
    
    if (!file_open) {
        logMessage("WARNING", "No file is currently open");
        return;
    }
    
    current_file.close();
    logMessage("INFO", "Closed data file: " + current_filename + 
               " (wrote " + std::to_string(data_count.load()) + " entries)");
    
    file_open = false;
    current_filename.clear();
}

/**
 * @brief Write data to the current file
 * @param type Data type/category
 * @param data Data content
 */
void writeData(const std::string& type, const std::string& data) {
    std::lock_guard<std::mutex> lock(file_mutex);
    
    if (!file_open) {
        logMessage("WARNING", "Cannot write data - no file open");
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    current_file << ms << "," << type << "," << data << std::endl;
    current_file.flush();  // Ensure data is written immediately
    
    data_count++;
    
    if (data_count % 100 == 0) {
        logMessage("DATA", "Written " + std::to_string(data_count.load()) + 
                   " entries to " + current_filename);
    }
}

/**
 * @brief Enable broadcast capability on a socket
 */
bool enableBroadcast(SOCKET sock) {
    int broadcast_enable = 1;
    return setsockopt(sock, SOL_SOCKET, SO_BROADCAST, 
                      (const char*)&broadcast_enable, sizeof(broadcast_enable)) != SOCKET_ERROR;
}

/**
 * @brief Set socket to non-blocking mode
 */
bool setNonBlocking(SOCKET sock) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(sock, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(sock, F_GETFL, 0);
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
#endif
}

/**
 * @brief Try to bind to a discovery port
 */
int tryBindDiscoveryPort(SOCKET sock) {
    sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    
    for (int port_offset = 0; port_offset < 10; port_offset++) {
        int try_port = DISCOVERY_BASE_PORT + port_offset;
        local_addr.sin_port = htons(try_port);
        
        if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) != SOCKET_ERROR) {
            logMessage("INFO", "Bound to discovery port " + std::to_string(try_port));
            return try_port;
        }
    }
    
    return -1;
}

/**
 * @brief Discovery thread - broadcasts presence and listens for Unity peer
 */
void discoveryThread() {
    SOCKET send_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock == INVALID_SOCKET) {
        logMessage("ERROR", "Failed to create send socket");
        return;
    }
    
    enableBroadcast(send_sock);
    setNonBlocking(send_sock);
    
    SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_sock == INVALID_SOCKET) {
        logMessage("ERROR", "Failed to create receive socket");
        closesocket(send_sock);
        return;
    }
    
    int reuse = 1;
    setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, 
               (const char*)&reuse, sizeof(reuse));
    setNonBlocking(recv_sock);
    
    actual_discovery_port = tryBindDiscoveryPort(recv_sock);
    bool can_receive = (actual_discovery_port != -1);
    
    if (!can_receive) {
        logMessage("WARNING", "Could not bind discovery port - broadcast only mode");
        closesocket(recv_sock);
        recv_sock = INVALID_SOCKET;
    }
    
    logMessage("INFO", "Discovery service started");
    logMessage("INFO", "Looking for Unity peer: " + PEER_ID);
    
    sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    char buffer[BUFFER_SIZE];
    auto last_broadcast = std::chrono::steady_clock::now();
    
    while (running && !peer_discovered) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_broadcast).count() >= 2) {
            std::string discover_msg = MSG_DISCOVER + ":" + NODE_ID + ":" + 
                                       std::to_string(DATA_PORT) + ":" + 
                                       std::to_string(actual_discovery_port);
            
            for (int port_offset = 0; port_offset < 10; port_offset++) {
                broadcast_addr.sin_port = htons(DISCOVERY_BASE_PORT + port_offset);
                sendto(send_sock, discover_msg.c_str(), discover_msg.length(), 0,
                       (sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
            }
            
            logMessage("DISCOVERY", "Broadcasting presence...");
            last_broadcast = now;
        }
        
        if (recv_sock != INVALID_SOCKET) {
            sockaddr_in sender_addr;
            socklen_t sender_len = sizeof(sender_addr);
            memset(buffer, 0, BUFFER_SIZE);
            
            int recv_len = recvfrom(recv_sock, buffer, BUFFER_SIZE - 1, 0,
                                    (sockaddr*)&sender_addr, &sender_len);
            
            if (recv_len > 0) {
                buffer[recv_len] = '\0';
                std::string received(buffer);
                
                std::istringstream iss(received);
                std::string msg_type, sender_id, sender_port_str, sender_disc_port;
                std::getline(iss, msg_type, ':');
                std::getline(iss, sender_id, ':');
                std::getline(iss, sender_port_str, ':');
                std::getline(iss, sender_disc_port, ':');
                
                if (msg_type == MSG_DISCOVER && sender_id == PEER_ID) {
                    peer_address = inet_ntoa(sender_addr.sin_addr);
                    peer_port = std::stoi(sender_port_str);
                    
                    logMessage("DISCOVERY", "Unity peer found at " + peer_address + 
                               ":" + std::to_string(peer_port));
                    
                    if (!sender_disc_port.empty()) {
                        int peer_disc_port = std::stoi(sender_disc_port);
                        sockaddr_in peer_disc_addr;
                        memset(&peer_disc_addr, 0, sizeof(peer_disc_addr));
                        peer_disc_addr.sin_family = AF_INET;
                        peer_disc_addr.sin_addr = sender_addr.sin_addr;
                        peer_disc_addr.sin_port = htons(peer_disc_port);
                        
                        std::string ack_msg = MSG_ACKNOWLEDGE + ":" + NODE_ID + ":" + 
                                              std::to_string(DATA_PORT);
                        sendto(send_sock, ack_msg.c_str(), ack_msg.length(), 0,
                               (sockaddr*)&peer_disc_addr, sizeof(peer_disc_addr));
                    }
                    
                    peer_discovered = true;
                }
                else if (msg_type == MSG_ACKNOWLEDGE && sender_id == PEER_ID) {
                    peer_address = inet_ntoa(sender_addr.sin_addr);
                    peer_port = std::stoi(sender_port_str);
                    
                    logMessage("DISCOVERY", "Acknowledgment from Unity at " + 
                               peer_address + ":" + std::to_string(peer_port));
                    
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
    logMessage("INFO", "Discovery service stopped");
}

/**
 * @brief Process received command
 * @param command Command string
 * @return Response string
 */
std::string processCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd_type;
    std::getline(iss, cmd_type, ':');
    
    if (cmd_type == CMD_OPEN_FILE) {
        std::string filename;
        std::getline(iss, filename);
        
        if (openDataFile(filename)) {
            return MSG_STATUS + ":FILE_OPENED:" + current_filename;
        } else {
            return MSG_STATUS + ":FILE_OPEN_FAILED";
        }
    }
    else if (cmd_type == CMD_CLOSE_FILE) {
        closeDataFile();
        return MSG_STATUS + ":FILE_CLOSED";
    }
    else if (cmd_type == CMD_STATUS) {
        std::stringstream ss;
        ss << MSG_STATUS << ":RUNNING:";
        ss << "file_open=" << (file_open.load() ? "true" : "false") << ",";
        ss << "filename=" << (file_open.load() ? current_filename : "none") << ",";
        ss << "data_count=" << data_count.load() << ",";
        ss << "files_created=" << file_count.load();
        return ss.str();
    }
    else if (cmd_type == CMD_SHUTDOWN) {
        if (file_open) {
            closeDataFile();
        }
        running = false;
        return MSG_STATUS + ":SHUTTING_DOWN";
    }
    
    return MSG_STATUS + ":UNKNOWN_COMMAND";
}

/**
 * @brief Data receive thread - handles incoming data and commands
 */
void dataReceiveThread(SOCKET sock) {
    char buffer[BUFFER_SIZE];
    sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    
    sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peer_address.c_str());
    peer_addr.sin_port = htons(peer_port);
    
    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                (sockaddr*)&sender_addr, &sender_len);
        
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            std::string received(buffer);
            
            // Parse message type
            std::istringstream iss(received);
            std::string msg_type;
            std::getline(iss, msg_type, ':');
            
            if (msg_type == MSG_COMMAND) {
                std::string command_data = received.substr(MSG_COMMAND.length() + 1);
                logMessage("COMMAND", "Received: " + command_data);
                
                std::string response = processCommand(command_data);
                sendto(sock, response.c_str(), response.length(), 0,
                       (sockaddr*)&peer_addr, sizeof(peer_addr));
            }
            else if (msg_type == MSG_DATA) {
                std::string data_content = received.substr(MSG_DATA.length() + 1);
                
                // Parse data type and content
                std::istringstream data_iss(data_content);
                std::string data_type, data_value;
                std::getline(data_iss, data_type, ':');
                std::getline(data_iss, data_value);
                
                // Write to file if open
                if (file_open) {
                    writeData(data_type, data_value);
                }
                
                // Log every 10th data message to console
                static int console_log_count = 0;
                if (++console_log_count % 10 == 0) {
                    logMessage("DATA", "Received: " + data_type + " = " + data_value);
                }
            }
            else if (msg_type == MSG_KEEPALIVE) {
                // Respond to keepalive
                std::string response = MSG_KEEPALIVE + ":" + NODE_ID;
                sendto(sock, response.c_str(), response.length(), 0,
                       (sockaddr*)&peer_addr, sizeof(peer_addr));
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

/**
 * @brief Perform handshake with Unity peer
 */
bool performHandshake(SOCKET sock) {
    logMessage("HANDSHAKE", "Initiating handshake protocol...");
    
    sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peer_address.c_str());
    peer_addr.sin_port = htons(peer_port);
    
    std::string handshake_msg = MSG_HANDSHAKE + ":" + NODE_ID;
    int attempts = 0;
    bool handshake_sent = false;
    bool handshake_received = false;
    
    char buffer[BUFFER_SIZE];
    
    while (attempts < 10 && (!handshake_sent || !handshake_received)) {
        if (!handshake_sent) {
            sendto(sock, handshake_msg.c_str(), handshake_msg.length(), 0,
                   (sockaddr*)&peer_addr, sizeof(peer_addr));
            logMessage("HANDSHAKE", "Sending handshake (attempt " + 
                       std::to_string(attempts + 1) + ")");
        }
        
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
        logMessage("HANDSHAKE", "Handshake complete! Connection established.");
        return true;
    }
    
    logMessage("HANDSHAKE", "Handshake failed after " + 
               std::to_string(attempts) + " attempts");
    return false;
}

/**
 * @brief Display application banner
 */
void displayBanner() {
    std::cout << "\n";
    std::cout << "=========================================\n";
    std::cout << "   AIMLAB-VR-Data Streamer (C++ Node)   \n";
    std::cout << "   Author: Pi Ko (pi.ko@nyu.edu)        \n";
    std::cout << "   Version: 1.0                          \n";
    std::cout << "=========================================\n\n";
}

/**
 * @brief Display status information
 */
void displayStatus() {
    std::cout << "\n--- Current Status ---\n";
    std::cout << "Connected to Unity: " << (handshake_complete.load() ? "Yes" : "No") << "\n";
    std::cout << "File Open: " << (file_open.load() ? "Yes" : "No") << "\n";
    if (file_open) {
        std::cout << "Current File: " << current_filename << "\n";
        std::cout << "Data Entries: " << data_count.load() << "\n";
    }
    std::cout << "Total Files Created: " << file_count.load() << "\n";
    std::cout << "----------------------\n\n";
}

/**
 * @brief Console command thread for local control
 */
void consoleThread() {
    std::string input;
    
    while (running) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "status") {
            displayStatus();
        }
        else if (input == "open") {
            openDataFile();
        }
        else if (input == "close") {
            closeDataFile();
        }
        else if (input == "help") {
            std::cout << "\nAvailable commands:\n";
            std::cout << "  status - Display current status\n";
            std::cout << "  open   - Open a new data file\n";
            std::cout << "  close  - Close current data file\n";
            std::cout << "  quit   - Exit application\n";
            std::cout << "  help   - Show this help message\n\n";
        }
        else if (input == "quit" || input == "exit") {
            running = false;
            break;
        }
        else if (!input.empty()) {
            std::cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }
}

/**
 * @brief Main function
 */
int main() {
    displayBanner();
    
    // Initialize Winsock
    if (!initializeWinsock()) {
        return 1;
    }
    
    // Create data directory
    createDataDirectory();
    
    // Start discovery thread
    std::thread discovery(discoveryThread);
    
    logMessage("INFO", "Waiting for Unity connection...");
    
    // Wait for peer discovery
    while (!peer_discovered && running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    if (!peer_discovered) {
        logMessage("ERROR", "Unity peer discovery failed");
        discovery.join();
        cleanupWinsock();
        return 1;
    }
    
    // Wait for peer to be ready
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Create data socket
    SOCKET data_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (data_sock == INVALID_SOCKET) {
        logMessage("ERROR", "Failed to create data socket");
        running = false;
        discovery.join();
        cleanupWinsock();
        return 1;
    }
    
    setNonBlocking(data_sock);
    
    int reuse = 1;
    setsockopt(data_sock, SOL_SOCKET, SO_REUSEADDR, 
               (const char*)&reuse, sizeof(reuse));
    
    sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(DATA_PORT);
    
    if (bind(data_sock, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
        logMessage("ERROR", "Failed to bind data socket");
        closesocket(data_sock);
        running = false;
        discovery.join();
        cleanupWinsock();
        return 1;
    }
    
    // Perform handshake
    if (!performHandshake(data_sock)) {
        logMessage("ERROR", "Handshake failed");
        closesocket(data_sock);
        running = false;
        discovery.join();
        cleanupWinsock();
        return 1;
    }
    
    handshake_complete = true;
    
    logMessage("INFO", "AIMLAB-VR Data Streamer ready!");
    logMessage("INFO", "Type 'help' for available commands");
    
    // Start threads
    std::thread data_receiver(dataReceiveThread, data_sock);
    std::thread console(consoleThread);
    
    // Keep alive loop
    sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peer_address.c_str());
    peer_addr.sin_port = htons(peer_port);
    
    auto last_keepalive = std::chrono::steady_clock::now();
    
    while (running) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_keepalive).count() >= 5) {
            std::string keepalive = MSG_KEEPALIVE + ":" + NODE_ID;
            sendto(data_sock, keepalive.c_str(), keepalive.length(), 0,
                   (sockaddr*)&peer_addr, sizeof(peer_addr));
            last_keepalive = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Cleanup
    if (file_open) {
        closeDataFile();
    }
    
    running = false;
    discovery.join();
    data_receiver.join();
    console.join();
    closesocket(data_sock);
    cleanupWinsock();
    
    logMessage("INFO", "AIMLAB-VR Data Streamer terminated");
    return 0;
}
