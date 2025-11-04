# AIMLAB VR Data Streamer - Quick Start Guide

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025

## Quick Setup (5 Minutes)

### Step 1: Build the Application
```batch
build.bat
```

### Step 2: Connect Hardware
1. Plug Arduino into USB port
2. Ensure Unity VR application is running on the same network

### Step 3: Run Application
```batch
AIMLAB_Console.exe
```

The application will automatically try to connect to both devices on startup!

## Common Tasks

### Record a VR Session
1. Wait for auto-connect to complete
2. Press **5** to start recording
3. Enter a filename (e.g., "experiment_01")
4. Perform your VR experiment
5. Press **6** to stop recording
6. Find your data in `experiment_01.csv`

### Test Vibration Motor
1. Ensure Arduino is connected (check status display)
2. Press **3** to send vibration command
3. Motor should vibrate briefly

### Manual Unity Connection
If auto-discovery fails:
1. Press **2** for Unity connection
2. Wait for timeout or press any key to cancel
3. Enter **y** for manual IP entry
4. Enter Unity device IP address (e.g., "192.168.1.100")

### Reconnect Devices
- Press **7** to re-run auto-connect for both devices
- Press **1** to reconnect Arduino only
- Press **2** to reconnect Unity only

## Status Display

The top of the screen shows:
- **Arduino Motor**: Connection status and COM port
- **Unity VR**: Connection status and IP address
- **Recording**: Status, filename, duration, and packet count

## Tips

- **Green indicators** mean connected (in status messages)
- **Press any key** to return to menu after each operation
- **Press 0** to exit cleanly
- Check the **README.md** for detailed troubleshooting

## Example Workflow

```
1. Start application → Auto-connect runs
2. Check status → Both devices connected
3. Press 3 → Test vibration (verify Arduino works)
4. Press 5 → Start recording "session_01"
5. [Perform VR experiment]
6. Press 6 → Stop recording
7. Press 0 → Exit
8. Open session_01.csv in Excel/Python
```

## File Outputs

All CSV files are saved in the same directory as the executable:
- `experiment_01.csv`
- `session_01.csv`
- etc.

## Need Help?

- Read **README.md** for detailed information
- Check firewall settings if Unity won't connect
- Replug Arduino if COM port issues occur
- Contact: pi.ko@nyu.edu

