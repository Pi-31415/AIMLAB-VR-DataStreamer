/**
 * @file main_v6.2.cpp
 * @brief AIMLAB VR Data Streamer v6.2 - WITH PROGRESS INDICATORS
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 04 November 2025
 * @version v6.2
 * 
 * Changelog:
 * - v6.2 (04 November 2025): Added progress indicators, enhanced debugging, status messages
 * - v6.1 (04 November 2025): Fixed Arduino status display, timer countdown, VR discovery
 * - v6.0 (04 November 2025): Complete VR data streaming integration
 * 
 * NEW IN v6.2:
 *   - Progress bars for Arduino COM port scanning
 *   - Progress bars for Unity VR discovery
 *   - Real-time status messages during operations
 *   - 30-second timeout for manual refresh operations
 *   - Enhanced debug logging with timestamps
 *   - Auto-reset of progress indicators after completion
 *   - Thread-safe debug output
 * 
 * Description:
 *   Unified GUI application for controlling VR experiments with progress tracking
 * 
 * Build: 
 *   Run build_v6.2.bat or:
 *   g++ main_v6.2.cpp imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp ^
 *       backends/imgui_impl_sdl2.cpp backends/imgui_impl_sdlrenderer2.cpp ^
 *       -I. -Ibackends -ISDL2/x86_64-w64-mingw32/include/SDL2 ^
 *       -LSDL2/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lgdi32 -lws2_32 ^
 *       -o AIMLAB_VR_DataStreamer_v6.2.exe -std=c++17 -mwindows
 */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <SDL.h>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <queue>
#include <iomanip>
#include <ctime>
#include <algorithm>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#pragma comment(lib, "ws2_32.lib")

namespace fs = std::filesystem;

// ============================================================================
// Global Configuration
// ============================================================================

const int VR_PORT = 55000;
const int DISCOVERY_PORT = 55001;
const int DISCOVERY_TIMEOUT = 20;  // seconds for auto-discovery
const int MANUAL_DISCOVERY_TIMEOUT = 30;  // seconds for manual refresh
const int WINDOW_WIDTH = 650;
const int WINDOW_HEIGHT = 550;

// NYU Purple color scheme
const ImVec4 NYU_PURPLE = ImVec4(0.33f, 0.18f, 0.66f, 1.0f);  // #542AA8
const ImVec4 NYU_PURPLE_LIGHT = ImVec4(0.45f, 0.30f, 0.78f, 1.0f);
const ImVec4 NYU_PURPLE_DARK = ImVec4(0.25f, 0.10f, 0.55f, 1.0f);
const ImVec4 BACKGROUND_COLOR = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
const ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
const ImVec4 GREEN = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
const ImVec4 RED = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);
const ImVec4 ORANGE = ImVec4(0.8f, 0.5f, 0.0f, 1.0f);
const ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

// ============================================================================
// Global State Variables
// ============================================================================

// Fonts
ImFont* fontDefault = nullptr;
ImFont* fontLarge = nullptr;
ImFont* fontSmall = nullptr;

// Arduino Connection
std::atomic<bool> arduinoConnected(false);
std::atomic<bool> arduinoBusy(false);
std::atomic<float> arduinoProgress(0.0f);
std::string arduinoStatusMessage = "";
HANDLE hSerial = INVALID_HANDLE_VALUE;
std::string arduinoPort = "N/A";
std::mutex arduinoMutex;
std::chrono::steady_clock::time_point arduinoSearchStart;

// Unity Connection
std::atomic<bool> unityConnected(false);
std::atomic<bool> unityBusy(false);
std::atomic<float> unityProgress(0.0f);
std::string unityStatusMessage = "";
SOCKET vrSocket = INVALID_SOCKET;
std::string unityIP = "N/A";
std::thread unityReceiveThread;
std::mutex unityMutex;
std::chrono::steady_clock::time_point unitySearchStart;

// Data Streaming
std::atomic<bool> streaming(false);
std::atomic<bool> recording(false);
std::ofstream dataFile;
std::string currentFilename = "";
char filenameBuffer[256] = "experiment_data";
std::queue<std::string> dataQueue;
std::mutex dataQueueMutex;

// Statistics
std::atomic<int> packetsReceived(0);
std::atomic<int> packetsProcessed(0);
std::atomic<int> linesWritten(0);
std::chrono::steady_clock::time_point streamStartTime;

// Auto-discovery
std::atomic<bool> autoDiscoveryRunning(false);
std::atomic<int> discoveryTimeRemaining(0);
std::thread autoDiscoveryThread;
std::chrono::steady_clock::time_point discoveryStartTime;

// ============================================================================
// Forward Declarations
// ============================================================================

bool connectUnity(const std::string& ip);
void debugLog(const std::string& message);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Thread-safe debug logging with timestamps
 */
void debugLog(const std::string& message) {
    static std::mutex logMutex;
    std::lock_guard<std::mutex> lock(logMutex);
    
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);
    
    std::cout << std::put_time(&bt, "%H:%M:%S");
    std::cout << '.' << std::setfill('0') << std::setw(3) << ms.count();
    std::cout << " | " << message << std::endl;
    std::cout.flush();
}

/**
 * @brief Generate unique filename by appending numbers if file exists
 */
std::string generateUniqueFilename(const std::string& baseName) {
    std::string filename = baseName + ".csv";
    if (!fs::exists(filename)) {
        return filename;
    }
    
    int counter = 1;
    while (counter < 1000) {
        filename = baseName + "_" + std::to_string(counter) + ".csv";
        if (!fs::exists(filename)) {
            return filename;
        }
        counter++;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << baseName << "_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".csv";
    return ss.str();
}

/**
 * @brief Initialize Winsock for network communication
 */
bool initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        debugLog("ERROR: WSAStartup failed with code " + std::to_string(result));
        return false;
    }
    debugLog("Winsock initialized successfully");
    return true;
}

// ============================================================================
// Arduino Communication Functions
// ============================================================================

/**
 * @brief List available COM ports
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
 * @brief Read line from serial port
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
 * @brief Handshake with Arduino
 */
bool handshakeArduino(HANDLE serial) {
    EscapeCommFunction(serial, CLRDTR);
    Sleep(250);
    EscapeCommFunction(serial, SETDTR);
    Sleep(2000);
    
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
 * @brief Connect to Arduino with progress tracking
 */
void connectArduino(bool isManual = false) {
    debugLog("=== Arduino Connection Attempt Started ===");
    
    if (isManual) {
        arduinoSearchStart = std::chrono::steady_clock::now();
        arduinoStatusMessage = "Initializing search...";
        arduinoProgress = 0.0f;
    }
    
    std::lock_guard<std::mutex> lock(arduinoMutex);
    
    // Disconnect if already connected
    if (arduinoConnected) {
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
        }
        arduinoConnected = false;
        arduinoPort = "N/A";
        debugLog("Disconnected existing Arduino connection");
        return;
    }
    
    std::vector<std::string> ports = listCOMPorts();
    debugLog("Found " + std::to_string(ports.size()) + " COM ports to scan");
    
    if (ports.empty()) {
        debugLog("WARNING: No COM ports found!");
        if (isManual) {
            arduinoProgress = 1.0f;
            arduinoStatusMessage = "No COM ports available";
        }
        return;
    }
    
    float progressStep = 1.0f / ports.size();
    float currentProgress = 0.0f;
    
    for (size_t idx = 0; idx < ports.size(); idx++) {
        const auto& port = ports[idx];
        
        if (isManual) {
            currentProgress = (float)(idx + 1) / (float)ports.size();
            arduinoProgress = currentProgress;
            
            // Extract port name for display
            size_t pos = port.find("COM");
            std::string portDisplay = (pos != std::string::npos) ? port.substr(pos) : port;
            arduinoStatusMessage = "Checking " + portDisplay + "... [" + 
                                 std::to_string(idx + 1) + "/" + std::to_string(ports.size()) + "]";
        }
        
        debugLog("Checking port: " + port);
        
        HANDLE h = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_EXISTING, 0, NULL);
        if (h == INVALID_HANDLE_VALUE) {
            debugLog("  - Could not open port");
            continue;
        }
        
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
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fDsrSensitivity = FALSE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        dcb.fErrorChar = FALSE;
        dcb.fNull = FALSE;
        dcb.fAbortOnError = FALSE;
        
        if (!SetCommState(h, &dcb)) {
            debugLog("  - Failed to set comm state");
            CloseHandle(h);
            continue;
        }
        
        COMMTIMEOUTS timeouts;
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.ReadTotalTimeoutConstant = 100;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 100;
        
        if (!SetCommTimeouts(h, &timeouts)) {
            debugLog("  - Failed to set timeouts");
            CloseHandle(h);
            continue;
        }
        
        PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
        
        debugLog("  - Attempting handshake...");
        if (handshakeArduino(h)) {
            hSerial = h;
            arduinoConnected = true;
            size_t pos = port.find("COM");
            arduinoPort = (pos != std::string::npos) ? port.substr(pos) : port;
            
            debugLog("SUCCESS: Connected to Arduino on " + arduinoPort);
            
            if (isManual) {
                arduinoProgress = 1.0f;
                arduinoStatusMessage = "Connected to " + arduinoPort + "!";
            }
            return;
        } else {
            debugLog("  - Handshake failed");
        }
        CloseHandle(h);
    }
    
    arduinoConnected = false;
    arduinoPort = "N/A";
    debugLog("FAILED: No Arduino found on any COM port");
    
    if (isManual) {
        arduinoProgress = 1.0f;
        arduinoStatusMessage = "No vibration motor found";
    }
}

/**
 * @brief Send test vibration command
 */
void testVibration() {
    if (!arduinoConnected || hSerial == INVALID_HANDLE_VALUE) {
        debugLog("Test vibration ignored - Arduino not connected");
        return;
    }
    
    const char cmd = '1';
    DWORD written;
    WriteFile(hSerial, &cmd, 1, &written, NULL);
    WriteFile(hSerial, "\n", 1, &written, NULL);
    debugLog("Test vibration command sent");
}

// ============================================================================
// Unity VR Communication Functions
// ============================================================================

/**
 * @brief Receive data from Unity
 */
void receiveDataThread() {
    debugLog("Unity receive thread started");
    char buffer[1024];
    
    while (unityConnected && vrSocket != INVALID_SOCKET) {
        int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0) {
            buffer[result] = '\0';
            packetsReceived++;
            
            std::string data(buffer);
            data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
            data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
            
            std::lock_guard<std::mutex> lock(dataQueueMutex);
            dataQueue.push(data);
        } else if (result == 0) {
            debugLog("Unity connection closed gracefully");
            break;
        } else {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                debugLog("Unity receive error: " + std::to_string(error));
                break;
            }
        }
        
        Sleep(10);
    }
    
    unityConnected = false;
    debugLog("Unity receive thread ended");
}

/**
 * @brief Listen for Unity discovery broadcast with progress
 */
bool discoverUnity(int timeoutSeconds, bool isManual = false) {
    debugLog("=== Unity Discovery Started (timeout: " + std::to_string(timeoutSeconds) + "s) ===");
    
    if (isManual) {
        unitySearchStart = std::chrono::steady_clock::now();
        unityStatusMessage = "Initializing discovery...";
        unityProgress = 0.0f;
    }
    
    SOCKET discoverySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (discoverySocket == INVALID_SOCKET) {
        debugLog("ERROR: Failed to create discovery socket: " + std::to_string(WSAGetLastError()));
        if (isManual) {
            unityStatusMessage = "Socket creation failed";
            unityProgress = 1.0f;
        }
        return false;
    }
    
    // Enable broadcast
    char broadcast = 1;
    setsockopt(discoverySocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    
    // Set reuse address
    char reuse = 1;
    setsockopt(discoverySocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(DISCOVERY_PORT);
    
    if (bind(discoverySocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
        debugLog("ERROR: Failed to bind discovery socket: " + std::to_string(WSAGetLastError()));
        closesocket(discoverySocket);
        if (isManual) {
            unityStatusMessage = "Failed to bind to port " + std::to_string(DISCOVERY_PORT);
            unityProgress = 1.0f;
        }
        return false;
    }
    
    debugLog("Listening for VR headset broadcast on port " + std::to_string(DISCOVERY_PORT));
    
    // Set non-blocking mode
    u_long mode = 1;
    ioctlsocket(discoverySocket, FIONBIO, &mode);
    
    char buffer[256];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        int remaining = timeoutSeconds - elapsedSeconds;
        
        if (!isManual) {
            if (!autoDiscoveryRunning && !unityBusy) break;
            discoveryTimeRemaining = std::max(0, remaining);
        } else {
            unityProgress = (float)elapsedSeconds / (float)timeoutSeconds;
            unityStatusMessage = "Listening for VR headset... (" + std::to_string(remaining) + "s remaining)";
        }
        
        if (remaining <= 0) {
            debugLog("Discovery timeout reached");
            break;
        }
        
        int result = recvfrom(discoverySocket, buffer, sizeof(buffer) - 1, 0,
                             (sockaddr*)&senderAddr, &senderAddrSize);
        
        if (result > 0) {
            buffer[result] = '\0';
            debugLog("Received broadcast: '" + std::string(buffer) + "'");
            
            if (strstr(buffer, "VR_HEADSET_DISCOVERY") != nullptr) {
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(senderAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
                debugLog("FOUND: VR headset at " + std::string(ipStr));
                
                closesocket(discoverySocket);
                
                if (isManual) {
                    unityStatusMessage = "Connecting to " + std::string(ipStr) + "...";
                    unityProgress = 0.9f;
                }
                
                bool success = connectUnity(ipStr);
                
                if (isManual) {
                    unityProgress = 1.0f;
                    if (success) {
                        unityStatusMessage = "Connected to " + std::string(ipStr) + "!";
                    } else {
                        unityStatusMessage = "Connection failed";
                    }
                }
                
                return success;
            }
        }
        
        Sleep(100);
    }
    
    closesocket(discoverySocket);
    debugLog("FAILED: No VR headset found");
    
    if (isManual) {
        unityProgress = 1.0f;
        unityStatusMessage = "No VR headset found";
    }
    
    return false;
}

/**
 * @brief Connect to Unity VR application
 */
bool connectUnity(const std::string& ip) {
    debugLog("Attempting TCP connection to " + ip + ":" + std::to_string(VR_PORT));
    
    std::lock_guard<std::mutex> lock(unityMutex);
    
    // Clean up existing connection
    if (vrSocket != INVALID_SOCKET) {
        unityConnected = false;
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        
        if (unityReceiveThread.joinable()) {
            unityReceiveThread.join();
        }
    }
    
    vrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (vrSocket == INVALID_SOCKET) {
        debugLog("ERROR: Failed to create TCP socket: " + std::to_string(WSAGetLastError()));
        return false;
    }
    
    // Set timeout for connection attempt
    DWORD timeout = 5000;
    setsockopt(vrSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(vrSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(VR_PORT);
    
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        debugLog("ERROR: Invalid IP address format: " + ip);
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    if (connect(vrSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        debugLog("ERROR: TCP connect failed: " + std::to_string(WSAGetLastError()));
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    debugLog("TCP connection established, sending handshake...");
    
    // Send handshake
    const char* handshake = "DATA_RECEIVER_READY\n";
    if (send(vrSocket, handshake, strlen(handshake), 0) == SOCKET_ERROR) {
        debugLog("ERROR: Failed to send handshake: " + std::to_string(WSAGetLastError()));
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    // Wait for response
    char buffer[256];
    int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
    if (result > 0) {
        buffer[result] = '\0';
        debugLog("Received handshake response: '" + std::string(buffer) + "'");
        
        if (strstr(buffer, "VR_HEADSET_READY")) {
            unityConnected = true;
            unityIP = ip;
            
            // Set socket back to non-blocking for receive thread
            u_long mode = 1;
            ioctlsocket(vrSocket, FIONBIO, &mode);
            
            // Start receive thread
            unityReceiveThread = std::thread(receiveDataThread);
            
            debugLog("SUCCESS: Connected to Unity VR application!");
            return true;
        } else {
            debugLog("ERROR: Invalid handshake response");
        }
    } else {
        debugLog("ERROR: No handshake response received");
    }
    
    closesocket(vrSocket);
    vrSocket = INVALID_SOCKET;
    return false;
}

// ============================================================================
// Data Recording Functions
// ============================================================================

/**
 * @brief Start data recording
 */
bool startRecording(const std::string& filename) {
    if (recording) return false;
    
    currentFilename = generateUniqueFilename(filename);
    dataFile.open(currentFilename, std::ios::out);
    
    if (!dataFile.is_open()) {
        debugLog("ERROR: Failed to create file: " + currentFilename);
        currentFilename = "";
        return false;
    }
    
    // Write CSV header
    dataFile << "Timestamp,HeadPosX,HeadPosY,HeadPosZ,HeadRotX,HeadRotY,HeadRotZ,HeadRotW,"
             << "LeftHandPosX,LeftHandPosY,LeftHandPosZ,LeftHandRotX,LeftHandRotY,LeftHandRotZ,LeftHandRotW,"
             << "RightHandPosX,RightHandPosY,RightHandPosZ,RightHandRotX,RightHandRotY,RightHandRotZ,RightHandRotW"
             << std::endl;
    
    recording = true;
    linesWritten = 0;
    packetsReceived = 0;
    packetsProcessed = 0;
    streamStartTime = std::chrono::steady_clock::now();
    
    // Clear data queue
    {
        std::lock_guard<std::mutex> lock(dataQueueMutex);
        while (!dataQueue.empty()) {
            dataQueue.pop();
        }
    }
    
    debugLog("Recording started: " + currentFilename);
    return true;
}

/**
 * @brief Stop data recording
 */
void stopRecording() {
    if (recording && dataFile.is_open()) {
        dataFile.close();
        debugLog("Recording stopped: " + std::to_string(linesWritten.load()) + " lines written");
    }
    recording = false;
    currentFilename = "";
}

/**
 * @brief Process data queue and write to file
 */
void processDataQueue() {
    if (!recording || !dataFile.is_open()) return;
    
    std::lock_guard<std::mutex> lock(dataQueueMutex);
    
    while (!dataQueue.empty()) {
        std::string data = dataQueue.front();
        dataQueue.pop();
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - streamStartTime).count();
        
        dataFile << elapsed << "," << data << std::endl;
        linesWritten++;
        packetsProcessed++;
    }
}

// ============================================================================
// Auto-Discovery Function
// ============================================================================

/**
 * @brief Run auto-discovery for both Arduino and Unity
 */
void runAutoDiscovery() {
    debugLog("========================================");
    debugLog("AUTO-DISCOVERY STARTED");
    debugLog("========================================");
    
    autoDiscoveryRunning = true;
    discoveryTimeRemaining = DISCOVERY_TIMEOUT;
    discoveryStartTime = std::chrono::steady_clock::now();
    
    // Try Arduino connection
    if (!arduinoConnected) {
        debugLog("Searching for Arduino...");
        arduinoBusy = true;
        connectArduino(false);
        arduinoBusy = false;
    } else {
        debugLog("Arduino already connected");
    }
    
    // Try Unity discovery
    if (!unityConnected) {
        debugLog("Searching for Unity VR...");
        unityBusy = true;
        discoverUnity(DISCOVERY_TIMEOUT, false);
        unityBusy = false;
    } else {
        debugLog("Unity VR already connected");
    }
    
    autoDiscoveryRunning = false;
    discoveryTimeRemaining = 0;
    
    debugLog("========================================");
    debugLog("AUTO-DISCOVERY COMPLETED");
    debugLog("Arduino: " + std::string(arduinoConnected ? "FOUND" : "NOT FOUND"));
    debugLog("Unity VR: " + std::string(unityConnected ? "FOUND" : "NOT FOUND"));
    debugLog("========================================");
}

// ============================================================================
// GUI Functions
// ============================================================================

/**
 * @brief Apply NYU themed style
 */
void applyNYUStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    
    style.WindowPadding = ImVec2(20, 20);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 10);
    style.ItemInnerSpacing = ImVec2(10, 8);
    
    style.ScrollbarSize = 15.0f;
    style.GrabMinSize = 12.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = BACKGROUND_COLOR;
    colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
    colors[ImGuiCol_FrameBg] = WHITE;
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.92f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.87f, 1.00f, 1.00f);
    colors[ImGuiCol_TitleBg] = NYU_PURPLE;
    colors[ImGuiCol_TitleBgActive] = NYU_PURPLE_DARK;
    colors[ImGuiCol_Button] = NYU_PURPLE;
    colors[ImGuiCol_ButtonHovered] = NYU_PURPLE_LIGHT;
    colors[ImGuiCol_ButtonActive] = NYU_PURPLE_DARK;
    colors[ImGuiCol_Header] = NYU_PURPLE_LIGHT;
    colors[ImGuiCol_HeaderHovered] = NYU_PURPLE;
    colors[ImGuiCol_HeaderActive] = NYU_PURPLE_DARK;
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TextDisabled] = GRAY;
    colors[ImGuiCol_CheckMark] = NYU_PURPLE;
    colors[ImGuiCol_SliderGrab] = NYU_PURPLE;
    colors[ImGuiCol_SliderGrabActive] = NYU_PURPLE_DARK;
    colors[ImGuiCol_PlotHistogram] = NYU_PURPLE;
}

/**
 * @brief Load custom fonts
 */
bool loadFonts(ImGuiIO& io) {
    io.Fonts->Clear();
    
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 3;
    fontConfig.OversampleV = 3;
    fontConfig.PixelSnapH = true;
    
    fontDefault = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 16.0f, &fontConfig);
    if (!fontDefault) {
        fontDefault = io.Fonts->AddFontDefault();
    }
    
    fontLarge = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 24.0f, &fontConfig);
    if (!fontLarge) fontLarge = fontDefault;
    
    fontSmall = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 14.0f, &fontConfig);
    if (!fontSmall) fontSmall = fontDefault;
    
    return true;
}

// ============================================================================
// Main Application
// ============================================================================

int main(int argc, char** argv) {
    // Enable console for debugging
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    freopen_s(&pCout, "CONOUT$", "w", stderr);
    std::ios_base::sync_with_stdio(false);
    
    debugLog("========================================");
    debugLog("AIMLAB VR Data Streamer v6.2");
    debugLog("WITH PROGRESS INDICATORS");
    debugLog("Author: Pi Ko (pi.ko@nyu.edu)");
    debugLog("Date: 04 November 2025");
    debugLog("========================================");
    
    // Initialize Winsock
    if (!initializeWinsock()) {
        MessageBoxA(NULL, "Failed to initialize Winsock", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        debugLog("ERROR: SDL_Init failed: " + std::string(SDL_GetError()));
        MessageBoxA(NULL, SDL_GetError(), "SDL2 Initialization Failed", MB_OK | MB_ICONERROR);
        WSACleanup();
        return -1;
    }
    debugLog("SDL2 initialized");
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "AIMLAB VR Data Streamer v6.2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        debugLog("ERROR: Window creation failed");
        MessageBoxA(NULL, SDL_GetError(), "Window Creation Failed", MB_OK | MB_ICONERROR);
        SDL_Quit();
        WSACleanup();
        return -1;
    }
    debugLog("Window created");
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer) {
        debugLog("ERROR: Renderer creation failed");
        MessageBoxA(NULL, SDL_GetError(), "Renderer Creation Failed", MB_OK | MB_ICONERROR);
        SDL_DestroyWindow(window);
        SDL_Quit();
        WSACleanup();
        return -1;
    }
    debugLog("Renderer created");
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    applyNYUStyle();
    
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    loadFonts(io);
    
    debugLog("ImGui initialized");
    
    // Start auto-discovery
    debugLog("Starting auto-discovery thread...");
    autoDiscoveryThread = std::thread(runAutoDiscovery);
    
    // Main loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Process events
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        // Update timer if auto-discovery is running
        if (autoDiscoveryRunning) {
            auto elapsed = std::chrono::steady_clock::now() - discoveryStartTime;
            int remaining = DISCOVERY_TIMEOUT - std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
            discoveryTimeRemaining = std::max(0, remaining);
        }
        
        // Update manual progress timeouts
        if (arduinoBusy && arduinoProgress < 1.0f) {
            auto elapsed = std::chrono::steady_clock::now() - arduinoSearchStart;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= MANUAL_DISCOVERY_TIMEOUT) {
                arduinoProgress = 1.0f;
                arduinoStatusMessage = "Search timeout - No motor found";
                debugLog("Arduino search timed out");
            }
        }
        
        if (unityBusy && unityProgress < 1.0f) {
            auto elapsed = std::chrono::steady_clock::now() - unitySearchStart;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= MANUAL_DISCOVERY_TIMEOUT) {
                unityProgress = 1.0f;
                unityStatusMessage = "Search timeout - No headset found";
                debugLog("Unity search timed out");
            }
        }
        
        // Process data queue if streaming
        if (streaming && recording) {
            processDataQueue();
        }
        
        // Start ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Main window
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT));
            
            ImGui::Begin("AIMLAB VR Data Streamer", nullptr,
                        ImGuiWindowFlags_NoResize | 
                        ImGuiWindowFlags_NoMove | 
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoTitleBar);
            
            // Title
            if (fontLarge) ImGui::PushFont(fontLarge);
            ImGui::TextColored(NYU_PURPLE, "AIMLAB VR Data Streamer v6.2");
            if (fontLarge) ImGui::PopFont();
            
            if (fontSmall) ImGui::PushFont(fontSmall);
            ImGui::TextColored(GRAY, "Developed at AIMLAB, New York University Abu Dhabi");
            if (fontSmall) ImGui::PopFont();
            
            ImGui::Separator();
            ImGui::Spacing();
            
            // Auto-discovery status
            if (autoDiscoveryRunning) {
                ImGui::TextColored(ORANGE, "Auto-discovery in progress... (%d seconds remaining)",
                                  discoveryTimeRemaining.load());
            }
            
            ImGui::Spacing();
            
            // Connection Status Panel
            ImGui::TextColored(NYU_PURPLE, "Connection Status");
            ImGui::Separator();
            
            // Arduino status - FIXED: Single status display
            ImGui::Text("Vibration Motor:");
            ImGui::SameLine(150);
            
            bool isArduinoConnected = arduinoConnected.load();
            if (isArduinoConnected) {
                ImGui::TextColored(GREEN, "CONNECTED [%s]", arduinoPort.c_str());
                ImGui::SameLine();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                draw_list->AddCircleFilled(ImVec2(p.x - 5, p.y + 8), 5.0f, IM_COL32(0, 200, 0, 255));
            } else {
                ImGui::TextColored(RED, "NOT CONNECTED");
                ImGui::SameLine();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                draw_list->AddCircleFilled(ImVec2(p.x - 5, p.y + 8), 5.0f, IM_COL32(200, 0, 0, 255));
            }
            
            // Arduino progress bar - NEW IN v6.2
            if (arduinoBusy && arduinoProgress < 1.0f) {
                ImGui::ProgressBar(arduinoProgress.load(), ImVec2(-1, 0), arduinoStatusMessage.c_str());
            } else if (arduinoBusy && arduinoProgress >= 1.0f) {
                ImGui::TextColored(GRAY, "%s", arduinoStatusMessage.c_str());
                // Auto-reset after 2 seconds
                static auto resetTime = std::chrono::steady_clock::now();
                if (arduinoProgress >= 1.0f) {
                    if (std::chrono::steady_clock::now() - resetTime > std::chrono::seconds(2)) {
                        arduinoBusy = false;
                        arduinoProgress = 0.0f;
                        arduinoStatusMessage = "";
                        resetTime = std::chrono::steady_clock::now();
                    }
                }
            }
            
            // Unity status
            ImGui::Text("VR Headset:");
            ImGui::SameLine(150);
            
            bool isUnityConnected = unityConnected.load();
            if (isUnityConnected) {
                ImGui::TextColored(GREEN, "CONNECTED [%s]", unityIP.c_str());
                ImGui::SameLine();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                draw_list->AddCircleFilled(ImVec2(p.x - 5, p.y + 8), 5.0f, IM_COL32(0, 200, 0, 255));
            } else {
                ImGui::TextColored(RED, "NOT CONNECTED");
                ImGui::SameLine();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                draw_list->AddCircleFilled(ImVec2(p.x - 5, p.y + 8), 5.0f, IM_COL32(200, 0, 0, 255));
            }
            
            // Unity progress bar - NEW IN v6.2
            if (unityBusy && unityProgress < 1.0f) {
                ImGui::ProgressBar(unityProgress.load(), ImVec2(-1, 0), unityStatusMessage.c_str());
            } else if (unityBusy && unityProgress >= 1.0f) {
                ImGui::TextColored(GRAY, "%s", unityStatusMessage.c_str());
                // Auto-reset after 2 seconds
                static auto resetTime = std::chrono::steady_clock::now();
                if (unityProgress >= 1.0f) {
                    if (std::chrono::steady_clock::now() - resetTime > std::chrono::seconds(2)) {
                        unityBusy = false;
                        unityProgress = 0.0f;
                        unityStatusMessage = "";
                        resetTime = std::chrono::steady_clock::now();
                    }
                }
            }
            
            ImGui::Spacing();
            
            // Control Buttons
            ImGui::TextColored(NYU_PURPLE, "Controls");
            ImGui::Separator();
            
            // Refresh buttons
            if (ImGui::Button("Refresh Vibration Motor", ImVec2(180, 35)) && 
                !arduinoBusy && !autoDiscoveryRunning) {
                debugLog("=== MANUAL Arduino Refresh Requested ===");
                arduinoBusy = true;
                arduinoProgress = 0.0f;
                std::thread([]() {
                    connectArduino(true);
                }).detach();
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Refresh VR Headset", ImVec2(180, 35)) && 
                !unityBusy && !autoDiscoveryRunning) {
                debugLog("=== MANUAL Unity Refresh Requested ===");
                unityBusy = true;
                unityProgress = 0.0f;
                std::thread([]() {
                    discoverUnity(MANUAL_DISCOVERY_TIMEOUT, true);
                }).detach();
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Test Vibration", ImVec2(140, 35))) {
                std::thread(testVibration).detach();
            }
            
            ImGui::Spacing();
            
            // Data Recording Section
            ImGui::TextColored(NYU_PURPLE, "Data Recording");
            ImGui::Separator();
            
            ImGui::Text("Output Filename:");
            ImGui::SameLine();
            ImGui::PushItemWidth(250);
            ImGui::InputText("##filename", filenameBuffer, sizeof(filenameBuffer));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::Text(".csv");
            
            if (recording) {
                if (ImGui::Button("Stop Recording", ImVec2(150, 35))) {
                    streaming = false;
                    stopRecording();
                }
                ImGui::SameLine();
                ImGui::TextColored(GREEN, "Recording to: %s", currentFilename.c_str());
            } else {
                bool canStart = isUnityConnected && strlen(filenameBuffer) > 0;
                if (!canStart) {
                    ImGui::BeginDisabled();
                }
                
                if (ImGui::Button("Start Recording", ImVec2(150, 35))) {
                    if (startRecording(filenameBuffer)) {
                        streaming = true;
                    }
                }
                
                if (!canStart) {
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextColored(GRAY, "Connect VR headset and enter filename to start");
                }
            }
            
            ImGui::Spacing();
            
            // Statistics
            ImGui::TextColored(NYU_PURPLE, "Statistics");
            ImGui::Separator();
            
            if (streaming) {
                auto elapsed = std::chrono::steady_clock::now() - streamStartTime;
                auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
                
                ImGui::Text("Recording Time: %02d:%02d:%02d", 
                           (int)(seconds / 3600), (int)((seconds % 3600) / 60), (int)(seconds % 60));
                ImGui::Text("Packets Received: %d", packetsReceived.load());
                ImGui::Text("Packets Processed: %d", packetsProcessed.load());
                ImGui::Text("Lines Written: %d", linesWritten.load());
                
                int queueSize = 0;
                {
                    std::lock_guard<std::mutex> lock(dataQueueMutex);
                    queueSize = dataQueue.size();
                }
                ImGui::Text("Queue Size: %d", queueSize);
            } else {
                ImGui::TextColored(GRAY, "Not recording");
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            
            // Footer
            if (fontSmall) ImGui::PushFont(fontSmall);
            ImGui::TextColored(GRAY, "Version 6.2 - November 2025 - WITH PROGRESS INDICATORS");
            if (fontSmall) ImGui::PopFont();
            
            ImGui::End();
        }
        
        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 238, 238, 238, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
    debugLog("========================================");
    debugLog("SHUTDOWN INITIATED");
    debugLog("========================================");
    
    autoDiscoveryRunning = false;
    streaming = false;
    
    if (autoDiscoveryThread.joinable()) {
        autoDiscoveryThread.join();
    }
    
    if (recording) {
        stopRecording();
    }
    
    if (arduinoConnected && hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        debugLog("Arduino connection closed");
    }
    
    if (unityConnected) {
        unityConnected = false;
        if (vrSocket != INVALID_SOCKET) {
            closesocket(vrSocket);
        }
        if (unityReceiveThread.joinable()) {
            unityReceiveThread.join();
        }
        debugLog("Unity connection closed");
    }
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    WSACleanup();
    
    debugLog("Shutdown complete. Goodbye!");
    debugLog("========================================");
    
    FreeConsole();
    
    return 0;
}

