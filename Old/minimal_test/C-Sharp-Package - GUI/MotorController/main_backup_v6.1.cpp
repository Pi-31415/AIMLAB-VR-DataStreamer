/**
 * @file main.cpp
 * @brief AIMLAB VR Data Streamer - Unified Control Interface
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 04 November 2025
 * @version v6.2
 * 
 * Changelog:
 * - v6.2 (04 November 2025): Added progress indicators, enhanced debugging, and fixed duplicate status displays
 * - v6.1 (04 November 2025): Fixed Arduino status display, timer countdown, and VR discovery with debugging console
 * - v6.0 (04 November 2025): Complete VR data streaming integration with Unity TCP communication
 * - v5.2 (04 November 2025): Added custom OpenSans font support for professional appearance
 * - v5.1 (04 November 2025): CRITICAL FIX - Proper serial port timeouts and DTR reset for Arduino
 * - v5.0 (04 November 2025): Enhanced SDL2 implementation with improved styling and error handling
 * - v4.0 (04 November 2025): Migrated to Dear ImGui + SDL2 software renderer (no OpenGL)
 * - v3.0 (02 November 2025): Full serial communication with Arduino motor controller
 * - v2.0 (02 November 2025): Switched to Windows Native Controls (Win32 API)
 * - v1.0 (02 November 2025): Initial ImGui + GLFW + OpenGL3 implementation
 * 
 * Description:
 *   Unified GUI application for controlling VR experiments with:
 *   - Arduino vibration motor control
 *   - Unity VR headset data streaming
 *   - CSV data logging with auto-numbering
 *   - Real-time connection status monitoring
 *   - Non-blocking automatic discovery
 * 
 * Features:
 *   - Auto-discovery on startup (20-second timeout)
 *   - Manual refresh for both Arduino and Unity connections
 *   - Custom CSV filename with automatic conflict resolution
 *   - NYU purple and white color scheme
 *   - Professional OpenSans font
 *   - Real-time status indicators
 *   - Non-blocking threaded communication
 * 
 * Requirements:
 *   - MinGW-w64 (g++)
 *   - SDL2 development libraries
 *   - Dear ImGui (included in project)
 *   - Winsock2 for TCP communication
 *   - Arduino with vibration motor controller firmware
 *   - Unity VR application with data streaming
 * 
 * Build: 
 *   Run build.bat or:
 *   g++ main.cpp imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp ^
 *       backends/imgui_impl_sdl2.cpp backends/imgui_impl_sdlrenderer2.cpp ^
 *       -I. -Ibackends -ISDL2/x86_64-w64-mingw32/include/SDL2 ^
 *       -LSDL2/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lgdi32 -lws2_32 ^
 *       -o AIMLAB_VR_DataStreamer.exe -std=c++17 -mwindows
 * 
 * Usage: 
 *   ./AIMLAB_VR_DataStreamer.exe
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
const ImVec4 BACKGROUND_COLOR = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);  // #EEE
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
 * @brief Thread-safe debug logging
 */
void debugLog(const std::string& message) {
    static std::mutex logMutex;
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << "[" << std::chrono::system_clock::now().time_since_epoch().count() << "] " 
              << message << std::endl;
    std::cout.flush();  // Force flush to ensure output appears
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
    while (counter < 1000) {  // Reasonable limit
        filename = baseName + "_" + std::to_string(counter) + ".csv";
        if (!fs::exists(filename)) {
            return filename;
        }
        counter++;
    }
    
    // Fallback with timestamp
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
        debugLog("WSAStartup failed with error: " + std::to_string(result));
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
 * @brief Connect to Arduino
 */
void connectArduino() {
    std::lock_guard<std::mutex> lock(arduinoMutex);
    
    if (arduinoConnected) {
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
        }
        arduinoConnected = false;
        arduinoPort = "N/A";
        return;
    }
    
    std::vector<std::string> ports = listCOMPorts();
    
    for (const auto& port : ports) {
        HANDLE h = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_EXISTING, 0, NULL);
        if (h == INVALID_HANDLE_VALUE) continue;
        
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
            CloseHandle(h);
            continue;
        }
        
        PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
        
        if (handshakeArduino(h)) {
            hSerial = h;
            arduinoConnected = true;
            size_t pos = port.find("COM");
            arduinoPort = (pos != std::string::npos) ? port.substr(pos) : port;
            return;
        }
        CloseHandle(h);
    }
    
    arduinoConnected = false;
    arduinoPort = "N/A";
}

/**
 * @brief Send test vibration command
 */
void testVibration() {
    if (!arduinoConnected || hSerial == INVALID_HANDLE_VALUE) return;
    
    const char cmd = '1';
    DWORD written;
    WriteFile(hSerial, &cmd, 1, &written, NULL);
    WriteFile(hSerial, "\n", 1, &written, NULL);
}

// ============================================================================
// Unity VR Communication Functions
// ============================================================================

/**
 * @brief Receive data from Unity
 */
void receiveDataThread() {
    char buffer[1024];
    
    while (unityConnected && vrSocket != INVALID_SOCKET) {
        int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0) {
            buffer[result] = '\0';
            packetsReceived++;
            
            // Remove newlines from the data
            std::string data(buffer);
            data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
            data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
            
            std::lock_guard<std::mutex> lock(dataQueueMutex);
            dataQueue.push(data);
        } else if (result == 0) {
            // Connection closed
            break;
        } else {
            // Check for real error (not just would block)
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                break;
            }
        }
        
        Sleep(10);  // Small delay
    }
    
    unityConnected = false;
}

/**
 * @brief Listen for Unity discovery broadcast
 */
bool discoverUnity(int timeoutSeconds) {
    SOCKET discoverySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (discoverySocket == INVALID_SOCKET) {
        std::cerr << "Failed to create discovery socket\n";
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
        std::cerr << "Failed to bind discovery socket: " << WSAGetLastError() << "\n";
        closesocket(discoverySocket);
        return false;
    }
    
    // Set non-blocking mode
    u_long mode = 1;
    ioctlsocket(discoverySocket, FIONBIO, &mode);
    
    char buffer[256];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    
    discoveryStartTime = std::chrono::steady_clock::now();
    
    while (true) {
        if (!autoDiscoveryRunning && !unityBusy) break;
        
        auto elapsed = std::chrono::steady_clock::now() - discoveryStartTime;
        int remaining = timeoutSeconds - std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        discoveryTimeRemaining = std::max(0, remaining);
        
        if (remaining <= 0) break;
        
        int result = recvfrom(discoverySocket, buffer, sizeof(buffer) - 1, 0,
                             (sockaddr*)&senderAddr, &senderAddrSize);
        
        if (result > 0) {
            buffer[result] = '\0';
            std::cout << "Received broadcast: " << buffer << std::endl;
            
            if (strstr(buffer, "VR_HEADSET_DISCOVERY") != nullptr) {
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(senderAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
                std::cout << "Found VR headset at: " << ipStr << std::endl;
                
                closesocket(discoverySocket);
                
                // Try to connect
                if (connectUnity(ipStr)) {
                    return true;
                }
            }
        }
        
        Sleep(100);
    }
    
    closesocket(discoverySocket);
    return false;
}

/**
 * @brief Connect to Unity VR application
 */
bool connectUnity(const std::string& ip) {
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
        std::cerr << "Failed to create socket\n";
        return false;
    }
    
    // Set timeout for connection attempt
    DWORD timeout = 5000; // 5 seconds
    setsockopt(vrSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(vrSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(VR_PORT);
    
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip << "\n";
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    std::cout << "Attempting to connect to " << ip << ":" << VR_PORT << std::endl;
    
    if (connect(vrSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect: " << WSAGetLastError() << "\n";
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    // Send handshake
    const char* handshake = "DATA_RECEIVER_READY\n";
    if (send(vrSocket, handshake, strlen(handshake), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send handshake\n";
        closesocket(vrSocket);
        vrSocket = INVALID_SOCKET;
        return false;
    }
    
    // Wait for response
    char buffer[256];
    int result = recv(vrSocket, buffer, sizeof(buffer) - 1, 0);
    if (result > 0) {
        buffer[result] = '\0';
        std::cout << "Received response: " << buffer << std::endl;
        
        if (strstr(buffer, "VR_HEADSET_READY")) {
            unityConnected = true;
            unityIP = ip;
            
            // Set socket back to non-blocking for receive thread
            u_long mode = 1;
            ioctlsocket(vrSocket, FIONBIO, &mode);
            
            // Start receive thread
            unityReceiveThread = std::thread(receiveDataThread);
            
            std::cout << "Successfully connected to Unity VR application\n";
            return true;
        }
    }
    
    std::cerr << "Handshake failed\n";
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
    
    return true;
}

/**
 * @brief Stop data recording
 */
void stopRecording() {
    if (recording && dataFile.is_open()) {
        dataFile.close();
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
        
        // Add timestamp
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
    autoDiscoveryRunning = true;
    discoveryTimeRemaining = DISCOVERY_TIMEOUT;
    discoveryStartTime = std::chrono::steady_clock::now();
    
    // Try Arduino connection
    if (!arduinoConnected) {
        arduinoBusy = true;
        connectArduino();
        arduinoBusy = false;
    }
    
    // Try Unity discovery
    if (!unityConnected) {
        unityBusy = true;
        discoverUnity(DISCOVERY_TIMEOUT);
        unityBusy = false;
    }
    
    autoDiscoveryRunning = false;
    discoveryTimeRemaining = 0;
}

// ============================================================================
// GUI Functions
// ============================================================================

/**
 * @brief Apply NYU themed style
 */
void applyNYUStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;
    
    // Spacing
    style.WindowPadding = ImVec2(20, 20);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 10);
    style.ItemInnerSpacing = ImVec2(10, 8);
    
    // Sizes
    style.ScrollbarSize = 15.0f;
    style.GrabMinSize = 12.0f;
    
    // Colors
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
    
    std::cout << "AIMLAB VR Data Streamer Starting...\n";
    
    // Initialize Winsock
    if (!initializeWinsock()) {
        MessageBoxA(NULL, "Failed to initialize Winsock", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        MessageBoxA(NULL, SDL_GetError(), "SDL2 Initialization Failed", MB_OK | MB_ICONERROR);
        WSACleanup();
        return -1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "AIMLAB VR Data Streamer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        MessageBoxA(NULL, SDL_GetError(), "Window Creation Failed", MB_OK | MB_ICONERROR);
        SDL_Quit();
        WSACleanup();
        return -1;
    }
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer) {
        MessageBoxA(NULL, SDL_GetError(), "Renderer Creation Failed", MB_OK | MB_ICONERROR);
        SDL_DestroyWindow(window);
        SDL_Quit();
        WSACleanup();
        return -1;
    }
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Apply NYU style
    applyNYUStyle();
    
    // Load fonts before initializing backends
    loadFonts(io);
    
    // Initialize backends AFTER font loading
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    // Start auto-discovery
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
            ImGui::TextColored(NYU_PURPLE, "AIMLAB VR Data Streamer");
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
            
            // Arduino status - FIXED: Only show one status
            ImGui::Text("Vibration Motor:");
            ImGui::SameLine(150);
            
            if (arduinoConnected) {
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
            
            // Unity status
            ImGui::Text("VR Headset:");
            ImGui::SameLine(150);
            
            if (unityConnected) {
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
            
            ImGui::Spacing();
            
            // Control Buttons
            ImGui::TextColored(NYU_PURPLE, "Controls");
            ImGui::Separator();
            
            // Refresh buttons
            if (ImGui::Button("Refresh Vibration Motor", ImVec2(180, 35)) && 
                !arduinoBusy && !autoDiscoveryRunning) {
                arduinoBusy = true;
                std::thread([]() {
                    connectArduino();
                    arduinoBusy = false;
                }).detach();
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Refresh VR Headset", ImVec2(180, 35)) && 
                !unityBusy && !autoDiscoveryRunning) {
                unityBusy = true;
                std::thread([]() {
                    discoverUnity(10);
                    unityBusy = false;
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
                bool canStart = unityConnected && strlen(filenameBuffer) > 0;
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
                
                // Data queue status
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
            ImGui::TextColored(GRAY, "Version 6.1 - November 2025");
            if (fontSmall) ImGui::PopFont();
            
            ImGui::End();
        }
        
        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 238, 238, 238, 255);  // #EEE background
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
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
    }
    
    if (unityConnected) {
        unityConnected = false;
        if (vrSocket != INVALID_SOCKET) {
            closesocket(vrSocket);
        }
        if (unityReceiveThread.joinable()) {
            unityReceiveThread.join();
        }
    }
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    WSACleanup();
    
    FreeConsole();
    
    return 0;
}

