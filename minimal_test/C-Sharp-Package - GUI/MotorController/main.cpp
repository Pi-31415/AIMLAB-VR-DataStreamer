/**
 * @file main.cpp
 * @brief AIMLAB-VR Vibration Motor Controller UI (Dear ImGui + SDL2 Software Renderer)
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 04 November 2025
 * @version v5.2
 * 
 * Changelog:
 * - v5.2 (04 November 2025): Added custom OpenSans font support for professional appearance
 * - v5.1 (04 November 2025): CRITICAL FIX - Proper serial port timeouts and DTR reset for Arduino
 * - v5.0 (04 November 2025): Enhanced SDL2 implementation with improved styling and error handling
 * - v4.0 (04 November 2025): Migrated to Dear ImGui + SDL2 software renderer (no OpenGL)
 * - v3.0 (02 November 2025): Full serial communication with Arduino motor controller
 * - v2.0 (02 November 2025): Switched to Windows Native Controls (Win32 API)
 * - v1.0 (02 November 2025): Initial ImGui + GLFW + OpenGL3 implementation
 * 
 * Description:
 *   Modern cross-platform GUI using Dear ImGui with SDL2 software renderer.
 *   Completely self-contained with NO OpenGL dependency - uses SDL2's built-in
 *   2D accelerated renderer which falls back to software rendering if needed.
 *   
 *   This implementation eliminates the Win32 API dependency and provides a fully
 *   portable, modern UI solution using only SDL2 and ImGui. The software renderer
 *   ensures compatibility across all systems without requiring OpenGL support.
 * 
 * Features:
 *   - Automatic COM port scanning (COM1-COM40)
 *   - Serial handshake with Arduino
 *   - Connection status with visual color indicator
 *   - Test vibration button
 *   - Non-blocking threads for serial I/O
 *   - Beautiful ImGui interface with modern styling
 *   - SDL2 software renderer (no OpenGL required)
 * 
 * Requirements:
 *   - MinGW-w64 (g++)
 *   - SDL2 development libraries
 *   - Dear ImGui (included in project)
 *   - Arduino with vibration motor controller firmware
 * 
 * Build: 
 *   Run build.bat or:
 *   g++ main.cpp imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_tables.cpp ^
 *       backends/imgui_impl_sdl2.cpp backends/imgui_impl_sdlrenderer2.cpp ^
 *       -I. -Ibackends -ISDL2/x86_64-w64-mingw32/include/SDL2 ^
 *       -LSDL2/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lgdi32 ^
 *       -o main.exe -std=c++17 -mwindows
 * 
 * Usage: 
 *   ./main.exe
 */

#include <windows.h>
#include <SDL.h>
#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

// ============================================================================
// Global Font Pointers
// ============================================================================

ImFont* fontDefault = nullptr;
ImFont* fontLarge = nullptr;
ImFont* fontSmall = nullptr;

// ============================================================================
// Serial Communication Functions
// ============================================================================

std::atomic<bool> connected(false);
std::atomic<bool> busy(false);
HANDLE hSerial = INVALID_HANDLE_VALUE;
std::string currentPort = "N/A";

/**
 * @brief Enumerate all available COM ports on the system
 * @return Vector of COM port device names (e.g., "\\\\.\\COM3")
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
 * @brief Read a line from serial port with timeout
 * @param serial Handle to serial port
 * @param out Output string to store the read line
 * @param timeout Timeout in milliseconds
 * @return True if a line was successfully read
 */
bool readLine(HANDLE serial, std::string &out, DWORD timeout = 3000) {
    char buffer[256];
    DWORD bytesRead;
    out.clear();
    DWORD start = GetTickCount();
    
    while (GetTickCount() - start < timeout) {
        if (ReadFile(serial, buffer, 1, &bytesRead, NULL)) {
            if (bytesRead == 1) {
                if (buffer[0] == '\n') {
                    // Line complete
                    return !out.empty();
                }
                if (buffer[0] != '\r') {
                    out.push_back(buffer[0]);
                }
            }
        } else {
            // Read error
            DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING) {
                return false;
            }
        }
        
        // Small delay to prevent CPU spinning
        Sleep(1);
    }
    
    return !out.empty();
}

/**
 * @brief Perform handshake protocol with Arduino
 * @param serial Handle to serial port
 * @return True if handshake was successful
 * 
 * Protocol:
 *   1. Toggle DTR to reset Arduino (for boards like Uno, Mega, etc.)
 *   2. Wait 2 seconds for Arduino to boot after reset
 *   3. Clear any boot messages from the buffer
 *   4. Send "HELLO\r\n" command
 *   5. Wait for "Vibration Motor Controller Ready" response
 */
bool handshakeArduino(HANDLE serial) {
    // Toggle DTR to reset Arduino (for boards like Uno, Mega, etc.)
    EscapeCommFunction(serial, CLRDTR);
    Sleep(250);
    EscapeCommFunction(serial, SETDTR);
    
    // Give Arduino time to boot after reset
    Sleep(2000);
    
    // Clear any boot messages
    PurgeComm(serial, PURGE_RXCLEAR);
    
    DWORD bytesWritten;
    const char* msg = "HELLO\r\n";
    if (!WriteFile(serial, msg, (DWORD)strlen(msg), &bytesWritten, NULL)) {
        return false;
    }
    
    // Flush write buffer to ensure data is sent
    FlushFileBuffers(serial);
    
    std::string line;
    if (readLine(serial, line, 4000)) {
        return (line.find("Vibration Motor Controller Ready") != std::string::npos);
    }
    return false;
}

/**
 * @brief Attempt to connect to Arduino on any available COM port
 * @return True if connection established successfully
 * 
 * This function scans all available COM ports and attempts a handshake
 * with each one until a successful connection is established.
 */
bool connectArduino() {
    // If already connected, disconnect
    if (connected) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        connected = false;
        currentPort = "N/A";
        return false;
    }
    
    std::vector<std::string> ports = listCOMPorts();
    
    for (const auto& port : ports) {
        HANDLE h = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                               OPEN_EXISTING, 0, NULL);
        if (h == INVALID_HANDLE_VALUE) continue;
        
        // Configure serial port parameters
        DCB dcb = {0};
        dcb.DCBlength = sizeof(dcb);
        GetCommState(h, &dcb);
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity   = NOPARITY;
        
        // IMPORTANT: Set proper flow control
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
        
        // CRITICAL FIX: Set proper timeouts for reliable communication
        COMMTIMEOUTS timeouts;
        timeouts.ReadIntervalTimeout = 50;          // Max time between bytes
        timeouts.ReadTotalTimeoutMultiplier = 10;   // Multiplier per byte
        timeouts.ReadTotalTimeoutConstant = 100;    // Base timeout
        timeouts.WriteTotalTimeoutMultiplier = 10;  
        timeouts.WriteTotalTimeoutConstant = 100;
        
        if (!SetCommTimeouts(h, &timeouts)) {
            CloseHandle(h);
            continue;
        }
        
        // Clear any pending data
        PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
        
        // Attempt handshake
        if (handshakeArduino(h)) {
            hSerial = h;
            connected = true;
            
            // Extract COM port number for display
            size_t pos = port.find("COM");
            if (pos != std::string::npos) {
                currentPort = port.substr(pos);
            } else {
                currentPort = port;
            }
            
            return true;
        }
        CloseHandle(h);
    }
    
    // No Arduino found
    connected = false;
    currentPort = "N/A";
    return false;
}

/**
 * @brief Send vibration test command to Arduino
 * 
 * Sends a '1\n' command to trigger vibration for a short duration.
 * The Arduino firmware interprets '1' as a vibration test command.
 */
void testVibration() {
    if (!connected || hSerial == INVALID_HANDLE_VALUE) return;
    
    const char cmd = '1';
    DWORD written;
    WriteFile(hSerial, &cmd, 1, &written, NULL);
    WriteFile(hSerial, "\n", 1, &written, NULL);
}

// ============================================================================
// GUI Application
// ============================================================================

/**
 * @brief Load custom fonts from TTF files
 * @param io ImGui IO context
 * @return True if fonts loaded successfully
 * 
 * This function loads OpenSans-Regular.ttf in multiple sizes:
 * - fontDefault: 16px for normal UI elements
 * - fontLarge: 20px for titles and headers
 * - fontSmall: 14px for status text and small labels
 * 
 * IMPORTANT: Don't build the font atlas here - let the SDL2 backend handle it
 * 
 * If font loading fails, falls back to ImGui's built-in font.
 */
bool loadCustomFonts(ImGuiIO& io) {
    // Clear any existing fonts
    io.Fonts->Clear();
    
    // Configure font settings for better quality
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 3;  // Horizontal oversampling for better quality
    fontConfig.OversampleV = 3;  // Vertical oversampling for better quality
    fontConfig.PixelSnapH = true;  // Align to pixel boundaries
    
    // Try to load OpenSans as default font (16px)
    fontDefault = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 16.0f, &fontConfig);
    if (!fontDefault) {
        // Fallback to default font if OpenSans not found
        fontDefault = io.Fonts->AddFontDefault();
        std::cerr << "Warning: Could not load OpenSans-Regular.ttf, using default font\n";
        fontLarge = fontDefault;
        fontSmall = fontDefault;
        return false;
    }
    
    // Load larger version for titles (20px)
    fontLarge = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 20.0f, &fontConfig);
    if (!fontLarge) {
        fontLarge = fontDefault; // Fallback to default size
    }
    
    // Load smaller version for status text (14px)
    fontSmall = io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 14.0f, &fontConfig);
    if (!fontSmall) {
        fontSmall = fontDefault; // Fallback to default size
    }
    
    // DON'T BUILD THE FONT ATLAS HERE!
    // The SDL2 renderer backend will do it automatically when initialized
    // Removed: io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    
    return true;
}

/**
 * @brief Apply custom ImGui styling for modern appearance
 */
void applyCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    
    // Spacing
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    
    // Font rendering settings for better quality
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    
    // Colors - Light theme with professional look
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.80f, 0.80f, 0.90f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.65f, 0.65f, 0.90f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.55f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.75f, 1.00f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.65f, 0.90f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
}

/**
 * @brief Main application entry point
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 = success)
 */
int main(int argc, char** argv) {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        MessageBoxA(NULL, SDL_GetError(), "SDL2 Initialization Failed", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "AIMLAB-VR Motor Controller - SDL2 + ImGui",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        500, 320,
        SDL_WINDOW_SHOWN
    );
    
    if (window == NULL) {
        MessageBoxA(NULL, SDL_GetError(), "Window Creation Failed", MB_OK | MB_ICONERROR);
        SDL_Quit();
        return -1;
    }
    
    // Create SDL2 renderer (accelerated with VSync, falls back to software if needed)
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (renderer == NULL) {
        MessageBoxA(NULL, SDL_GetError(), "Renderer Creation Failed", MB_OK | MB_ICONERROR);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    
    // Load custom fonts BEFORE initializing backends
    if (!loadCustomFonts(io)) {
        std::cerr << "Warning: Using fallback font\n";
    }
    
    // Apply custom styling
    applyCustomStyle();
    
    // Initialize ImGui backends AFTER font loading
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
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
            
            if (event.type == SDL_WINDOWEVENT && 
                event.window.event == SDL_WINDOWEVENT_CLOSE && 
                event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }
        }
        
        // Start new ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // ====================================================================
        // Main GUI Window
        // ====================================================================
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float)500, (float)320));
            
            ImGui::Begin("AIMLAB-VR Motor Controller", nullptr,
                        ImGuiWindowFlags_NoResize | 
                        ImGuiWindowFlags_NoMove | 
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoTitleBar);
            
            // Title with large font
            if (fontLarge) ImGui::PushFont(fontLarge);
            ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.5f, 1.0f), "AIMLAB-VR Motor Controller");
            if (fontLarge) ImGui::PopFont();
            
            ImGui::Separator();
            ImGui::Spacing();
            
            // Author info with small font
            if (fontSmall) ImGui::PushFont(fontSmall);
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Author: Pi Ko (pi.ko@nyu.edu)");
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Version: v5.2 - SDL2 + ImGui (OpenSans Font)");
            if (fontSmall) ImGui::PopFont();
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // Connect button
            if (ImGui::Button("Connect to / Refresh Vibration Motor", ImVec2(400, 40)) && !busy) {
                busy = true;
                std::thread([]() {
                    connectArduino();
                    busy = false;
                }).detach();
            }
            
            if (busy) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.0f, 1.0f), "Scanning...");
            }
            
            ImGui::Spacing();
            
            // Test vibration button
            if (ImGui::Button("Test Vibration", ImVec2(200, 35))) {
                std::thread(testVibration).detach();
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // Status display with colored indicator
            ImGui::Text("Status: ");
            ImGui::SameLine();
            
            if (connected) {
                ImGui::TextColored(ImVec4(0.0f, 0.7f, 0.0f, 1.0f), "CONNECTED");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.0f, 1.0f), "[%s]", currentPort.c_str());
                
                // Draw green indicator circle
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                draw_list->AddCircleFilled(
                    ImVec2(p.x + 440, p.y - 10), 
                    8.0f, 
                    IM_COL32(0, 200, 0, 255)
                );
            } else {
                ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "NOT CONNECTED");
                
                // Draw red indicator circle
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                draw_list->AddCircleFilled(
                    ImVec2(p.x + 440, p.y - 10), 
                    8.0f, 
                    IM_COL32(200, 0, 0, 255)
                );
            }
            
            ImGui::Spacing();
            ImGui::Spacing();
            
            // Instructions with small font
            if (fontSmall) ImGui::PushFont(fontSmall);
            ImGui::TextWrapped("Instructions: Click 'Connect' to scan for Arduino. "
                             "Once connected, use 'Test Vibration' to verify motor operation.");
            if (fontSmall) ImGui::PopFont();
            
            ImGui::End();
        }
        
        // ====================================================================
        // Rendering
        // ====================================================================
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
    if (connected && hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
    }
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

