/**
 * @file main.cpp
 * @brief AIMLAB-VR Vibration Motor Controller UI (Win32 API)
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 02 November 2025
 * @version v3.0
 * 
 * Changelog:
 * - v3.0 (02 November 2025): Full serial communication with Arduino motor controller
 * - v2.0 (02 November 2025): Switched to Windows Native Controls (Win32 API)
 * - v1.0 (02 November 2025): Initial ImGui implementation
 * 
 * Simple Win32 GUI for connecting to an Arduino-based vibration motor controller.
 * Scans all COM ports, performs serial handshake, and triggers vibration via button.
 * 
 * Features:
 * - Automatic COM port scanning (COM1-COM40)
 * - Serial handshake with Arduino
 * - Connection status with visual indicator
 * - Test vibration button
 * - Non-blocking threads for serial I/O
 * 
 * Requirements:
 * - MinGW-w64 (g++)
 * - Windows (Win32 API)
 * - Arduino with vibration motor controller firmware
 * 
 * Build: Run build.bat or: g++ main.cpp -lgdi32 -luser32 -o main.exe -std=c++17
 * Usage: ./main.exe
 */

#include <windows.h>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include <sstream>

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool connectToArduino(HWND hwnd);
void testVibration();
std::vector<std::string> listCOMPorts();
bool handshakeArduino(HANDLE hSerial);
void setStatus(HWND hwnd, bool connected);

// Control IDs
#define ID_BTN_CONNECT  1001
#define ID_BTN_TEST     1002

// Global state
std::atomic<bool> connected(false);
HANDLE hSerial = INVALID_HANDLE_VALUE;
HWND statusLabel, connectBtn, testBtn, statusChip;

/**
 * @brief Enumerate available COM ports
 * @return Vector of available COM port names
 */
std::vector<std::string> listCOMPorts() {
    std::vector<std::string> ports;
    char portName[16];
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
 * @brief Read line from serial port (blocking up to timeout)
 * @param serial Serial port handle
 * @param out Output string
 * @param timeout Timeout in milliseconds
 * @return True if line was read successfully
 */
bool readLine(HANDLE serial, std::string &out, DWORD timeout = 1500) {
    char c;
    DWORD bytesRead;
    out.clear();
    DWORD start = GetTickCount();
    while (GetTickCount() - start < timeout) {
        if (ReadFile(serial, &c, 1, &bytesRead, NULL) && bytesRead == 1) {
            if (c == '\n') break;
            out.push_back(c);
        }
    }
    return !out.empty();
}

/**
 * @brief Perform handshake with Arduino
 * @param serial Serial port handle
 * @return True if handshake successful
 */
bool handshakeArduino(HANDLE serial) {
    // Give Arduino time to reset after opening the port
    // Most Arduinos reset when the serial port is opened
    Sleep(2000);

    DWORD bytesWritten;
    const char* msg = "HELLO\r\n";
    WriteFile(serial, msg, (DWORD)strlen(msg), &bytesWritten, NULL);

    std::string line;
    // Increased timeout to 4000 ms for more reliable handshake
    if (readLine(serial, line, 4000)) {
        if (line.find("Vibration Motor Controller Ready") != std::string::npos)
            return true;
    }
    return false;
}

/**
 * @brief Attempt to connect to Arduino on available COM ports
 * @param hwnd Window handle for UI updates
 * @return True if connection successful
 */
bool connectToArduino(HWND hwnd) {
    std::vector<std::string> ports = listCOMPorts();
    for (const auto& port : ports) {
        HANDLE serial = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                    OPEN_EXISTING, 0, NULL);
        if (serial == INVALID_HANDLE_VALUE) continue;

        // Configure serial port
        DCB dcb = { 0 };
        dcb.DCBlength = sizeof(dcb);
        GetCommState(serial, &dcb);
        dcb.BaudRate = CBR_9600;
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity   = NOPARITY;
        SetCommState(serial, &dcb);
        
        // Set timeouts to 0 for non-blocking behavior in readLine
        COMMTIMEOUTS timeouts = { 0, 0, 0, 0, 0 };
        SetCommTimeouts(serial, &timeouts);

        // Try handshake (includes 2-second delay for Arduino reset)
        if (handshakeArduino(serial)) {
            hSerial = serial;
            connected = true;
            setStatus(hwnd, true);
            return true;
        }
        CloseHandle(serial);
    }

    // No Arduino found on any port
    connected = false;
    setStatus(hwnd, false);
    return false;
}

/**
 * @brief Send vibration test command to Arduino
 */
void testVibration() {
    if (!connected || hSerial == INVALID_HANDLE_VALUE) return;
    const char cmd = '1';
    DWORD written;
    WriteFile(hSerial, &cmd, 1, &written, NULL);
    WriteFile(hSerial, "\n", 1, &written, NULL);
}

/**
 * @brief Update status display with connection state
 * @param hwnd Window handle
 * @param isConnected Connection status
 */
void setStatus(HWND hwnd, bool isConnected) {
    if (isConnected) {
        SetWindowText(statusLabel, "Vibration Motor Connected");
        SendMessage(statusChip, STM_SETICON,
                    (WPARAM)LoadIcon(NULL, IDI_INFORMATION), 0);
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(200,255,200)));
    } else {
        SetWindowText(statusLabel, "Vibration Motor Not Connected");
        SendMessage(statusChip, STM_SETICON,
                    (WPARAM)LoadIcon(NULL, IDI_ERROR), 0);
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(255,200,200)));
    }
    InvalidateRect(hwnd, NULL, TRUE);
}

/**
 * @brief Window procedure callback for handling messages
 * @param hwnd Window handle
 * @param msg Message identifier
 * @param wParam Additional message parameter
 * @param lParam Additional message parameter
 * @return Result of message processing
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            // Create title
            CreateWindow("STATIC", "AIMLAB-VR Motor Controller", 
                         WS_VISIBLE | WS_CHILD,
                         20, 10, 300, 20, hwnd, NULL, NULL, NULL);

            // Create connect button
            connectBtn = CreateWindow("BUTTON", "Connect to / Refresh Vibration Motor",
                         WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         20, 50, 280, 30, hwnd, (HMENU)ID_BTN_CONNECT, NULL, NULL);

            // Create test button
            testBtn = CreateWindow("BUTTON", "Test Vibration", 
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         20, 100, 150, 30, hwnd, (HMENU)ID_BTN_TEST, NULL, NULL);

            // Create status label
            statusLabel = CreateWindow("STATIC", "Vibration Motor Not Connected",
                         WS_VISIBLE | WS_CHILD, 20, 150, 300, 20, hwnd, NULL, NULL, NULL);

            // Create status icon
            statusChip = CreateWindow("STATIC", "", 
                         WS_VISIBLE | WS_CHILD | SS_ICON,
                         340, 145, 32, 32, hwnd, NULL, NULL, NULL);

            setStatus(hwnd, false);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_CONNECT) {
                // Connect in background thread to avoid blocking UI
                std::thread(connectToArduino, hwnd).detach();
            } else if (LOWORD(wParam) == ID_BTN_TEST) {
                // Test vibration in background thread
                std::thread(testVibration).detach();
            }
            break;

        case WM_DESTROY:
            // Clean up serial connection
            if (connected && hSerial != INVALID_HANDLE_VALUE)
                CloseHandle(hSerial);
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/**
 * @brief Main entry point for Windows application
 * @param hInstance Current instance handle
 * @param hPrevInstance Previous instance handle (always NULL)
 * @param lpCmdLine Command line parameters
 * @param nCmdShow Window show state
 * @return Exit code
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    const char CLASS_NAME[] = "MotorControllerUI";
    
    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create main window (non-resizable)
    HWND hwnd = CreateWindow(
        CLASS_NAME,
        "AIMLAB-VR Motor Controller - Pi Ko (pi.ko@nyu.edu)",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        420, 250,
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

