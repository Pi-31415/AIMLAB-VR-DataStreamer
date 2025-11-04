/**
 * AIMLAB VR Streamer - Main Process
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 November 2025
 * Version: v2.1
 * 
 * Description:
 * Main Electron process for AIMLAB VR Data Collector.
 * Handles Unity UDP data reception, Arduino serial communication,
 * CSV file recording, and application lifecycle management.
 * 
 * Changelog:
 * v2.1 - 04 November 2025 - Connection fixes
 *        - Fixed Unity connection (no handshake required, data-based detection)
 *        - Fixed Arduino handshake with retry mechanism
 *        - Better serial port detection (CH340, FTDI support)
 *        - Improved error handling and cleanup
 *        - Added connection timeout based on data flow
 * v2.0 - 04 November 2025 - Major update for VR data collection
 *        - Added UDP server for Unity VR data reception
 *        - Added Serial communication for Arduino vibration motor
 *        - Implemented CSV recording with batching
 *        - Added connection handshake protocols
 * v1.0 - 04 November 2025 - Initial implementation
 */

const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const dgram = require('dgram');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const fs = require('fs');
const createCsvWriter = require('csv-writer').createObjectCsvWriter;

// Global Variables
let mainWindow;
let udpServer;
let serialPort;
let csvWriter;
let isRecording = false;
let currentFilename;
let dataBuffer = [];
let unityConnected = false;
let arduinoConnected = false;
let handshakeInterval = null;

// Unity Connection Management (without requiring handshake from Unity)
let lastDataReceived = null;
const DATA_TIMEOUT = 5000; // Consider disconnected after 5 seconds of no data

/**
 * Creates the main application window
 * @returns {BrowserWindow} The created window instance
 */
function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1000,
    height: 700,
    minWidth: 800,
    minHeight: 600,
    backgroundColor: '#FFFFFF',
    icon: path.join(__dirname, 'build', 'icon.png'),
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
    },
    title: 'AIMLAB VR Data Collector',
    autoHideMenuBar: true
  });

  mainWindow.loadFile('index.html');

  // Uncomment for debugging
  // mainWindow.webContents.openDevTools();

  mainWindow.on('closed', function () {
    // Cleanup on close
    if (udpServer) udpServer.close();
    if (serialPort && serialPort.isOpen) serialPort.close();
    if (unityCheckInterval) clearInterval(unityCheckInterval);
    if (handshakeInterval) clearInterval(handshakeInterval);
    mainWindow = null;
  });
}

/**
 * Logging Helper - Sends log messages to renderer
 * @param {string} message - Log message
 * @param {string} type - Message type (info, success, error, warning, arduino)
 */
function sendLog(message, type = 'info') {
  if (mainWindow && !mainWindow.isDestroyed()) {
    mainWindow.webContents.send('log-message', { 
      message, 
      type, 
      timestamp: new Date().toISOString() 
    });
  }
  console.log(`[${type.toUpperCase()}] ${message}`);
}

// ==================== Unity UDP Connection Handler ====================

let unityCheckInterval = null;

/**
 * Connects to Unity VR application via UDP
 */
ipcMain.handle('connect-unity', async (event, port = 8888) => {
  try {
    // Close existing server if any
    if (udpServer) {
      udpServer.close();
      udpServer = null;
    }
    
    udpServer = dgram.createSocket('udp4');
    
    udpServer.on('error', (err) => {
      sendLog(`Unity connection error: ${err.message}`, 'error');
      unityConnected = false;
      updateStatus();
    });
    
    udpServer.on('message', (msg, rinfo) => {
      handleUnityData(msg.toString(), rinfo);
    });
    
    udpServer.on('listening', () => {
      const address = udpServer.address();
      sendLog(`Unity UDP server listening on ${address.address}:${address.port}`, 'success');
      sendLog('Waiting for Unity data stream...', 'info');
      
      // Check for Unity data periodically
      if (unityCheckInterval) clearInterval(unityCheckInterval);
      unityCheckInterval = setInterval(checkUnityConnection, 1000);
    });
    
    udpServer.bind(port);
    return { success: true };
    
  } catch (error) {
    sendLog(`Failed to start UDP server: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Handles incoming Unity data (works with actual Unity data format)
 * @param {string} data - Raw data string from Unity
 * @param {object} rinfo - Remote info object
 */
function handleUnityData(data, rinfo) {
  try {
    lastDataReceived = Date.now();
    
    // Mark Unity as connected when we receive any data
    if (!unityConnected) {
      unityConnected = true;
      sendLog(`Unity connected from ${rinfo.address}:${rinfo.port}`, 'success');
      updateStatus();
    }
    
    // Parse the actual Unity data format
    const parsedData = parseUnityData(data);
    if (parsedData) {
      // Send to UI for display
      if (mainWindow && !mainWindow.isDestroyed()) {
        mainWindow.webContents.send('data-received', parsedData);
      }
      
      // Save to CSV if recording
      if (isRecording && csvWriter) {
        dataBuffer.push(parsedData);
        
        // Write in batches for performance
        if (dataBuffer.length >= 10) {
          csvWriter.writeRecords(dataBuffer)
            .then(() => {
              dataBuffer = [];
            })
            .catch(err => {
              sendLog(`Error writing to CSV: ${err.message}`, 'error');
            });
        }
      }
    }
  } catch (error) {
    sendLog(`Error processing Unity data: ${error.message}`, 'error');
  }
}

/**
 * Parse Unity Data Format (based on C# streamer)
 * Expected format: "timestamp,posX,posY,posZ,rotX,rotY,rotZ,rotW,trigger,grip,button1,button2"
 * @param {string} dataString - Raw data string
 * @returns {object|null} Parsed data object or null
 */
function parseUnityData(dataString) {
  try {
    const parts = dataString.trim().split(',');
    
    if (parts.length >= 12) {
      return {
        timestamp: parseFloat(parts[0]),
        positionX: parseFloat(parts[1]),
        positionY: parseFloat(parts[2]),
        positionZ: parseFloat(parts[3]),
        rotationX: parseFloat(parts[4]),
        rotationY: parseFloat(parts[5]),
        rotationZ: parseFloat(parts[6]),
        rotationW: parseFloat(parts[7]),
        triggerValue: parseFloat(parts[8]),
        gripValue: parseFloat(parts[9]),
        primaryButton: parts[10] === 'True' ? 1 : 0,
        secondaryButton: parts[11] === 'True' ? 1 : 0,
        receivedAt: new Date().toISOString()
      };
    } else {
      console.log('Incomplete data packet:', dataString);
    }
  } catch (error) {
    console.error('Parse error:', error);
  }
  return null;
}

/**
 * Check Unity Connection Status (based on data flow)
 */
function checkUnityConnection() {
  if (lastDataReceived && Date.now() - lastDataReceived > DATA_TIMEOUT) {
    if (unityConnected) {
      unityConnected = false;
      sendLog('Unity connection lost (no data received)', 'warning');
      updateStatus();
    }
  }
}

/**
 * Update connection status in UI
 */
function updateStatus() {
  if (mainWindow && !mainWindow.isDestroyed()) {
    mainWindow.webContents.send('status-update', { 
      unity: unityConnected, 
      arduino: arduinoConnected 
    });
  }
}

/**
 * Disconnect from Unity
 */
ipcMain.handle('disconnect-unity', async () => {
  if (unityCheckInterval) {
    clearInterval(unityCheckInterval);
    unityCheckInterval = null;
  }
  
  if (udpServer) {
    udpServer.close();
    udpServer = null;
    unityConnected = false;
    lastDataReceived = null;
    sendLog('Unity disconnected', 'info');
    updateStatus();
  }
  return { success: true };
});

// ==================== File Recording Handler ====================

/**
 * Start CSV recording
 * @param {string} filename - Desired filename without extension
 */
ipcMain.handle('start-recording', async (event, filename) => {
  try {
    // Create data directory if it doesn't exist
    const dataDir = path.join(__dirname, 'data');
    if (!fs.existsSync(dataDir)) {
      fs.mkdirSync(dataDir);
    }
    
    // Generate unique filename if exists
    let finalFilename = filename.replace(/\.csv$/i, ''); // Remove .csv if provided
    let counter = 1;
    let filepath = path.join(dataDir, `${finalFilename}.csv`);
    
    while (fs.existsSync(filepath)) {
      filepath = path.join(dataDir, `${finalFilename}_${counter}.csv`);
      counter++;
    }
    
    // Extract just the filename for display
    const displayName = path.basename(filepath, '.csv');
    
    // Create CSV writer
    csvWriter = createCsvWriter({
      path: filepath,
      header: [
        { id: 'timestamp', title: 'Timestamp' },
        { id: 'positionX', title: 'Position_X' },
        { id: 'positionY', title: 'Position_Y' },
        { id: 'positionZ', title: 'Position_Z' },
        { id: 'rotationX', title: 'Rotation_X' },
        { id: 'rotationY', title: 'Rotation_Y' },
        { id: 'rotationZ', title: 'Rotation_Z' },
        { id: 'rotationW', title: 'Rotation_W' },
        { id: 'triggerValue', title: 'Trigger_Value' },
        { id: 'gripValue', title: 'Grip_Value' },
        { id: 'primaryButton', title: 'Primary_Button' },
        { id: 'secondaryButton', title: 'Secondary_Button' },
        { id: 'receivedAt', title: 'Received_At' }
      ]
    });
    
    isRecording = true;
    currentFilename = displayName;
    dataBuffer = [];
    
    sendLog(`Recording started: ${displayName}.csv`, 'success');
    return { success: true, filename: displayName };
    
  } catch (error) {
    sendLog(`Failed to start recording: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Stop CSV recording
 */
ipcMain.handle('stop-recording', async () => {
  try {
    // Write remaining buffer
    if (isRecording && dataBuffer.length > 0 && csvWriter) {
      await csvWriter.writeRecords(dataBuffer);
    }
    
    isRecording = false;
    const recordedFile = currentFilename;
    currentFilename = null;
    csvWriter = null;
    dataBuffer = [];
    
    sendLog(`Recording stopped: ${recordedFile}.csv`, 'success');
    sendLog(`File saved in data folder`, 'info');
    return { success: true, filename: recordedFile };
    
  } catch (error) {
    sendLog(`Failed to stop recording: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

// ==================== Arduino Serial Connection Handler ====================

/**
 * Connect to Arduino vibration motor controller
 */
ipcMain.handle('connect-arduino', async () => {
  try {
    // List all available ports
    const ports = await SerialPort.list();
    sendLog(`Found ${ports.length} serial ports`, 'info');
    
    // Debug: List all ports
    ports.forEach(port => {
      sendLog(`Port: ${port.path}, Manufacturer: ${port.manufacturer || 'Unknown'}`, 'info');
    });
    
    // Find Arduino port
    let arduinoPort = ports.find(port => {
      return port.manufacturer && (
        port.manufacturer.includes('Arduino') ||
        port.manufacturer.includes('arduino') ||
        port.manufacturer.includes('CH340') || // Common Arduino clone chip
        port.manufacturer.includes('FTDI')     // Another common chip
      );
    });
    
    // If no Arduino found, try first available port
    if (!arduinoPort && ports.length > 0) {
      arduinoPort = ports[0];
      sendLog(`No Arduino detected, trying ${arduinoPort.path}`, 'warning');
    }
    
    if (!arduinoPort) {
      throw new Error('No serial ports found. Is Arduino plugged in?');
    }
    
    // Close existing connection if any
    if (serialPort && serialPort.isOpen) {
      serialPort.close();
    }
    
    sendLog(`Attempting to connect to ${arduinoPort.path}`, 'info');
    
    serialPort = new SerialPort({
      path: arduinoPort.path,
      baudRate: 9600,
      autoOpen: false // We'll open it manually
    });
    
    // Set up parser for line-by-line reading
    const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\r\n' }));
    
    // Handle incoming data from Arduino
    parser.on('data', (data) => {
      sendLog(`Arduino: ${data}`, 'arduino');
      
      // Check for handshake response
      if (data.includes('Vibration Motor Controller Ready')) {
        arduinoConnected = true;
        sendLog('Arduino handshake confirmed!', 'success');
        updateStatus();
        
        // Clear handshake interval
        if (handshakeInterval) {
          clearInterval(handshakeInterval);
          handshakeInterval = null;
        }
      }
      
      // Handle motor status messages
      if (data.includes('Motor ON') || data.includes('Motor activated')) {
        sendLog('Motor vibrating', 'success');
      }
      if (data.includes('Motor OFF')) {
        sendLog('Motor stopped', 'info');
      }
    });
    
    serialPort.on('error', (err) => {
      sendLog(`Arduino error: ${err.message}`, 'error');
      arduinoConnected = false;
      updateStatus();
    });
    
    serialPort.on('close', () => {
      sendLog('Arduino disconnected', 'warning');
      arduinoConnected = false;
      updateStatus();
      if (handshakeInterval) {
        clearInterval(handshakeInterval);
        handshakeInterval = null;
      }
    });
    
    // Open the port
    await new Promise((resolve, reject) => {
      serialPort.open((err) => {
        if (err) reject(err);
        else resolve();
      });
    });
    
    sendLog(`Serial port ${arduinoPort.path} opened successfully`, 'success');
    sendLog('Sending handshake to Arduino...', 'info');
    
    // Send handshake repeatedly until we get response
    let handshakeAttempts = 0;
    const maxAttempts = 10;
    
    const sendHandshake = () => {
      if (!serialPort || !serialPort.isOpen) {
        if (handshakeInterval) clearInterval(handshakeInterval);
        return;
      }
      
      if (arduinoConnected) {
        if (handshakeInterval) clearInterval(handshakeInterval);
        return;
      }
      
      handshakeAttempts++;
      if (handshakeAttempts > maxAttempts) {
        sendLog('Arduino handshake timeout - device may not be running correct sketch', 'error');
        if (handshakeInterval) clearInterval(handshakeInterval);
        return;
      }
      
      serialPort.write('HELLO\n', (err) => {
        if (err) {
          sendLog(`Error sending handshake: ${err.message}`, 'error');
        } else {
          sendLog(`Handshake attempt ${handshakeAttempts}/${maxAttempts}`, 'info');
        }
      });
    };
    
    // Send first handshake immediately
    setTimeout(sendHandshake, 500); // Small delay for Arduino to initialize
    
    // Then repeat every second until connected
    handshakeInterval = setInterval(sendHandshake, 1000);
    
    return { success: true, port: arduinoPort.path };
    
  } catch (error) {
    sendLog(`Failed to connect to Arduino: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Send test command to motor
 */
ipcMain.handle('test-motor', async () => {
  try {
    if (!serialPort || !serialPort.isOpen) {
      throw new Error('Serial port not open');
    }
    
    if (!arduinoConnected) {
      throw new Error('Arduino handshake not confirmed');
    }
    
    return new Promise((resolve) => {
      serialPort.write('1\n', (err) => {
        if (err) {
          sendLog(`Failed to send motor command: ${err.message}`, 'error');
          resolve({ success: false, error: err.message });
        } else {
          sendLog('Motor test command sent', 'success');
          resolve({ success: true });
        }
      });
    });
    
  } catch (error) {
    sendLog(`Motor test error: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Disconnect from Arduino
 */
ipcMain.handle('disconnect-arduino', async () => {
  if (handshakeInterval) {
    clearInterval(handshakeInterval);
    handshakeInterval = null;
  }
  
  if (serialPort && serialPort.isOpen) {
    serialPort.close();
    serialPort = null;
    arduinoConnected = false;
    sendLog('Arduino disconnected', 'info');
    updateStatus();
  }
  return { success: true };
});

// ==================== Application Lifecycle ====================

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  // Clean up connections
  if (udpServer) udpServer.close();
  if (serialPort && serialPort.isOpen) serialPort.close();
  if (unityCheckInterval) clearInterval(unityCheckInterval);
  if (handshakeInterval) clearInterval(handshakeInterval);
  
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

// Handle app quit
app.on('before-quit', () => {
  if (udpServer) udpServer.close();
  if (serialPort && serialPort.isOpen) serialPort.close();
  if (unityCheckInterval) clearInterval(unityCheckInterval);
  if (handshakeInterval) clearInterval(handshakeInterval);
});
