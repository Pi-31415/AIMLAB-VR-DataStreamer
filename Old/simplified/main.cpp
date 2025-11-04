/**
 * @file main.cpp
 * @brief AIMLAB VR Data Streamer - Console Application
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 04 November 2025
 * @version v1.1
 * 
 * Console application for controlling VR experiments:
 * - Arduino vibration motor control via serial
 * - Unity VR headset data streaming via TCP
 * - CSV data recording
 * - Simple console menu interface
 * 
 * Changelog:
 * v1.1 - 04 November 2025 - Fixed Unity connection protocol
 *   - Fixed discovery protocol (receiver now broadcasts, not listens)
 *   - Added direct connection method (connectUnityDirect)
 *   - Added localhost auto-try in autoConnect
 *   - Improved receive thread with better error reporting
 *   - Added connection options menu (discovery/localhost/manual)
 *   - Simplified data reception (no handshake requirement in direct mode)
 * 
 * v1.0 - 04 November 2025 - Initial console implementation
 *   - Auto-connect functionality for Arduino and Unity
 *   - Serial communication with Arduino vibration motor
 *   - TCP socket connection for Unity VR data streaming
 *   - CSV data recording with timestamps
 *   - Interactive console menu interface
 */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <mutex>
#include <chrono>
#include <conio.h>
#include <iomanip>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

// ============================================================================
// Configuration
// ============================================================================

const int VR_PORT = 55000;
const int DISCOVERY_PORT = 55001;
const int DISCOVERY_TIMEOUT = 10;  // seconds

// ============================================================================
// Global Variables
// ============================================================================

// Arduino
std::atomic<bool> arduinoConnected(false);
HANDLE hSerial = INVALID_HANDLE_VALUE;
std::string arduinoPort = "N/A";
std::mutex arduinoMutex;

// Unity
std::atomic<bool> unityConnected(false);
SOCKET vrSocket = INVALID_SOCKET;
std::string unityIP = "N/A";
std::thread receiveThread;
std::mutex unityMutex;
std::atomic<bool> receiveThreadRunning(false);

// Data recording
std::atomic<bool> recording(false);
std::ofstream dataFile;
std::string currentFilename = "";
std::queue<std::string> dataQueue;
std::mutex dataQueueMutex;
std::atomic<int> packetsReceived(0);
std::chrono::steady_clock::time_point recordingStartTime;

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Clears the console screen
 */
void clearScreen() {
    system("cls");
}

/**
 * @brief Prints the application header
 */
void printHeader() {
    std::cout << "========================================\n";
    std::cout << "    AIMLAB VR Data Streamer Console    \n";
    std::cout << "    NYU Abu Dhabi - Version 1.1        \n";
    std::cout << "========================================\n\n";
}

/**
 * @brief Prints current connection and recording status
 */
void printStatus() {
    std::cout << "\n[STATUS]\n";
    std::cout << "Arduino Motor: ";
    if (arduinoConnected) {
        std::cout << "CONNECTED (" << arduinoPort << ")\n";
    } else {
        std::cout << "DISCONNECTED\n";
    }
    
    std::cout << "Unity VR:      ";
    if (unityConnected) {
        std::cout << "CONNECTED (" << unityIP << ")\n";
    } else {
        std::cout << "DISCONNECTED\n";
    }
    
    std::cout << "Recording:     ";
    if (recording) {
        auto elapsed = std::chrono::steady_clock::now() - recordingStartTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        std::cout << "ACTIVE (" << currentFilename << ") - " 
                  << seconds << "s, " << packetsReceived << " packets\n";
    } else {
        std::cout << "INACTIVE\n";
    }
    std::cout << "\n";
}

/**
 * @brief Prints the main menu options
 */
void printMenu() {
    std::cout << "[MENU]\n";
    std::cout << "1. Connect/Refresh Arduino\n";
    std::cout << "2. Connect/Refresh Unity\n";
    std::cout << "3. Test Vibration\n";
    std::cout << "4. Send Message to Unity\n";
    std::cout << "5. Start Recording\n";
    std::cout << "6. Stop Recording\n";
    std::cout << "7. Auto-Connect Both\n";
    std::cout << "8. Disconnect All\n";
    std::cout << "0. Exit\n";
    std::cout << "\nChoice: ";
}

// ============================================================================
// Arduino Functions
// ============================================================================

/**
 * @brief Lists all available COM ports on the system
 * @return Vector of available COM port names
 */
std::vector<std::string> listCOMPorts() {
    std::vector<std::string> ports;
    char portName[32];
    
    for (int i = 1; i <= 40; i++) {
        sprintf(portName, "\\\\.\\COM%d", i);
        HANDLE h = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_EXISTING, 0, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            ports.push_back(portName);
            CloseHandle(h);
        }
    }
    return ports;
}

/**
 * @brief Reads a line from the serial port
 * @param serial Handle to the serial port
 * @param out Output string to store the line
 * @param timeout Timeout in milliseconds
 * @return True if a line was read successfully
 */
bool readSerialLine(HANDLE serial, std::string &out, DWORD timeout = 3000) {
    char buffer[256];
    DWORD bytesRead;
    out.clear();
    DWORD start = GetTickCount();
    
    while (GetTickCount() - start < timeout) {
        if (ReadFile(serial, buffer, 1, &bytesRead, NULL)) {
            if (bytesRead == 1) {
                if (buffer[0] == '\n') {
                    return !out.empty();
                }
                if (buffer[0] != '\r') {
                    out.push_back(buffer[0]);
                }
            }
        }
        Sleep(1);
    }
    return !out.empty();
}

/**
 * @brief Performs handshake with Arduino to verify connection
 * @param serial Handle to the serial port
 * @return True if handshake successful
 */
bool handshakeArduino(HANDLE serial) {
    // Reset Arduino
    EscapeCommFunction(serial, CLRDTR);
    Sleep(250);
    EscapeCommFunction(serial, SETDTR);
    Sleep(2000);  // Wait for Arduino to boot
    
    PurgeComm(serial, PURGE_RXCLEAR);
    
    const char* msg = "HELLO\r\n";
    DWORD bytesWritten;
    if (!WriteFile(serial, msg, (DWORD)strlen(msg), &bytesWritten, NULL)) {
        return false;
    }
    
    FlushFileBuffers(serial);
    
    std::string response;
    if (readSerialLine(serial, response, 4000)) {
        return (response.find("Vibration Motor Controller Ready") != std::string::npos);
    }
    return false;
}

/**
 * @brief Connects to Arduino by scanning COM ports
 */
void connectArduino() {
    std::lock_guard<std::mutex> lock(arduinoMutex);
    
    // Disconnect if connected
    if (arduinoConnected && hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        arduinoConnected = false;
        arduinoPort = "N/A";
        std::cout << "Arduino disconnected.\n";
    }
    
    std::cout << "Scanning COM ports...\n";
    std::vector<std::string> ports = listCOMPorts();
    std::cout << "Found " << ports.size() << " COM ports.\n";
    
    for (const auto& port : ports) {
        std::cout << "Checking " << port << "... ";
        
        HANDLE h = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_EXISTING, 0, NULL);
        if (h == INVALID_HANDLE_VALUE) {
            std::cout << "failed to open.\n";
            continue;
        }
        
        // Configure serial
        DCB dcb = {0};
        dcb.DCBlength = sizeof(dcb);
        GetCommState(h, &dcb);
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
        dcb.fBinary = TRUE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        
        if (!SetCommState(h, &dcb)) {
            CloseHandle(h);
            std::cout << "failed to configure.\n";
            continue;
        }
        
        // Set timeouts
        COMMTIMEOUTS timeouts;
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.ReadTotalTimeoutConstant = 100;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 100;
        
        if (!SetCommTimeouts(h, &timeouts)) {
            CloseHandle(h);
            std::cout << "failed to set timeouts.\n";
            continue;
        }
        
        PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
        
        std::cout << "handshaking... ";
        if (handshakeArduino(h)) {
            hSerial = h;
            arduinoConnected = true;
            size_t pos = port.find("COM");
            arduinoPort = (pos != std::string::npos) ? port.substr(pos) : port;
            std::cout << "SUCCESS!\n";
            std::cout << "Arduino connected on " << arduinoPort << "\n";
            return;
        }
        
        CloseHandle(h);
        std::cout << "not an Arduino.\n";
    }
    
    std::cout << "No Arduino found on any COM port.\n";
}

/**
 * @brief Sends a test vibration command to Arduino
 */
void testVibration() {
    if (!arduinoConnected || hSerial == INVALID_HANDLE_VALUE) {
        std::cout << "Arduino not connected!\n";
        return;
    }
    
    const char cmd = '1';
    DWORD written;
    WriteFile(hSerial, &cmd, 1, &written, NULL);
    WriteFile(hSerial, "\n", 1, &written, NULL);
    std::cout << "Vibration command sent.\n";
}

// ============================================================================
// Unity Functions
// ============================================================================

// Forward declaration
bool connectUnity(const std::string& ip);

/**
 * @brief Thread function to receive data from Unity
 */
void receiveDataThread() {
    char buffer[1024];
    receiveThreadRunning = true;
    
    std::cout << "Receive thread started.\n";
    
    while (unityConnected && vrSocket != INVALID_SOCKET && receiveThreadRunning) {
        int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0) {
            buffer[result] = '\0';
            packetsReceived++;
            
            // Just store the raw data
            if (recording) {
                std::lock_guard<std::mutex> lock(dataQueueMutex);
                dataQueue.push(std::string(buffer));
            }
            
            // Debug output (optional - comment out in production)
            // if (packetsReceived % 100 == 0) {
            //     std::cout << "Received packet #" << packetsReceived << "\n";
            // }
        } else if (result == 0) {
            std::cout << "Connection closed by Unity.\n";
            break;  // Connection closed
        } else {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                std::cout << "Receive error: " << error << "\n";
                break;  // Real error
            }
        }
        Sleep(1);
    }
    
    unityConnected = false;
    receiveThreadRunning = false;
    std::cout << "Receive thread ended.\n";
}

/**
 * @brief Discovers Unity VR headset via UDP broadcast
 * @return True if Unity was discovered and connected
 */
bool discoverUnity() {
    std::cout << "Broadcasting discovery message to find Unity...\n";
    
    SOCKET broadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcastSocket == INVALID_SOCKET) {
        std::cout << "Failed to create broadcast socket.\n";
        return false;
    }
    
    // Enable broadcast
    char broadcast = 1;
    setsockopt(broadcastSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    
    // Set up broadcast address
    sockaddr_in broadcastAddr;
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(DISCOVERY_PORT);
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;
    
    // Send discovery broadcast
    const char* discoveryMsg = "DATA_RECEIVER_DISCOVERY";
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        int remaining = DISCOVERY_TIMEOUT - std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        
        if (remaining <= 0) {
            std::cout << "Discovery timeout.\n";
            break;
        }
        
        // Send broadcast
        sendto(broadcastSocket, discoveryMsg, strlen(discoveryMsg), 0,
               (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        
        std::cout << "Broadcast sent, waiting for Unity response... (" << remaining << "s remaining)\n";
        
        // Listen for response
        char buffer[256];
        sockaddr_in senderAddr;
        int senderAddrSize = sizeof(senderAddr);
        
        // Set receive timeout
        DWORD timeout = 1000;  // 1 second
        setsockopt(broadcastSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        
        int result = recvfrom(broadcastSocket, buffer, sizeof(buffer) - 1, 0,
                             (sockaddr*)&senderAddr, &senderAddrSize);
        
        if (result > 0) {
            buffer[result] = '\0';
            std::cout << "Received response: " << buffer << "\n";
            
            if (strstr(buffer, "VR_HEADSET_HERE")) {
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(senderAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
                std::cout << "Found Unity at " << ipStr << "\n";
                
                closesocket(broadcastSocket);
                return connectUnity(ipStr);
            }
        }
        
        if (_kbhit()) {
            _getch();
            std::cout << "Discovery cancelled.\n";
            break;
        }
        
        Sleep(1000);
    }
    
    closesocket(broadcastSocket);
    return false;
}

/**
 * @brief Connects to Unity VR headset via TCP (with handshake)
 * @param ip IP address of Unity VR headset
 * @return True if connection successful
 */
bool connectUnity(const std::string& ip) {
    std::lock_guard<std::mutex> lock(unityMutex);
    
    // Disconnect if connected
    if (unityConnected) {
        receiveThreadRunning = false;
        unityConnected = false;
        
        if (vrSocket != INVALID_SOCKET) {
            closesocket(vrSocket);
            vrSocket = INVALID_SOCKET;
        }
        
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
        
        std::cout << "Unity disconnected.\n";
    }
    
    std::cout << "Connecting to Unity at " << ip << ":" << VR_PORT << "...\n";
    
    vrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (vrSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket.\n";
        return false;
    }
    
    // Set timeout
    DWORD timeout = 5000;
    setsockopt(vrSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(vrSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(VR_PORT);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);
    
    if (connect(vrSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Failed to connect: " << WSAGetLastError() << "\n";
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    // Send handshake
    const char* handshake = "DATA_RECEIVER_READY\n";
    send(vrSocket, handshake, strlen(handshake), 0);
    
    // Wait for response
    char buffer[256];
    int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
    if (result > 0) {
        buffer[result] = '\0';
        
        if (strstr(buffer, "VR_HEADSET_READY")) {
            unityConnected = true;
            unityIP = ip;
            
            // Set non-blocking
            u_long mode = 1;
            ioctlsocket(vrSocket, FIONBIO, &mode);
            
            // Start receive thread
            receiveThread = std::thread(receiveDataThread);
            
            std::cout << "Unity connected successfully!\n";
            return true;
        }
    }
    
    std::cout << "Handshake failed.\n";
    closesocket(vrSocket);
    vrSocket = INVALID_SOCKET;
    return false;
}

/**
 * @brief Connects directly to Unity VR headset via TCP (simplified, no handshake)
 * @param ip IP address of Unity VR headset (empty for localhost)
 * @return True if connection successful
 */
bool connectUnityDirect(const std::string& ip) {
    std::lock_guard<std::mutex> lock(unityMutex);
    
    // Disconnect if connected
    if (unityConnected) {
        receiveThreadRunning = false;
        unityConnected = false;
        
        if (vrSocket != INVALID_SOCKET) {
            closesocket(vrSocket);
            vrSocket = INVALID_SOCKET;
        }
        
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
    }
    
    std::cout << "Connecting to Unity (direct)...\n";
    
    vrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (vrSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket.\n";
        return false;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(VR_PORT);
    
    // Try localhost first if no IP provided
    std::string connectIP = ip.empty() ? "127.0.0.1" : ip;
    
    if (inet_pton(AF_INET, connectIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cout << "Invalid IP address.\n";
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    std::cout << "Connecting to " << connectIP << ":" << VR_PORT << "...\n";
    
    if (connect(vrSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Failed to connect: " << WSAGetLastError() << "\n";
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    std::cout << "TCP connected, starting data reception...\n";
    
    // Set non-blocking for receive
    u_long mode = 1;
    ioctlsocket(vrSocket, FIONBIO, &mode);
    
    unityConnected = true;
    unityIP = connectIP;
    
    // Start receive thread immediately
    receiveThread = std::thread(receiveDataThread);
    
    std::cout << "Unity connected successfully!\n";
    return true;
}

/**
 * @brief Sends a custom message to Unity
 */
void sendToUnity() {
    if (!unityConnected || vrSocket == INVALID_SOCKET) {
        std::cout << "Unity not connected!\n";
        return;
    }
    
    std::cout << "Enter message to send: ";
    std::string message;
    std::getline(std::cin, message);
    
    message += "\n";
    int result = send(vrSocket, message.c_str(), message.length(), 0);
    if (result == SOCKET_ERROR) {
        std::cout << "Failed to send message.\n";
    } else {
        std::cout << "Message sent.\n";
    }
}

// ============================================================================
// Recording Functions
// ============================================================================

/**
 * @brief Starts recording VR data to CSV file
 */
void startRecording() {
    if (recording) {
        std::cout << "Already recording!\n";
        return;
    }
    
    std::cout << "Enter filename (without .csv): ";
    std::string filename;
    std::getline(std::cin, filename);
    
    if (filename.empty()) filename = "data";
    
    currentFilename = filename + ".csv";
    dataFile.open(currentFilename);
    
    if (!dataFile.is_open()) {
        std::cout << "Failed to open file.\n";
        return;
    }
    
    // Write header
    dataFile << "Timestamp,HeadPosX,HeadPosY,HeadPosZ,HeadRotX,HeadRotY,HeadRotZ,HeadRotW,"
             << "LeftHandPosX,LeftHandPosY,LeftHandPosZ,LeftHandRotX,LeftHandRotY,LeftHandRotZ,LeftHandRotW,"
             << "RightHandPosX,RightHandPosY,RightHandPosZ,RightHandRotX,RightHandRotY,RightHandRotZ,RightHandRotW"
             << std::endl;
    
    recording = true;
    packetsReceived = 0;
    recordingStartTime = std::chrono::steady_clock::now();
    
    std::cout << "Recording started to " << currentFilename << "\n";
    
    // Start processing thread
    std::thread([&]() {
        while (recording) {
            std::lock_guard<std::mutex> lock(dataQueueMutex);
            while (!dataQueue.empty()) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - recordingStartTime).count();
                
                dataFile << elapsed << "," << dataQueue.front() << std::endl;
                dataQueue.pop();
            }
            Sleep(10);
        }
    }).detach();
}

/**
 * @brief Stops recording VR data
 */
void stopRecording() {
    if (!recording) {
        std::cout << "Not recording.\n";
        return;
    }
    
    recording = false;
    Sleep(100);  // Let processing thread finish
    
    if (dataFile.is_open()) {
        dataFile.close();
    }
    
    auto elapsed = std::chrono::steady_clock::now() - recordingStartTime;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    
    std::cout << "Recording stopped. Duration: " << seconds << "s, Packets: " << packetsReceived << "\n";
    currentFilename = "";
}

// ============================================================================
// Main Functions
// ============================================================================

/**
 * @brief Auto-connects to both Arduino and Unity devices
 */
void autoConnect() {
    std::cout << "\nAuto-connecting to devices...\n";
    
    // Try Arduino
    std::cout << "\n[Arduino Connection]\n";
    connectArduino();
    
    // Try Unity
    std::cout << "\n[Unity Connection]\n";
    std::cout << "Trying localhost first...\n";
    
    // First try localhost (common for testing)
    if (connectUnityDirect("127.0.0.1")) {
        return;
    }
    
    std::cout << "Localhost failed. Trying discovery broadcast...\n";
    
    // Then try discovery
    if (!discoverUnity()) {
        std::cout << "Unity auto-discovery failed. Enter IP manually? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 'y' || choice == 'Y') {
            std::cout << "Enter Unity IP address: ";
            std::string ip;
            std::getline(std::cin, ip);
            connectUnityDirect(ip);
        }
    }
}

/**
 * @brief Disconnects all devices and stops recording
 */
void disconnectAll() {
    std::cout << "Disconnecting all devices...\n";
    
    // Stop recording
    if (recording) {
        stopRecording();
    }
    
    // Disconnect Arduino
    if (arduinoConnected && hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        arduinoConnected = false;
        arduinoPort = "N/A";
    }
    
    // Disconnect Unity
    if (unityConnected) {
        receiveThreadRunning = false;
        unityConnected = false;
        
        if (vrSocket != INVALID_SOCKET) {
            closesocket(vrSocket);
            vrSocket = INVALID_SOCKET;
        }
        
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
        
        unityIP = "N/A";
    }
    
    std::cout << "All devices disconnected.\n";
}

/**
 * @brief Main entry point
 */
int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed to initialize Winsock.\n";
        return -1;
    }
    
    clearScreen();
    printHeader();
    
    std::cout << "Starting auto-connect...\n";
    autoConnect();
    
    bool running = true;
    while (running) {
        clearScreen();
        printHeader();
        printStatus();
        printMenu();
        
        char choice = _getch();
        std::cout << choice << "\n\n";
        
        switch (choice) {
            case '1':
                connectArduino();
                break;
                
            case '2':
                std::cout << "\n[Unity Connection Options]\n";
                std::cout << "1. Try automatic discovery (broadcast)\n";
                std::cout << "2. Connect to localhost (127.0.0.1)\n";
                std::cout << "3. Enter IP manually\n";
                std::cout << "Choice: ";
                {
                    char subChoice;
                    std::cin >> subChoice;
                    std::cin.ignore();
                    
                    if (subChoice == '1') {
                        discoverUnity();
                    } else if (subChoice == '2') {
                        connectUnityDirect("127.0.0.1");
                    } else if (subChoice == '3') {
                        std::cout << "Enter Unity IP: ";
                        std::string ip;
                        std::getline(std::cin, ip);
                        connectUnityDirect(ip);
                    } else {
                        std::cout << "Invalid choice.\n";
                    }
                }
                break;
                
            case '3':
                testVibration();
                break;
                
            case '4':
                sendToUnity();
                break;
                
            case '5':
                startRecording();
                break;
                
            case '6':
                stopRecording();
                break;
                
            case '7':
                autoConnect();
                break;
                
            case '8':
                disconnectAll();
                break;
                
            case '0':
                running = false;
                break;
                
            default:
                std::cout << "Invalid choice.\n";
                break;
        }
        
        if (running && choice != '0') {
            std::cout << "\nPress any key to continue...";
            _getch();
        }
    }
    
    // Cleanup
    disconnectAll();
    WSACleanup();
    
    std::cout << "\nGoodbye!\n";
    return 0;
}

