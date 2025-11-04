# Unity Connection Protocol Fix - v1.1

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.1

---

## Problem Identified

The original `main.cpp` had a **protocol mismatch** with the Unity side:

1. **Discovery Protocol Backwards**: The console app was listening for broadcasts, but Unity was also broadcasting. They never found each other because both were broadcasting/listening incorrectly.

2. **Connection Direction**: Unity acts as the TCP server (accepts connections), not the client.

3. **No Direct Connection Method**: Required discovery to work, making testing difficult.

---

## Changes Made

### 1. Fixed Discovery Protocol

**Before:** Console app listened for Unity broadcasts on UDP port 55001

**After:** Console app now broadcasts `"DATA_RECEIVER_DISCOVERY"` and listens for `"VR_HEADSET_HERE"` responses

```cpp
// New behavior in discoverUnity()
- Broadcasts discovery message every second
- Listens for "VR_HEADSET_HERE" response
- Extracts sender IP from response
- Connects to that IP
```

### 2. Added Direct Connection Method

New function: `connectUnityDirect(ip)`

**Features:**
- Skips discovery entirely
- No handshake requirement (optional)
- Immediate TCP connection
- Perfect for localhost testing
- Simpler error handling

**Use cases:**
- Testing on same machine (localhost)
- Known IP addresses
- When discovery fails

### 3. Enhanced Connection Menu

**New Option 2 Menu:**
```
[Unity Connection Options]
1. Try automatic discovery (broadcast)
2. Connect to localhost (127.0.0.1)
3. Enter IP manually
```

**Benefits:**
- Quick localhost connection for testing
- Still supports discovery
- Manual IP entry as fallback

### 4. Improved Auto-Connect

**New behavior:**
1. First tries localhost (common for testing)
2. Then tries discovery broadcast
3. Finally offers manual IP entry

**Advantage:** Faster connection when testing locally

### 5. Better Receive Thread

**Improvements:**
- Console feedback when thread starts/stops
- Better error reporting
- Connection close detection
- Simplified data storage (no unnecessary cleaning)
- Optional debug output (commented out)

---

## Technical Details

### Discovery Protocol Flow

**Old (Broken):**
```
Unity: Broadcasts "VR_HEADSET_DISCOVERY"
Console: Listens on port 55001
Result: Neither finds the other
```

**New (Fixed):**
```
Console: Broadcasts "DATA_RECEIVER_DISCOVERY" on port 55001
Unity: Listens, responds with "VR_HEADSET_HERE"
Console: Receives response, extracts IP, connects
Result: Connection established
```

### Direct Connection Flow

```
1. User selects direct connection
2. Console creates TCP socket
3. Console connects to Unity at specified IP:55000
4. Socket set to non-blocking mode
5. Receive thread starts immediately
6. Data begins flowing
```

### Data Reception

**Simplified approach:**
- Raw data stored directly (no cleaning unless recording)
- Packet counter increments
- Error messages on failure
- Thread-safe queue for recording

---

## Usage Examples

### Example 1: Testing Locally
```
1. Start Unity app on same PC
2. Run AIMLAB_Console.exe
3. Press 2 (Connect Unity)
4. Press 2 (Connect to localhost)
5. ✓ Connected instantly!
```

### Example 2: Network Connection
```
1. Start Unity app on VR headset (e.g., Quest)
2. Note the IP address (e.g., 192.168.1.100)
3. Run AIMLAB_Console.exe
4. Press 2 (Connect Unity)
5. Press 3 (Enter IP manually)
6. Type: 192.168.1.100
7. ✓ Connected!
```

### Example 3: Discovery Still Works
```
1. Start Unity app on network
2. Run AIMLAB_Console.exe
3. Press 2 (Connect Unity)
4. Press 1 (Automatic discovery)
5. Wait for broadcast responses
6. ✓ Finds Unity and connects!
```

---

## Backward Compatibility

### What Still Works
- ✅ Original `connectUnity()` function unchanged
- ✅ Arduino connection unchanged
- ✅ Recording functionality unchanged
- ✅ Data format unchanged
- ✅ Menu structure mostly same (option 2 enhanced)

### What's New
- ✅ `connectUnityDirect()` function (new)
- ✅ Enhanced menu for Unity connection
- ✅ Localhost auto-try in autoConnect
- ✅ Better error messages

---

## Testing Recommendations

### Test 1: Localhost Connection
```
1. Build and run Unity on same PC
2. Run console app
3. Should connect to localhost automatically
4. Verify data reception
```

### Test 2: Network Connection
```
1. Deploy Unity to VR headset
2. Note headset IP
3. Run console app
4. Use manual IP entry
5. Verify connection and data flow
```

### Test 3: Discovery Protocol
```
1. Ensure Unity broadcasts enabled
2. Run console app
3. Try discovery option
4. Check for "VR_HEADSET_HERE" response
5. Verify connection established
```

---

## Troubleshooting

### Issue: Localhost won't connect
**Cause:** Unity not running on same PC  
**Solution:** Use option 3 (manual IP) or start Unity locally

### Issue: Discovery times out
**Cause:** Network/firewall blocking UDP broadcasts  
**Solution:** Use option 2 (localhost) or option 3 (manual IP)

### Issue: "Connection closed by Unity"
**Cause:** Unity disconnected or crashed  
**Solution:** Restart Unity app and reconnect

### Issue: No data received
**Cause:** Unity not streaming or wrong protocol  
**Solution:** 
1. Check Unity script is attached
2. Verify transforms assigned
3. Check Unity console for errors
4. Enable debug output in receive thread

---

## Configuration

### Network Ports (unchanged)
- **TCP Data Port:** 55000
- **UDP Discovery Port:** 55001

### Timeouts
- **Discovery timeout:** 10 seconds (configurable)
- **TCP receive timeout:** 1 second per attempt
- **Socket timeout:** 5 seconds for initial connect

---

## Code Changes Summary

### Files Modified
- `main.cpp` - Updated to v1.1

### Lines Changed
- Added: ~140 lines (new function + menu enhancements)
- Modified: ~80 lines (discovery, autoConnect, receive thread)
- Total: ~220 lines changed/added

### Functions Added
1. `connectUnityDirect(ip)` - Direct TCP connection

### Functions Modified
1. `discoverUnity()` - Now broadcasts instead of listens
2. `receiveDataThread()` - Better error reporting
3. `autoConnect()` - Tries localhost first
4. Main menu case '2' - Enhanced with options

---

## Performance Impact

### Improvements
- ✅ Faster localhost connection (immediate vs 10s timeout)
- ✅ Better error feedback (immediate vs timeout)
- ✅ More reliable connection (multiple methods)

### No Impact
- Network bandwidth: Same
- CPU usage: Same
- Memory usage: Same
- Data recording: Same

---

## Future Enhancements

### Possible Improvements
- [ ] Auto-detect Unity on local network (mDNS)
- [ ] Connection retry logic
- [ ] Multiple Unity instances support
- [ ] Connection state persistence
- [ ] Network quality monitoring

---

## Build Instructions

### Rebuild Required
```batch
build.bat
```

### Verification
```batch
verify_setup.bat
```

### Test Run
```batch
AIMLAB_Console.exe
```

---

## Version History

### v1.1 - 04 November 2025
- Fixed discovery protocol (broadcasts instead of listens)
- Added direct connection method
- Enhanced Unity connection menu
- Improved auto-connect (tries localhost first)
- Better error reporting in receive thread

### v1.0 - 04 November 2025
- Initial implementation
- Auto-connect functionality
- Arduino serial communication
- Unity TCP streaming
- CSV recording

---

## Contact

For questions about these changes:

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Project:** AIMLAB VR Data Streamer  
**Institution:** NYU Abu Dhabi

---

**Document Version:** 1.0  
**Last Updated:** 04 November 2025

