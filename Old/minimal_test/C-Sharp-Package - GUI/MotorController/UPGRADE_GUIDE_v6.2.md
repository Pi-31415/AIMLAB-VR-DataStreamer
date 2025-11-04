# Upgrade Guide: v6.1 to v6.2

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025

## Overview

Version 6.2 adds progress indicators, enhanced debugging, and fixes remaining status display issues. Due to the extensive nature of the changes (adding progress tracking throughout), a complete file replacement is recommended.

## Critical Note

The v6.2 implementation requires extensive modifications throughout the codebase:
- Progress tracking for Arduino connection (40+ COM ports)
- Progress tracking for Unity discovery (30-second timeout)
- Thread-safe debug logging
- Status message handling
- Progress bar UI elements
- Auto-reset timers for completed operations

## Key Changes Summary

### 1. **New Global Variables**
```cpp
// Arduino progress tracking
std::atomic<float> arduinoProgress(0.0f);
std::string arduinoStatusMessage = "";
std::chrono::steady_clock::time_point arduinoSearchStart;

// Unity progress tracking
std::atomic<float> unityProgress(0.0f);
std::string unityStatusMessage = "";
std::chrono::steady_clock::time_point unitySearchStart;
```

### 2. **New Configuration Constants**
```cpp
const int MANUAL_DISCOVERY_TIMEOUT = 30;  // seconds for manual refresh
const int WINDOW_WIDTH = 650;  // increased for progress bars
const int WINDOW_HEIGHT = 550;  // increased for status messages
```

### 3. **Enhanced Debug Logging**
```cpp
void debugLog(const std::string& message) {
    static std::mutex logMutex;
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << "[timestamp] " << message << std::endl;
    std::cout.flush();  // Force immediate output
}
```

### 4. **Progress-Aware Connection Functions**
```cpp
void connectArduino(bool isManual = false);
bool discoverUnity(int timeoutSeconds, bool isManual = false);
```

### 5. **Progress Bar UI**
```cpp
if (arduinoBusy && arduinoProgress < 1.0f) {
    ImGui::ProgressBar(arduinoProgress.load(), ImVec2(-1, 0), 
                       arduinoStatusMessage.c_str());
}
```

### 6. **Auto-Reset After Completion**
```cpp
if (arduinoBusy && arduinoProgress >= 1.0f) {
    ImGui::TextColored(GRAY, arduinoStatusMessage.c_str());
    std::thread([&]() {
        Sleep(2000);
        arduinoBusy = false;
        arduinoProgress = 0.0f;
        arduinoStatusMessage = "";
    }).detach();
}
```

##  Status Display Fix

The duplicate status issue has been fixed by properly loading atomic values:

```cpp
// BEFORE (incorrect - direct atomic use in conditional)
if (arduinoConnected) { ... }

// AFTER (correct - load atomic value first)
bool isArduinoConnected = arduinoConnected.load();
if (isArduinoConnected) { ... }
```

## Implementation Approach

Due to the extensive changes required throughout the entire file (200+ lines modified/added), there are two approaches:

### Option A: Complete File Replacement (Recommended)
1. Backup current `main.cpp` to `main_backup_v6.1.cpp` ✅ (Already done)
2. Replace entire `main.cpp` with v6.2 implementation
3. Rebuild application
4. Test all features

### Option B: Manual Integration (Not Recommended)
1. Add all new global variables
2. Update all function signatures
3. Add progress tracking to connect functions
4. Update GUI rendering code
5. Add timeout handling in main loop
6. Update console initialization
7. Risk of missing changes or introducing bugs

## Files That Need Updates

1. **main.cpp** - Complete rewrite with progress tracking
2. **build.bat** - Update version number and features list

## Current Implementation Status

✅ Global variables updated (partial)  
✅ Debug logging function added  
✅ Forward declarations updated  
✅ Window size constants updated  
✅ Winsock initialization with logging  
⏳ Arduino connection function (needs progress tracking)  
⏳ Unity discovery function (needs progress tracking)  
⏳ GUI rendering (needs progress bars)  
⏳ Main loop (needs timeout handling)  

## Build Information

**Version:** 6.2  
**Executable:** AIMLAB_VR_DataStreamer.exe  
**Size:** ~1.9 MB (estimated)  
**New Dependencies:** None (uses existing libraries)

## Testing Checklist

After upgrading to v6.2, verify:

### Arduino Connection
- [ ] Progress bar shows during COM port scanning
- [ ] Status message updates for each port checked
- [ ] Progress completes at 100%
- [ ] Success message shows connected port
- [ ] Failure message shows after timeout
- [ ] Progress resets after 2 seconds

### Unity Discovery
- [ ] Progress bar shows during discovery
- [ ] Countdown timer updates
- [ ] IP address shown when found
- [ ] Connection attempt logged
- [ ] Success/failure message appears
- [ ] Progress resets after 2 seconds

### Console Output
- [ ] Console window opens on startup
- [ ] Debug messages appear immediately
- [ ] Thread-safe logging (no garbled text)
- [ ] Winsock initialization logged
- [ ] COM port scan logged
- [ ] UDP broadcast reception logged
- [ ] TCP connection attempts logged
- [ ] Handshake messages logged

### UI Improvements
- [ ] No duplicate status text
- [ ] Single status per device
- [ ] Atomic values loaded correctly
- [ ] Progress bars render correctly
- [ ] Window size accommodates new elements
- [ ] Auto-reset works properly

## Known Limitations

1. **Console Performance**: Heavy logging may impact performance
2. **Progress Accuracy**: COM port progress is based on port count, not time
3. **Status Messages**: Shared strings are not thread-safe (visual only)
4. **Auto-Reset**: Uses detached thread (cannot be canceled)

## Troubleshooting

### Console Not Showing Output
```cpp
// Ensure console is allocated
AllocConsole();
FILE* pCout;
freopen_s(&pCout, "CONOUT$", "w", stdout);
freopen_s(&pCout, "CONOUT$", "w", stderr);
std::ios_base::sync_with_stdio(false);  // Add this line
```

### Progress Bar Not Updating
- Ensure `arduinoBusy` and `unityBusy` are set before starting threads
- Check that progress is updated in the connection functions
- Verify atomic operations: `arduinoProgress.store(value)`

### Status Messages Not Showing
- Messages are stored in regular strings (not thread-safe)
- Update messages in the same thread that sets progress
- Use mutexes if messages are updated from multiple threads

## Rollback Instructions

If issues occur after upgrading:

```batch
copy main_backup_v6.1.cpp main.cpp
cmd /c build.bat
```

This restores the v6.1 implementation.

## Future Enhancements

Potential improvements for v6.3:

1. **Thread-Safe Status Messages**: Use mutex-protected strings
2. **Cancellable Operations**: Allow user to stop discovery
3. **Progress Time Estimates**: Show estimated time remaining
4. **Connection History**: Log all connection attempts
5. **Configuration File**: Save/load discovery timeout settings

## Support

For issues with v6.2 upgrade:

**Contact:** Pi Ko (pi.ko@nyu.edu)

**Provide:**
- Console output (copy/paste from console window)
- Screenshot of UI with issue
- Steps to reproduce
- v6.1 vs v6.2 behavior comparison

---

## Summary

Version 6.2 represents a significant enhancement with progress tracking throughout the application. While the changes are extensive, the improvements in user experience and debugging capabilities justify the upgrade effort.

**Recommendation:** Proceed with Option A (complete file replacement) for best results.

