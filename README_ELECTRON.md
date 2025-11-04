# AIMLAB VR Streamer - Electron Application

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 November 2025  
**Version:** v1.0

## Overview

AIMLAB VR Streamer is a desktop application built with Electron for streaming VR data from Unity applications. The application features a modern UI with NYU purple accents on a clean white background.

## Features

- **Connection Management**: Connect to VR systems via IP address and port
- **Streaming Controls**: Start and stop data streaming with real-time feedback
- **Progress Monitoring**: Visual progress bars for data transmission and buffer status
- **Statistics Dashboard**: Real-time statistics including data points sent, packets per second, and uptime
- **Activity Log**: Comprehensive logging of all application events
- **NYU Themed UI**: Professional interface with NYU purple (#57068c) accents

## Installation

1. Ensure Node.js is installed on your system
2. Navigate to the project directory
3. Dependencies are already installed (Electron v27.0.0)

## Running the Application

### Development Mode
```powershell
npm start
```

### With Logging Enabled
```powershell
npm run dev
```

## Project Structure

```
AIMLAB-VR-DataStreamer/
├── main.js              # Main Electron process
├── preload.js           # Preload script for IPC bridge
├── index.html           # Application UI structure
├── styles.css           # NYU-themed stylesheet
├── renderer.js          # Renderer process (UI logic)
├── package.json         # Node.js dependencies
└── README_ELECTRON.md   # This file
```

## UI Components

### Header
- Application title with NYU branding
- Real-time connection status indicator

### Connection Panel
- IP address input
- Port number input
- Connect/Disconnect buttons

### Streaming Controls Panel
- Start/Stop streaming buttons
- Data transmission progress bar
- Buffer status progress bar

### Statistics Panel
- Data Points Sent counter
- Packets per Second meter
- Total Data Sent display
- Session uptime timer

### Activity Log Panel
- Real-time event logging
- Color-coded messages (info, success, warning, error)
- Auto-scrolling log container

## Color Scheme

- **Primary**: NYU Purple (#57068c)
- **Background**: Solid White (#FFFFFF)
- **Accents**: Purple gradients and highlights
- **Success**: Green (#4CAF50)
- **Danger**: Red (#F44336)
- **Info**: Blue (#2196F3)
- **Warning**: Orange (#FF9800)

## Development Notes

### Security
- Context isolation enabled
- Node integration disabled
- Secure IPC communication via contextBridge

### IPC Handlers
The application includes placeholder IPC handlers in `main.js`:
- `start-streaming`: Initiates data streaming
- `stop-streaming`: Stops data streaming
- `get-status`: Retrieves current application status

### Customization
To integrate with actual VR streaming backend:
1. Update IPC handlers in `main.js`
2. Implement actual connection logic
3. Replace simulated statistics with real data
4. Add error handling for network operations

## Future Enhancements

- [ ] Integration with C++ backend
- [ ] Serial port communication
- [ ] File saving and data export
- [ ] Configuration profiles
- [ ] Multi-device support
- [ ] Real-time data visualization

## License

MIT License - NYU Research Project

## Contact

For questions or issues, contact Pi Ko at pi.ko@nyu.edu

---

**Note**: This is a demonstration application. The streaming functionality is currently simulated for UI testing purposes. Integration with actual VR data streaming backend is required for production use.

