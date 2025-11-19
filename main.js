 /**
  * AIMLAB VR Streamer - Main Process
  * 
  * Author: Pi Ko (pi.ko@nyu.edu)
  * Date: 19 November 2025
  * Version: v3.9
  * 
  * Description:
  * Main Electron process for AIMLAB VR Data Collector.
  * Handles Unity UDP data reception with full bidirectional protocol,
  * Arduino serial communication, TSV file reception, and application lifecycle.
  * 
 * Changelog:
 * v3.9 - 19 November 2025 - Added mid-experiment save with auto-sync
 *        - Added save-mid-experiment IPC handler
 *        - Sends SAVE_MID_EXPERIMENT command to Unity
 *        - Handles MID_SAVE_COMPLETE confirmation from Unity
 *        - Auto-syncs experiment data after mid-save (4 second delay)
 *        - Auto-syncs experiment data after stop experiment (4 second delay)
 * v3.8 - 19 November 2025 - Added ADB sync functionality with path configuration
 *        - Added sync-experiment-data IPC handler
 *        - Pulls data from Android headset via ADB
 *        - Syncs /sdcard/Android/data/com.AimLab.NHPT/files/HandMovement/ to ExperimentalData
 *        - Uses execFile to run adb commands safely
 *        - Added ADB path configuration (stored in userData/adb-config.json)
 *        - Default ADB path: Unity SDK platform-tools/adb.exe
 *        - Added get-adb-path and set-adb-path IPC handlers
 *        - User can select custom ADB path via file dialog
 * v3.7 - 05 November 2025 - Chunked TSV file transfer implementation
 *        - Added chunked TSV transfer support (TSV_HEADER, TSV_CHUNK, TSV_COMPLETE)
 *        - Handles large files split into 8KB chunks with Base64 encoding
 *        - Progress tracking with updates every 10 chunks
 *        - Timeout cleanup (60s) for interrupted transfers
 *        - Chunk validation and reassembly with error checking
 *        - Files saved to experimentData/Left_Hand or Right_Hand folders
 * v3.6 - 05 November 2025 - Added TSV file handling from Unity
 *        - Added handleTSVFileReceived function to save TSV files
 *        - Files saved to experimentData/Left_Hand or Right_Hand folders
 *        - Added check-experiment-files IPC handler
 *        - Removed CSV creation on experiment start (Unity sends TSV instead)
  * v3.5 - 05 November 2025 - Added experiment ID passing to Unity
  *        - IPC handlers now accept experimentId parameter
  *        - Commands sent as CMD:START_LEFT_EXPERIMENT:experimentId
  *        - Unity receives the experiment ID for internal use
  * v3.4 - 05 November 2025 - Added left/right hand experiment support
  *        - Added start-left-experiment and start-right-experiment IPC handlers
  *        - Sends CMD:START_LEFT_EXPERIMENT or CMD:START_RIGHT_EXPERIMENT to Unity
  *        - Filenames automatically prefixed with LEFT_ or RIGHT_
  * v3.3 - 05 November 2025 - Integrated recording with experiments
  *        - Added check-file-exists handler for pre-validation
  *        - Start Experiment now automatically starts recording
  *        - Stop Experiment now automatically stops recording
  *        - File existence check prevents overwriting data
 * v3.2 - 05 November 2025 - Added folder opening functionality
 *        - Added open-data-folder IPC handler to open ExperimentalData folder in Explorer
 *        - Uses shell.openPath for cross-platform folder opening
 *        - Creates folder if it doesn't exist before opening
 * v3.1 - 04 November 2025 - Force file creation fix
 *        - CSV file now created immediately with headers (not delayed)
 *        - Fixes issue where file wasn't created if no data received
 *        - Added csvWriter.writeRecords([]) to force header write
 *        - Better logging with full file paths
 * v3.0 - 04 November 2025 - ExperimentalData folder implementation
 *        - Changed save location from 'data' to 'ExperimentalData'
 *        - More descriptive folder name for research data
 *        - Created ExperimentalData directory
 *        - Updated .gitignore to exclude ExperimentalData
 * v2.9 - 04 November 2025 - ENOTDIR fix and path resolution
 *        - Fixed path resolution for packaged vs development mode
 *        - Added extensive diagnostic logging
 *        - Removed .gitkeep file that was causing issues
 *        - Multiple fallback strategies for directory creation
 *        - Write permission verification
 * v2.8 - 04 November 2025 - Experiment control and recording fixes
 *        - Added Start/Stop Experiment commands to control Unity
 *        - Fixed ENOTDIR error with proper directory creation
 *        - Changed batch size to 5 for safer writes
 *        - Added file rename notification modal
 *        - Added dialog module for alerts
 * v2.7 - 04 November 2025 - Data port conflict resolution
 *        - Electron now uses port 45102 (not 45101 to avoid Unity conflict)
 *        - Unity sends to 45102, Electron sends to 45101
 * v2.6 - 04 November 2025 - setBroadcast timing fix
 *        - Fixed EBADF error by setting broadcast AFTER bind
 *        - Simplified broadcast to target Unity's port 45000 only
 *        - Better error handling for broadcast operations
 * v2.5 - 04 November 2025 - Discovery port conflict fix
 *        - Fixed port conflict (Unity on 45000, Electron on 45001-45009)
 *        - Added bidirectional discovery (both broadcast)
 *        - Electron sends discovery broadcasts to Unity
 *        - Active handshake initiation after ACK
 *        - Proper broadcast to all discovery ports
 * v2.4 - 04 November 2025 - Full Unity protocol implementation
 * v2.3 - 04 November 2025 - Port configuration fix
 * v2.2 - 04 November 2025 - Socket error fixes
 * v2.0 - 04 November 2025 - VR data collection
 * v1.0 - 04 November 2025 - Initial implementation
 */

const { app, BrowserWindow, ipcMain, dialog, shell } = require('electron');
const path = require('path');
const dgram = require('dgram');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const fs = require('fs');
const createCsvWriter = require('csv-writer').createObjectCsvWriter;
const { execFile } = require('child_process');   // For running adb

// ADB Configuration
const configPath = path.join(app.getPath("userData"), "adb-config.json");

/**
 * Load ADB path from configuration file
 * @returns {string} ADB executable path
 */
function loadAdbPath() {
  try {
    if (fs.existsSync(configPath)) {
      const data = JSON.parse(fs.readFileSync(configPath, "utf8"));
      return data.adbPath;
    }
  } catch (_) {}
  
  // Default fallback (Unity ADB path)
  return "C:\\Program Files\\Unity\\Hub\\Editor\\2021.3.22f1\\Editor\\Data\\PlaybackEngines\\AndroidPlayer\\SDK\\platform-tools\\adb.exe";
}

/**
 * Save ADB path to configuration file
 * @param {string} p - ADB executable path
 */
function saveAdbPath(p) {
  fs.writeFileSync(configPath, JSON.stringify({ adbPath: p }, null, 2));
}

// Experiment Data Path Constants - Define ONCE for consistency
const EXPERIMENT_DATA_PATH = path.join(app.getPath('desktop'), 'ExperimentData');
const LEFT_HAND_PATH = path.join(EXPERIMENT_DATA_PATH, 'Left_Hand');
const RIGHT_HAND_PATH = path.join(EXPERIMENT_DATA_PATH, 'Right_Hand');

// Global Variables
let mainWindow = null;
let discoveryServer = null;  // For discovery on ports 45001-45009 (NOT 45000!)
let dataServer = null;       // For data on port 45101
let serialPort = null;
let csvWriter = null;
let isRecording = false;
let currentFilename = null;
let dataBuffer = [];
let unityConnected = false;
let arduinoConnected = false;
let handshakeInterval = null;
let unityEndpoint = null;
let discoveryInterval = null;
let myDiscoveryPort = 0;

// TSV Transfer Tracking
const tsvTransfers = new Map(); // Store ongoing TSV transfers

// Protocol Constants (must match Unity)
const DISCOVERY_BASE_PORT = 45000;
const DATA_PORT = 45102;  // OUR data port (Electron listens here)
const UNITY_DATA_PORT = 45101;  // Unity's data port (Unity listens here)
const NODE_ID = "AIMLAB_CPP";  // Unity expects this
const PEER_ID = "AIMLAB_UNITY";  // Unity identifies as this

// Protocol Messages
const MSG_DISCOVER = "DISCOVER";
const MSG_ACKNOWLEDGE = "ACK";
const MSG_HANDSHAKE = "HANDSHAKE";
const MSG_READY = "READY";
const MSG_DATA = "DATA";
const MSG_KEEPALIVE = "KEEPALIVE";
const MSG_COMMAND = "CMD";

 // Command Constants
 const CMD_START_LEFT_EXPERIMENT = "START_LEFT_EXPERIMENT";
 const CMD_START_RIGHT_EXPERIMENT = "START_RIGHT_EXPERIMENT";
 const CMD_STOP_EXPERIMENT = "STOP_EXPERIMENT";
 const CMD_SAVE_MID_EXPERIMENT = "SAVE_MID_EXPERIMENT";

/**
 * Creates the main application window
 * @returns {BrowserWindow} The created window instance
 */
function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1000,
    height: 800,
    minWidth: 1000,
    maxWidth: 1000,
    minHeight: 800,
    maxHeight: 800,
    resizable: false,
    backgroundColor: '#FFFFFF',
    icon: path.join(__dirname, 'build', 'icon.ico'),
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

  // Handle window close
  mainWindow.on('closed', () => {
    cleanupConnections();
    mainWindow = null;
  });
}

/**
 * Cleanup function - Properly close all connections
 */
function cleanupConnections() {
  // Stop intervals
  if (discoveryInterval) {
    clearInterval(discoveryInterval);
    discoveryInterval = null;
  }
  
  if (handshakeInterval) {
    clearInterval(handshakeInterval);
    handshakeInterval = null;
  }
  
  // Close discovery server
  if (discoveryServer) {
    try {
      discoveryServer.close(() => {
        console.log('Discovery server closed');
      });
    } catch (err) {
      console.log('Discovery server cleanup:', err.message);
    }
    discoveryServer = null;
  }
  
  // Close data server
  if (dataServer) {
    try {
      dataServer.close(() => {
        console.log('Data server closed');
      });
    } catch (err) {
      console.log('Data server cleanup:', err.message);
    }
    dataServer = null;
  }
  
  // Close serial port
  if (serialPort && serialPort.isOpen) {
    try {
      serialPort.close();
      console.log('Serial port closed');
    } catch (err) {
      console.log('Serial port cleanup:', err.message);
    }
    serialPort = null;
  }
  
  // Reset connection states
  unityConnected = false;
  arduinoConnected = false;
  unityEndpoint = null;
}

/**
 * Logging Helper - Sends log messages to renderer
 * @param {string} message - Log message
 * @param {string} type - Message type (info, success, error, warning, arduino, debug)
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

// ==================== Unity Connection with Full Protocol ====================

/**
 * Connects to Unity VR application with full bidirectional protocol
 * Unity uses port 45000, Electron uses 45001-45009 to avoid conflict
 */
ipcMain.handle('connect-unity', async (event, port = DATA_PORT) => {
  try {
    // Clean up existing servers
    if (discoveryServer) {
      try {
        discoveryServer.close();
      } catch (err) {
        console.log('Discovery server cleanup:', err.message);
      }
      discoveryServer = null;
    }
    
    if (dataServer) {
      try {
        dataServer.close();
      } catch (err) {
        console.log('Data server cleanup:', err.message);
      }
      dataServer = null;
    }
    
    // Stop any discovery broadcasts
    if (discoveryInterval) {
      clearInterval(discoveryInterval);
      discoveryInterval = null;
    }
    
    // Reset connection state
    unityConnected = false;
    unityEndpoint = null;
    
    sendLog('Starting Unity connection...', 'info');
    
    // Step 1: Create discovery server - AVOID port 45000 (Unity uses it)
    discoveryServer = dgram.createSocket('udp4');
    
    discoveryServer.on('error', (err) => {
      sendLog(`Discovery server error: ${err.message}`, 'error');
    });
    
    discoveryServer.on('message', (msg, rinfo) => {
      const message = msg.toString();
      sendLog(`Discovery received: ${message}`, 'debug');
      
      // Parse discovery message: "DISCOVER:AIMLAB_UNITY:dataPort:discoveryPort"
      const parts = message.split(':');
      if (parts[0] === MSG_DISCOVER && parts[1] === PEER_ID) {
        // Unity found!
        const unityDataPort = parseInt(parts[2]) || UNITY_DATA_PORT;
        const unityDiscoveryPort = parts.length > 3 ? parseInt(parts[3]) : 45000;
        
        sendLog(`Unity discovered! Data port: ${unityDataPort}, Discovery port: ${unityDiscoveryPort}`, 'success');
        
        // Send ACK with OUR data port (45102) so Unity knows where to send data
        const ackMessage = `${MSG_ACKNOWLEDGE}:${NODE_ID}:${DATA_PORT}`;
        discoveryServer.send(
          Buffer.from(ackMessage), 
          unityDiscoveryPort,  // Send to Unity's discovery port (45000)
          rinfo.address,
          (err) => {
            if (!err) {
              sendLog(`Sent ACK to Unity (our data port: ${DATA_PORT})`, 'success');
              
              // Store Unity's endpoint for sending data TO Unity
              unityEndpoint = { address: rinfo.address, port: UNITY_DATA_PORT };
              
              // Initiate handshake on Unity's data port after receiving discovery
              setTimeout(() => {
                if (dataServer && unityEndpoint) {
                  const handshakeMsg = `${MSG_HANDSHAKE}:${NODE_ID}`;
                  dataServer.send(
                    Buffer.from(handshakeMsg),
                    UNITY_DATA_PORT,  // Send to Unity's port (45101)
                    unityEndpoint.address,
                    (err) => {
                      if (!err) {
                        sendLog(`Sent HANDSHAKE to Unity on port ${UNITY_DATA_PORT}`, 'info');
                      }
                    }
                  );
                }
              }, 500);
            } else {
              sendLog(`Failed to send ACK: ${err.message}`, 'error');
            }
          }
        );
      }
      // Also handle ACK messages from Unity
      else if (parts[0] === MSG_ACKNOWLEDGE && parts[1] === PEER_ID) {
        const unityDataPort = parseInt(parts[2]) || UNITY_DATA_PORT;
        sendLog(`Received ACK from Unity, data port: ${unityDataPort}`, 'success');
        unityEndpoint = { address: rinfo.address, port: UNITY_DATA_PORT };
      }
    });
    
    // Bind discovery server to ports 45001-45009 (NOT 45000 - Unity is using it!)
    let bound = false;
    
    // Start from 45001, NOT 45000
    for (let i = 1; i < 10; i++) {
      try {
        const tryPort = DISCOVERY_BASE_PORT + i;
        await new Promise((resolve, reject) => {
          discoveryServer.bind(tryPort, '0.0.0.0', (err) => {
            if (err) {
              reject(err);
            } else {
              // Set broadcast AFTER successful bind
              try {
                discoveryServer.setBroadcast(true);
                myDiscoveryPort = tryPort;
                bound = true;
                resolve();
              } catch (broadcastErr) {
                reject(broadcastErr);
              }
            }
          });
        });
        
        sendLog(`Discovery server on port ${myDiscoveryPort}`, 'success');
        sendLog(`Unity is on port 45000, we are on port ${myDiscoveryPort}`, 'info');
        break;
      } catch (err) {
        console.log(`Port ${DISCOVERY_BASE_PORT + i} failed:`, err.message);
        if (i === 9) {
          sendLog('Could not bind any discovery port (45001-45009)', 'error');
          throw new Error('Could not bind discovery port');
        }
      }
    }
    
    if (!bound) {
      throw new Error('Failed to bind discovery server');
    }
    
    // Step 2: Create data server for handshake and data
    dataServer = dgram.createSocket('udp4');
    
    dataServer.on('message', (msg, rinfo) => {
      const message = msg.toString();

      // Check for chunked TSV file transfer messages
      if (message.startsWith('TSV_HEADER|')) {
        handleTSVHeader(message);
        return; // Don't process as other message type
      } else if (message.startsWith('TSV_CHUNK|')) {
        handleTSVChunk(message);
        return; // Don't process as other message type
      } else if (message.startsWith('TSV_COMPLETE|')) {
        handleTSVComplete(message);
        return; // Don't process as other message type
      }
      
      // Legacy support: Check if this is a single TSV file transfer (non-chunked)
      if (message.startsWith('TSV_FILE|')) {
        handleTSVFileReceived(message);
        return; // Don't process as other message type
      }
      
      // Handle mid-save completion confirmation from Unity
      if (message.includes('MID_SAVE_COMPLETE')) {
        sendLog('Mid-experiment save completed in Unity', 'success');
        if (mainWindow && !mainWindow.isDestroyed()) {
          mainWindow.webContents.send('log', {
            message: 'Mid-experiment save completed',
            type: 'success'
          });
        }
        return; // Don't process as other message type
      }
      
      // Handle vibration commands from Unity
      if (message.startsWith('VIBRATE:')) {
        const motorId = message.split(':')[1] || '1';
        
        // Check if serialPort exists and is open
        if (serialPort && serialPort.isOpen) {
          serialPort.write('1\n', (err) => {
            if (err) {
              sendLog(`Error sending vibration to Arduino: ${err.message}`, 'error');
            } else {
              sendLog('Vibration command sent to Arduino', 'success');
            }
          });
        } else {
          sendLog('Motor not connected - cannot send vibration', 'warning');
        }
        return; // Don't process as other message type
      } 
      else if (message.startsWith('VIBRATE_CUSTOM:')) {
        const parts = message.split(':');
        const duration = parts[1];
        const intensity = parts[2];
        
        // Check if serialPort exists and is open
        if (serialPort && serialPort.isOpen) {
          serialPort.write(`C:${duration}:${intensity}\n`, (err) => {
            if (err) {
              sendLog(`Error sending custom vibration to Arduino: ${err.message}`, 'error');
            } else {
              sendLog(`Custom vibration sent: ${duration}ms at ${intensity}%`, 'success');
            }
          });
        } else {
          sendLog('Motor not connected - cannot send vibration', 'warning');
        }
        return; // Don't process as other message type
      }
      
      // Handle different message types
      if (message.startsWith(MSG_HANDSHAKE)) {
        sendLog(`Received HANDSHAKE from Unity`, 'success');
        
        // Send READY response immediately
        const readyMessage = `${MSG_READY}:${NODE_ID}`;
        dataServer.send(
          Buffer.from(readyMessage),
          rinfo.port,
          rinfo.address,
          (err) => {
            if (!err) {
              sendLog('Sent READY to Unity - connection established!', 'success');
              unityConnected = true;
              updateStatus();
              
              // Stop discovery broadcasts
              if (discoveryInterval) {
                clearInterval(discoveryInterval);
                discoveryInterval = null;
              }
            } else {
              sendLog(`Failed to send READY: ${err.message}`, 'error');
            }
          }
        );
      }
      else if (message.startsWith(MSG_READY)) {
        sendLog('Unity confirmed READY - fully connected!', 'success');
        unityConnected = true;
        updateStatus();
        
        // Stop discovery broadcasts
        if (discoveryInterval) {
          clearInterval(discoveryInterval);
          discoveryInterval = null;
        }
      }
      else if (message.startsWith(MSG_DATA)) {
        // Actual VR data
        if (!unityConnected) {
          unityConnected = true;
          sendLog('Unity connected and sending data!', 'success');
          updateStatus();
        }
        handleUnityData(message, rinfo);
      }
      else if (message.startsWith(MSG_KEEPALIVE)) {
        // Respond to keepalive
        const keepaliveResponse = `${MSG_KEEPALIVE}:${NODE_ID}`;
        dataServer.send(Buffer.from(keepaliveResponse), rinfo.port, rinfo.address);
      }
      else if (message.startsWith(MSG_COMMAND) || message.startsWith('CMD:')) {
        // Command response from Unity
        const cmdResponse = message.substring(4);
        sendLog(`Unity command response: ${cmdResponse}`, 'info');
      }
      else {
        // Try parsing as raw CSV data (backwards compatibility)
        handleUnityData(message, rinfo);
      }
    });
    
    dataServer.on('error', (err) => {
      sendLog(`Data server error: ${err.message}`, 'error');
    });
    
    // Bind data server to 45101
    await new Promise((resolve, reject) => {
      dataServer.bind(DATA_PORT, '0.0.0.0', (err) => {
        if (err) reject(err);
        else resolve();
      });
    });
    
    sendLog(`Data server listening on port ${DATA_PORT} (our port)`, 'success');
    sendLog(`Unity will send data to our port ${DATA_PORT}`, 'info');
    sendLog(`We will send to Unity's port ${UNITY_DATA_PORT}`, 'info');
    
    // Step 3: Broadcast our own discovery messages to find Unity
    const broadcastDiscovery = () => {
      if (!discoveryServer || unityConnected) return;
      
      try {
        const discoverMsg = `${MSG_DISCOVER}:${NODE_ID}:${DATA_PORT}:${myDiscoveryPort}`;
        const broadcastAddr = '255.255.255.255';
        
        // Send to Unity's discovery port (45000)
        discoveryServer.send(
          Buffer.from(discoverMsg),
          45000,  // Unity's discovery port
          broadcastAddr,
          (err) => {
            if (!err) {
              sendLog('Sent discovery broadcast', 'debug');
            } else {
              console.log('Broadcast error:', err.message);
            }
          }
        );
      } catch (err) {
        console.log('Broadcast error:', err.message);
      }
    };
    
    // Broadcast discovery every 2 seconds until connected
    discoveryInterval = setInterval(() => {
      if (unityConnected) {
        clearInterval(discoveryInterval);
        discoveryInterval = null;
      } else {
        broadcastDiscovery();
      }
    }, 2000);
    
    // Initial broadcast
    setTimeout(broadcastDiscovery, 500);
    
    sendLog('Waiting for Unity...', 'info');
    sendLog('Make sure Unity VR app is running!', 'info');
    sendLog('Unity should be on port 45000', 'info');
    
    return { success: true };
    
  } catch (error) {
    sendLog(`Failed to start Unity connection: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Handle Unity VR data with protocol support
 * @param {string} data - Raw data string from Unity
 * @param {object} rinfo - Remote info object
 */
function handleUnityData(data, rinfo) {
  try {
    // Mark as connected if not already
    if (!unityConnected) {
      unityConnected = true;
      sendLog(`Unity connected and sending data from ${rinfo.address}:${rinfo.port}`, 'success');
      updateStatus();
    }
    
    // Remove "DATA:" prefix if present
    let dataString = data;
    if (dataString.startsWith('DATA:')) {
      // Format: "DATA:type:values"
      const parts = dataString.split(':');
      if (parts.length >= 3) {
        dataString = parts.slice(2).join(':'); // Get everything after "DATA:type:"
      } else {
        dataString = dataString.substring(5); // Just remove "DATA:"
      }
    }

    // Check for vibration command BEFORE parsing as CSV
    if (data.startsWith('DATA:VIBRATION:')) {
      const vibrationValue = data.split(':')[2];
      if (vibrationValue === '1' && serialPort && serialPort.isOpen) {
        serialPort.write('1\n', (err) => {
          if (err) {
            sendLog(`Error sending vibration to Arduino: ${err.message}`, 'error');
          } else {
            sendLog('Vibration command sent to Arduino', 'success');
          }
        });
      }
      return; // Don't process as CSV data
    }
    
    // Parse VR data
    const parsedData = parseUnityData(dataString);
    
    if (parsedData) {
      // Send to UI for display
      if (mainWindow && !mainWindow.isDestroyed()) {
        mainWindow.webContents.send('data-received', parsedData);
      }
      
      // Save to CSV if recording
      if (isRecording && csvWriter) {
        dataBuffer.push(parsedData);
        
        // Write in smaller batches (every 5 lines for safety)
        if (dataBuffer.length >= 5) {
          csvWriter.writeRecords(dataBuffer)
            .then(() => {
              sendLog(`Saved ${dataBuffer.length} data points`, 'debug');
              dataBuffer = [];
            })
            .catch(err => {
              sendLog(`Error writing to CSV: ${err.message}`, 'error');
            });
        }
      }
    }
  } catch (error) {
    console.error('Error handling Unity data:', error);
  }
}

/**
 * Parse Unity Data Format
 * Expected format: "timestamp,posX,posY,posZ,rotX,rotY,rotZ,rotW,trigger,grip,button1,button2"
 * @param {string} dataString - Raw data string
 * @returns {object|null} Parsed data object or null
 */
function parseUnityData(dataString) {
  try {
    const parts = dataString.trim().split(',');
    
    if (parts.length >= 12) {
      const parsed = {
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
      
      // Verify we got valid numbers
      if (!isNaN(parsed.timestamp) && !isNaN(parsed.positionX)) {
        return parsed;
      }
    }
    
    // Log unexpected format occasionally to avoid spam
    if (Math.random() < 0.01) {
      console.log('Unexpected data format:', dataString.substring(0, 100));
    }
    
  } catch (error) {
    console.error('Parse error:', error);
  }
  return null;
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
  // Stop discovery broadcasts
  if (discoveryInterval) {
    clearInterval(discoveryInterval);
    discoveryInterval = null;
  }
  
  // Close discovery server
  if (discoveryServer) {
    try {
      discoveryServer.close(() => {
        sendLog('Unity discovery server closed', 'info');
      });
    } catch (err) {
      console.log('Discovery server already closed:', err.message);
    }
    discoveryServer = null;
  }
  
  // Close data server
  if (dataServer) {
    try {
      dataServer.close(() => {
        sendLog('Unity data server closed', 'info');
      });
    } catch (err) {
      console.log('Data server already closed:', err.message);
    }
    dataServer = null;
  }
  
  unityConnected = false;
  unityEndpoint = null;
  updateStatus();
  
  return { success: true };
});

// ==================== File Recording Handler ====================

/**
 * Start CSV recording
 * @param {string} filename - Desired filename without extension
 */
ipcMain.handle('start-recording', async (event, filename) => {
  try {
    // Create data directory properly with extensive error handling
    // Use app.getPath for proper path resolution in packaged app
    const appPath = app.isPackaged ? path.dirname(app.getPath('exe')) : __dirname;
    
    // --- Use ExperimentalData folder ---
    const dataDir = path.join(appPath, 'ExperimentalData');
    // --- END MODIFICATION ---
    
    sendLog(`Attempting to create/verify data directory: ${dataDir}`, 'debug');
    
    // Check if 'data' exists
    let dirExists = false;
    try {
      const stats = fs.statSync(dataDir);
      if (stats.isDirectory()) {
        dirExists = true;
        sendLog('Data directory already exists', 'debug');
      } else {
        // It's a file, not a directory - remove it
        sendLog('Found file named "ExperimentalData", removing it...', 'warning');
        fs.unlinkSync(dataDir);
      }
    } catch (err) {
      // Doesn't exist
      sendLog('ExperimentalData directory does not exist, creating...', 'debug');
    }
    
    // Create directory if it doesn't exist
    if (!dirExists) {
      try {
        fs.mkdirSync(dataDir, { recursive: true });
        sendLog(`Created data directory: ${dataDir}`, 'success');
      } catch (mkdirErr) {
        // Last resort - try without recursive
        try {
          fs.mkdirSync(dataDir);
          sendLog('Created data directory (non-recursive)', 'success');
        } catch (finalErr) {
          throw new Error(`Cannot create data directory: ${finalErr.message}`);
        }
      }
    }
    
    // Verify directory is actually accessible
    try {
      fs.accessSync(dataDir, fs.constants.W_OK);
      sendLog('Data directory is writable', 'debug');
    } catch (accessErr) {
      throw new Error(`Data directory not writable: ${accessErr.message}`);
    }
    
    // Clean filename - remove invalid characters
    let baseFilename = filename.replace(/\.csv$/i, '').replace(/[^a-zA-Z0-9_-]/g, '_');
    if (!baseFilename) {
      baseFilename = `recording_${Date.now()}`;
    }
    
    let finalFilename = baseFilename;
    let counter = 1;
    let filepath = path.join(dataDir, `${finalFilename}.csv`);
    
    // Check for existing files and track them
    const existingFiles = [];
    while (fs.existsSync(filepath)) {
      existingFiles.push(`${finalFilename}.csv`);
      finalFilename = `${baseFilename}_${counter}`;
      filepath = path.join(dataDir, `${finalFilename}.csv`);
      counter++;
    }
    
    // Notify UI if file was renamed
    if (existingFiles.length > 0 && mainWindow && !mainWindow.isDestroyed()) {
      mainWindow.webContents.send('file-renamed', {
        original: baseFilename,
        renamed: finalFilename,
        existing: existingFiles
      });
    }
    
    // Create CSV writer
    csvWriter = createCsvWriter({
      path: filepath,
      header: [
        { id: 'timestamp', title: 'Unity_Time' },
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
        { id: 'receivedAt', title: 'System_Time' }
      ]
    });
    
    // --- THIS IS THE FIX ---
    // Force-write the header to create the file immediately
    try {
      await csvWriter.writeRecords([]); // This creates the file with the header
      sendLog('CSV file created with headers', 'debug');
    } catch (writeErr) {
      throw new Error(`Failed to create file: ${writeErr.message}`);
    }
    // --- END OF FIX ---
    
    isRecording = true;
    currentFilename = finalFilename;
    dataBuffer = [];
    
    sendLog(`Recording started: ${finalFilename}.csv`, 'success');
    sendLog(`Saving to: ${filepath}`, 'info');
    sendLog(`File created: ExperimentalData/${finalFilename}.csv`, 'success');
    
    return { success: true, filename: finalFilename, path: filepath };
    
  } catch (error) {
    // Detailed error logging
    sendLog(`Failed to start recording: ${error.message}`, 'error');
    sendLog(`Error code: ${error.code}`, 'error');
    sendLog(`Error stack: ${error.stack}`, 'debug');
    
    // Try to diagnose the issue
    try {
      const appPath = app.isPackaged ? path.dirname(app.getPath('exe')) : __dirname;
      const dataDir = path.join(appPath, 'data');
      sendLog(`Data directory path: ${dataDir}`, 'error');
      sendLog(`__dirname: ${__dirname}`, 'error');
      sendLog(`app.isPackaged: ${app.isPackaged}`, 'error');
      
      if (fs.existsSync(dataDir)) {
        const stats = fs.statSync(dataDir);
        sendLog(`Data exists - isDirectory: ${stats.isDirectory()}, isFile: ${stats.isFile()}`, 'error');
      } else {
        sendLog('Data path does not exist!', 'error');
      }
    } catch (diagErr) {
      sendLog(`Diagnostic error: ${diagErr.message}`, 'error');
    }
    
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
    
    const appPath = app.isPackaged ? path.dirname(app.getPath('exe')) : __dirname;
    
    // --- Use ExperimentalData folder ---
    const filePath = path.join(appPath, 'ExperimentalData', recordedFile + '.csv');
    // --- END MODIFICATION ---
    
    sendLog(`Recording stopped: ${recordedFile}.csv`, 'success');
    
    // --- FIX for log message ---
    sendLog(`File location: ExperimentalData/${recordedFile}.csv`, 'info');
    // --- END FIX ---
    
    sendLog(`Full path: ${filePath}`, 'info');
    
    // --- FIX for return value ---
    return { success: true, filename: recordedFile, path: filePath };
    // --- END FIX ---
    
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
    // Clean up existing connection
    if (handshakeInterval) {
      clearInterval(handshakeInterval);
      handshakeInterval = null;
    }
    
    if (serialPort && serialPort.isOpen) {
      try {
        serialPort.close();
      } catch (err) {
        console.log('Serial cleanup:', err.message);
      }
      serialPort = null;
      arduinoConnected = false;
    }
    
    // List all available ports
    const ports = await SerialPort.list();
    sendLog(`Found ${ports.length} serial ports`, 'info');
    
    // Log all ports for debugging
    ports.forEach(port => {
      console.log(`Port: ${port.path}, Manufacturer: ${port.manufacturer || 'Unknown'}`);
      sendLog(`Port: ${port.path}, Manufacturer: ${port.manufacturer || 'Unknown'}`, 'info');
    });
    
    // Find Arduino port
    let arduinoPort = ports.find(port => {
      return port.manufacturer && (
        port.manufacturer.includes('Arduino') ||
        port.manufacturer.includes('arduino') ||
        port.manufacturer.includes('CH340') ||
        port.manufacturer.includes('FTDI') ||
        port.manufacturer.includes('USB')
      );
    });
    
    // Fallback to first available port
    if (!arduinoPort && ports.length > 0) {
      arduinoPort = ports[0];
      sendLog(`No Arduino detected by name, trying ${arduinoPort.path}`, 'warning');
    }
    
    if (!arduinoPort) {
      throw new Error('No serial ports found. Is Arduino plugged in?');
    }
    
    sendLog(`Opening serial port ${arduinoPort.path}...`, 'info');
    
    // Create serial port connection
    serialPort = new SerialPort({
      path: arduinoPort.path,
      baudRate: 9600,
      autoOpen: false
    });
    
    // Set up line parser
    const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\r\n' }));
    
    // Handle incoming data
    parser.on('data', (data) => {
      sendLog(`Arduino: ${data}`, 'arduino');
      
      // Check for handshake confirmation
      if (data.includes('Vibration Motor Controller Ready')) {
        arduinoConnected = true;
        sendLog('Arduino handshake confirmed!', 'success');
        updateStatus();
        
        if (handshakeInterval) {
          clearInterval(handshakeInterval);
          handshakeInterval = null;
        }
      }
      
      // Log motor status
      if (data.includes('Motor ON') || data.includes('Motor activated')) {
        sendLog('Motor vibrating', 'success');
      }
      if (data.includes('Motor OFF')) {
        sendLog('Motor stopped', 'info');
      }
    });
    
    // Handle errors
    serialPort.on('error', (err) => {
      sendLog(`Arduino serial error: ${err.message}`, 'error');
      arduinoConnected = false;
      updateStatus();
    });
    
    // Handle close
    serialPort.on('close', () => {
      sendLog('Arduino serial port closed', 'warning');
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
    
    sendLog(`Serial port opened successfully`, 'success');
    
    // Send handshake with retry
    let attempts = 0;
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
      
      attempts++;
      if (attempts > maxAttempts) {
        sendLog('Arduino handshake timeout - check if correct sketch is uploaded', 'error');
        if (handshakeInterval) clearInterval(handshakeInterval);
        return;
      }
      
      serialPort.write('HELLO\n', (err) => {
        if (!err) {
          sendLog(`Handshake attempt ${attempts}/${maxAttempts}`, 'info');
        }
      });
    };
    
    // Initial handshake after delay
    setTimeout(sendHandshake, 500);
    
    // Retry every second
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
      throw new Error('Arduino not connected');
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

 // ==================== Unity Experiment Control ====================
 
 /**
  * Start left hand experiment in Unity
  * @param {string} experimentId - Experiment ID to send to Unity
  */
 ipcMain.handle('start-left-experiment', async (event, experimentId) => {
   try {
     if (!unityConnected) {
       throw new Error('Unity not connected');
     }
     
     if (!unityEndpoint || !dataServer) {
       throw new Error('Unity endpoint not established');
     }
     
     // Send START_LEFT_EXPERIMENT command with experiment ID to Unity
     const commandMsg = `${MSG_COMMAND}:${CMD_START_LEFT_EXPERIMENT}:${experimentId}`;
     dataServer.send(
       Buffer.from(commandMsg),
       UNITY_DATA_PORT,
       unityEndpoint.address,
       (err) => {
         if (err) {
           sendLog(`Failed to send start left command: ${err.message}`, 'error');
         } else {
           sendLog(`Start Left Hand Experiment command sent to Unity with ID: ${experimentId}`, 'success');
         }
       }
     );
     
     return { success: true };
     
   } catch (error) {
     sendLog(`Failed to start left hand experiment: ${error.message}`, 'error');
     return { success: false, error: error.message };
   }
 });
 
 /**
  * Start right hand experiment in Unity
  * @param {string} experimentId - Experiment ID to send to Unity
  */
 ipcMain.handle('start-right-experiment', async (event, experimentId) => {
   try {
     if (!unityConnected) {
       throw new Error('Unity not connected');
     }
     
     if (!unityEndpoint || !dataServer) {
       throw new Error('Unity endpoint not established');
     }
     
     // Send START_RIGHT_EXPERIMENT command with experiment ID to Unity
     const commandMsg = `${MSG_COMMAND}:${CMD_START_RIGHT_EXPERIMENT}:${experimentId}`;
     dataServer.send(
       Buffer.from(commandMsg),
       UNITY_DATA_PORT,
       unityEndpoint.address,
       (err) => {
         if (err) {
           sendLog(`Failed to send start right command: ${err.message}`, 'error');
         } else {
           sendLog(`Start Right Hand Experiment command sent to Unity with ID: ${experimentId}`, 'success');
         }
       }
     );
     
     return { success: true };
     
   } catch (error) {
     sendLog(`Failed to start right hand experiment: ${error.message}`, 'error');
     return { success: false, error: error.message };
   }
 });

/**
 * Stop experiment in Unity
 */
ipcMain.handle('stop-experiment', async () => {
  try {
    if (!unityConnected) {
      throw new Error('Unity not connected');
    }
    
    if (!unityEndpoint || !dataServer) {
      throw new Error('Unity endpoint not established');
    }
    
    // Send STOP_EXPERIMENT command to Unity
    const commandMsg = `${MSG_COMMAND}:${CMD_STOP_EXPERIMENT}`;
    dataServer.send(
      Buffer.from(commandMsg),
      UNITY_DATA_PORT,
      unityEndpoint.address,
      (err) => {
        if (err) {
          sendLog(`Failed to send stop command: ${err.message}`, 'error');
        } else {
          sendLog('Stop Experiment command sent to Unity', 'success');
        }
      }
    );
    
    return { success: true };
    
  } catch (error) {
    sendLog(`Failed to stop experiment: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Save data mid-experiment without stopping
 */
ipcMain.handle('save-mid-experiment', async () => {
  try {
    if (!unityConnected) {
      throw new Error('Unity not connected');
    }
    
    if (!unityEndpoint || !dataServer) {
      throw new Error('Unity endpoint not established');
    }
    
    // Send SAVE_MID_EXPERIMENT command to Unity
    const commandMsg = `${MSG_COMMAND}:${CMD_SAVE_MID_EXPERIMENT}`;
    dataServer.send(
      Buffer.from(commandMsg),
      UNITY_DATA_PORT,
      unityEndpoint.address,
      (err) => {
        if (err) {
          sendLog(`Failed to send mid-save command: ${err.message}`, 'error');
        } else {
          sendLog('Save Mid Experiment command sent to Unity', 'success');
        }
      }
    );
    
    return { success: true };
    
  } catch (error) {
    sendLog(`Failed to send mid-save command: ${error.message}`, 'error');
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
    try {
      serialPort.close();
      sendLog('Arduino disconnected', 'info');
    } catch (err) {
      console.log('Serial port already closed:', err.message);
    }
    serialPort = null;
    arduinoConnected = false;
    updateStatus();
  }
  
  return { success: true };
});

// ==================== Folder Operations ====================

/**
 * Open Experiment Data folder in Windows Explorer
 */
ipcMain.handle('open-data-folder', async () => {
  try {
    // Create directory if it doesn't exist
    if (!fs.existsSync(EXPERIMENT_DATA_PATH)) {
      try {
        fs.mkdirSync(EXPERIMENT_DATA_PATH, { recursive: true });
        sendLog(`Created ExperimentData folder: ${EXPERIMENT_DATA_PATH}`, 'success');
      } catch (mkdirErr) {
        throw new Error(`Cannot create data directory: ${mkdirErr.message}`);
      }
    }
    
    // Create subdirectories if they don't exist
    if (!fs.existsSync(LEFT_HAND_PATH)) {
      fs.mkdirSync(LEFT_HAND_PATH, { recursive: true });
    }
    if (!fs.existsSync(RIGHT_HAND_PATH)) {
      fs.mkdirSync(RIGHT_HAND_PATH, { recursive: true });
    }
    
    // Open folder in Windows Explorer (or default file manager on other OS)
    const openResult = await shell.openPath(EXPERIMENT_DATA_PATH);
    
    if (openResult) {
      // openPath returns empty string on success, error message on failure
      throw new Error(`Failed to open folder: ${openResult}`);
    }
    
    sendLog(`Opened folder: ${EXPERIMENT_DATA_PATH}`, 'success');
    return { success: true, path: EXPERIMENT_DATA_PATH };
    
  } catch (error) {
    sendLog(`Failed to open data folder: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Get current ADB path from configuration
 */
ipcMain.handle("get-adb-path", () => {
  return loadAdbPath();
});

/**
 * Set ADB path by opening file dialog
 */
ipcMain.handle("set-adb-path", async () => {
  const result = await dialog.showOpenDialog(mainWindow, {
    title: "Select adb.exe",
    filters: [
      { name: "ADB Executable", extensions: ["exe"] }
    ],
    properties: ["openFile"]
  });

  if (result.canceled || result.filePaths.length === 0) {
    return { success: false, error: "No file selected" };
  }

  const selected = result.filePaths[0];
  saveAdbPath(selected);
  sendLog(`ADB path saved: ${selected}`, 'success');

  return { success: true, adbPath: selected };
});

/**
 * Sync experiment data from headset (ADB pull) into ExperimentalData
 * Copies: /sdcard/Android/data/com.AimLab.NHPT/files/HandMovement/.
 */
ipcMain.handle('sync-experiment-data', async () => {
  try {
    const adbPath = loadAdbPath();   // Use stored ADB path

    // Ensure ExperimentData directory exists (same logic as open-data-folder)
    if (!fs.existsSync(EXPERIMENT_DATA_PATH)) {
      try {
        fs.mkdirSync(EXPERIMENT_DATA_PATH, { recursive: true });
        sendLog(`Created ExperimentData folder: ${EXPERIMENT_DATA_PATH}`, 'success');
      } catch (mkdirErr) {
        throw new Error(`Cannot create data directory: ${mkdirErr.message}`);
      }
    }
    
    // Create subdirectories if they don't exist
    if (!fs.existsSync(LEFT_HAND_PATH)) {
      fs.mkdirSync(LEFT_HAND_PATH, { recursive: true });
    }
    if (!fs.existsSync(RIGHT_HAND_PATH)) {
      fs.mkdirSync(RIGHT_HAND_PATH, { recursive: true });
    }

    const sourcePath = '/sdcard/Android/data/com.AimLab.NHPT/files/HandMovement/.';
    const adbArgs = ['-d', 'pull', sourcePath, EXPERIMENT_DATA_PATH];

    sendLog(`Starting ADB sync: ${adbPath} ${adbArgs.join(' ')}`, 'info');

    return await new Promise((resolve) => {
      execFile(adbPath, adbArgs, { windowsHide: true }, (error, stdout, stderr) => {
        if (error) {
          sendLog(`ADB sync failed: ${error.message}`, 'error');
          if (stderr) {
            sendLog(`ADB stderr: ${stderr}`, 'error');
          }
          return resolve({
            success: false,
            error: error.message,
            stderr: stderr || ''
          });
        }

        if (stdout) {
          sendLog(`ADB output: ${stdout}`, 'info');
        }

        sendLog(`ADB sync completed to: ${EXPERIMENT_DATA_PATH}`, 'success');
        resolve({
          success: true,
          path: EXPERIMENT_DATA_PATH,
          stdout: stdout || ''
        });
      });
    });
  } catch (error) {
    sendLog(`Sync experiment data error: ${error.message}`, 'error');
    return { success: false, error: error.message };
  }
});

/**
 * Check if file with given experiment ID already exists
 * @param {string} experimentId - Experiment ID to check
 */
ipcMain.handle('check-file-exists', async (event, experimentId) => {
  try {
    const appPath = app.isPackaged ? path.dirname(app.getPath('exe')) : __dirname;
    const dataDir = path.join(appPath, 'ExperimentalData');
    
    // Clean filename
    const cleanFilename = experimentId.replace(/\.csv$/i, '').replace(/[^a-zA-Z0-9_-]/g, '_');
    const filepath = path.join(dataDir, `${cleanFilename}.csv`);
    
    const exists = fs.existsSync(filepath);
    
    if (exists) {
      sendLog(`File check: ${cleanFilename}.csv already exists`, 'warning');
    } else {
      sendLog(`File check: ${cleanFilename}.csv is available`, 'debug');
    }
    
    return { success: true, exists: exists, filename: cleanFilename };
    
  } catch (error) {
    sendLog(`Error checking file existence: ${error.message}`, 'error');
    return { success: false, exists: false, error: error.message };
  }
});

// ==================== TSV File Handling ====================

/**
 * Handle TSV transfer header from Unity
 * Format: "TSV_HEADER|transferId|fileName|totalChunks|totalSize"
 * @param {string} message - TSV header message
 */
function handleTSVHeader(message) {
  try {
    // Parse: "TSV_HEADER|transferId|fileName|totalChunks|totalSize"
    const parts = message.split('|');
    if (parts.length >= 5) {
      const transferId = parts[1];
      const fileName = parts[2];
      const totalChunks = parseInt(parts[3]);
      const totalSize = parseInt(parts[4]);
      
      // Initialize transfer tracking
      tsvTransfers.set(transferId, {
        fileName: fileName,
        totalChunks: totalChunks,
        totalSize: totalSize,
        chunks: new Array(totalChunks),
        receivedChunks: 0,
        startTime: Date.now()
      });
      
      sendLog(`Starting TSV transfer: ${fileName} (${totalChunks} chunks, ${totalSize} bytes)`, 'info');
      console.log(`[Main] Starting TSV transfer: ${fileName} (${totalChunks} chunks, ${totalSize} bytes)`);
    } else {
      sendLog('Invalid TSV header format', 'error');
    }
  } catch (error) {
    sendLog(`Error handling TSV header: ${error.message}`, 'error');
    console.error('[Main] Error handling TSV header:', error);
  }
}

/**
 * Handle TSV chunk from Unity
 * Format: "TSV_CHUNK|transferId|chunkIndex|base64Data"
 * @param {string} message - TSV chunk message
 */
function handleTSVChunk(message) {
  try {
    // Parse: "TSV_CHUNK|transferId|chunkIndex|base64Data"
    // Use indexOf to handle base64 data that might contain | characters
    const firstPipe = message.indexOf('|');
    const secondPipe = message.indexOf('|', firstPipe + 1);
    const thirdPipe = message.indexOf('|', secondPipe + 1);
    
    if (firstPipe === -1 || secondPipe === -1 || thirdPipe === -1) {
      sendLog('Invalid TSV chunk format', 'error');
      return;
    }
    
    const transferId = message.substring(firstPipe + 1, secondPipe);
    const chunkIndex = parseInt(message.substring(secondPipe + 1, thirdPipe));
    const base64Data = message.substring(thirdPipe + 1);
    
    const transfer = tsvTransfers.get(transferId);
    if (!transfer) {
      sendLog(`Unknown transfer ID: ${transferId}`, 'error');
      console.error(`[Main] Unknown transfer ID: ${transferId}`);
      return;
    }
    
    // Validate chunk index
    if (chunkIndex < 0 || chunkIndex >= transfer.totalChunks) {
      sendLog(`Invalid chunk index: ${chunkIndex}`, 'error');
      return;
    }
    
    // Store chunk (decode from base64)
    try {
      transfer.chunks[chunkIndex] = Buffer.from(base64Data, 'base64');
      transfer.receivedChunks++;
      
      // Log progress every 10 chunks or on last chunk
      if (transfer.receivedChunks % 10 === 0 || transfer.receivedChunks === transfer.totalChunks) {
        const progress = Math.round((transfer.receivedChunks / transfer.totalChunks) * 100);
        sendLog(`TSV transfer progress: ${progress}% (${transfer.receivedChunks}/${transfer.totalChunks} chunks)`, 'info');
        console.log(`[Main] TSV transfer progress: ${transfer.receivedChunks}/${transfer.totalChunks} (${progress}%)`);
        
        // Send progress update to renderer
        if (mainWindow && !mainWindow.isDestroyed()) {
          mainWindow.webContents.send('tsv-progress', {
            fileName: transfer.fileName,
            progress: progress,
            received: transfer.receivedChunks,
            total: transfer.totalChunks
          });
        }
      }
    } catch (decodeError) {
      sendLog(`Error decoding chunk ${chunkIndex}: ${decodeError.message}`, 'error');
      console.error(`[Main] Error decoding chunk ${chunkIndex}:`, decodeError);
    }
  } catch (error) {
    sendLog(`Error handling TSV chunk: ${error.message}`, 'error');
    console.error('[Main] Error handling TSV chunk:', error);
  }
}

/**
 * Handle TSV transfer completion from Unity
 * Format: "TSV_COMPLETE|transferId"
 * @param {string} message - TSV completion message
 */
function handleTSVComplete(message) {
  try {
    // Parse: "TSV_COMPLETE|transferId"
    const parts = message.split('|');
    if (parts.length >= 2) {
      const transferId = parts[1];
      
      const transfer = tsvTransfers.get(transferId);
      if (!transfer) {
        sendLog(`Unknown transfer ID for completion: ${transferId}`, 'error');
        console.error(`[Main] Unknown transfer ID for completion: ${transferId}`);
        return;
      }
      
      // Check if all chunks received
      if (transfer.receivedChunks !== transfer.totalChunks) {
        sendLog(`Incomplete transfer: received ${transfer.receivedChunks}/${transfer.totalChunks} chunks for ${transfer.fileName}`, 'error');
        console.error(`[Main] Incomplete transfer: received ${transfer.receivedChunks}/${transfer.totalChunks} chunks`);
        
        // Clean up incomplete transfer
        tsvTransfers.delete(transferId);
        return;
      }
      
      // Check for missing chunks
      let missingChunks = [];
      for (let i = 0; i < transfer.totalChunks; i++) {
        if (!transfer.chunks[i]) {
          missingChunks.push(i);
        }
      }
      
      if (missingChunks.length > 0) {
        sendLog(`Missing chunks: ${missingChunks.join(', ')} for ${transfer.fileName}`, 'error');
        console.error(`[Main] Missing chunks: ${missingChunks.join(', ')}`);
        tsvTransfers.delete(transferId);
        return;
      }
      
      // Reassemble the file
      const completeBuffer = Buffer.concat(transfer.chunks);
      const fileContent = completeBuffer.toString('utf8');
      
      // Determine which hand folder based on filename
      let subFolder = '';
      if (transfer.fileName.includes('LEFT_') || transfer.fileName.includes('HandLeft') || transfer.fileName.includes('Left')) {
        subFolder = 'Left_Hand';
      } else if (transfer.fileName.includes('RIGHT_') || transfer.fileName.includes('HandRight') || transfer.fileName.includes('Right')) {
        subFolder = 'Right_Hand';
      }
      
      // Save to ExperimentData folder (use consistent path)
      const folderPath = subFolder === 'Left_Hand' ? LEFT_HAND_PATH 
        : subFolder === 'Right_Hand' ? RIGHT_HAND_PATH 
        : EXPERIMENT_DATA_PATH;
      
      // Create directory if it doesn't exist
      if (!fs.existsSync(folderPath)) {
        fs.mkdirSync(folderPath, { recursive: true });
        sendLog(`Created directory: ${folderPath}`, 'success');
      }
      
      const filePath = path.join(folderPath, transfer.fileName);
      fs.writeFileSync(filePath, fileContent, 'utf8');
      
      // Calculate transfer time
      const transferTime = ((Date.now() - transfer.startTime) / 1000).toFixed(2);
      
      sendLog(`TSV file saved: ${transfer.fileName} (transfer took ${transferTime}s)`, 'success');
      sendLog(`File location: ${filePath}`, 'info');
      console.log(`[Main] TSV file saved: ${filePath} (transfer took ${transferTime}s)`);
      
      // Notify renderer
      if (mainWindow && !mainWindow.isDestroyed()) {
        mainWindow.webContents.send('tsv-saved', { 
          fileName: transfer.fileName, 
          path: filePath,
          transferTime: transferTime,
          fileSize: transfer.totalSize
        });
      }
      
      // Clean up transfer tracking
      tsvTransfers.delete(transferId);
    } else {
      sendLog('Invalid TSV complete format', 'error');
    }
  } catch (error) {
    sendLog(`Error completing TSV transfer: ${error.message}`, 'error');
    console.error('[Main] Error completing TSV transfer:', error);
  }
}

/**
 * Handle TSV file received from Unity (legacy non-chunked format)
 * Format: "TSV_FILE|filename|content"
 * @param {string} message - Complete TSV file message
 */
function handleTSVFileReceived(message) {
  try {
    // Parse message format: "TSV_FILE|filename|content"
    const parts = message.split('|');
    if (parts.length >= 3) {
      const fileName = parts[1];
      const fileContent = parts.slice(2).join('|'); // In case content has | characters
      
      // Determine which hand folder based on filename
      let subFolder = '';
      if (fileName.includes('LEFT_') || fileName.includes('HandLeft') || fileName.includes('Left')) {
        subFolder = 'Left_Hand';
      } else if (fileName.includes('RIGHT_') || fileName.includes('HandRight') || fileName.includes('Right')) {
        subFolder = 'Right_Hand';
      }
      
      // Save to ExperimentData folder (use consistent path)
      const folderPath = subFolder === 'Left_Hand' ? LEFT_HAND_PATH 
        : subFolder === 'Right_Hand' ? RIGHT_HAND_PATH 
        : EXPERIMENT_DATA_PATH;
      
      // Create directory if it doesn't exist
      if (!fs.existsSync(folderPath)) {
        fs.mkdirSync(folderPath, { recursive: true });
        sendLog(`Created directory: ${folderPath}`, 'success');
      }
      
      const filePath = path.join(folderPath, fileName);
      fs.writeFileSync(filePath, fileContent, 'utf8');
      
      sendLog(`TSV file saved: ${fileName}`, 'success');
      sendLog(`File location: ${filePath}`, 'info');
      
      // Notify renderer
      if (mainWindow && !mainWindow.isDestroyed()) {
        mainWindow.webContents.send('tsv-saved', { fileName, path: filePath });
      }
    } else {
      sendLog('Invalid TSV file format received', 'error');
    }
  } catch (error) {
    sendLog(`Error saving TSV file: ${error.message}`, 'error');
    console.error('[Main] Error saving TSV file:', error);
  }
}

/**
 * Check for existing experiment files with given ID
 * @param {string} experimentId - Experiment ID to check
 */
ipcMain.handle('check-experiment-files', async (event, experimentId) => {
  try {
    // Create directories if they don't exist
    if (!fs.existsSync(LEFT_HAND_PATH)) {
      fs.mkdirSync(LEFT_HAND_PATH, { recursive: true });
    }
    if (!fs.existsSync(RIGHT_HAND_PATH)) {
      fs.mkdirSync(RIGHT_HAND_PATH, { recursive: true });
    }
    
    let existingFiles = [];
    
    // Check Left_Hand folder
    if (fs.existsSync(LEFT_HAND_PATH)) {
      try {
        const leftFiles = fs.readdirSync(LEFT_HAND_PATH);
        const matchingLeft = leftFiles.filter(file => 
          file.includes(experimentId) && (file.endsWith('.tsv') || file.endsWith('.csv'))
        );
        existingFiles = existingFiles.concat(matchingLeft.map(f => `Left_Hand/${f}`));
      } catch (err) {
        sendLog(`Error reading Left_Hand folder: ${err.message}`, 'error');
      }
    }
    
    // Check Right_Hand folder
    if (fs.existsSync(RIGHT_HAND_PATH)) {
      try {
        const rightFiles = fs.readdirSync(RIGHT_HAND_PATH);
        const matchingRight = rightFiles.filter(file => 
          file.includes(experimentId) && (file.endsWith('.tsv') || file.endsWith('.csv'))
        );
        existingFiles = existingFiles.concat(matchingRight.map(f => `Right_Hand/${f}`));
      } catch (err) {
        sendLog(`Error reading Right_Hand folder: ${err.message}`, 'error');
      }
    }
    
    return existingFiles;
  } catch (error) {
    sendLog(`Error checking experiment files: ${error.message}`, 'error');
    console.error('[Main] Error checking experiment files:', error);
    return [];
  }
});

// ==================== TSV Transfer Cleanup ====================

/**
 * Cleanup timed-out TSV transfers
 * Runs every 30 seconds to remove transfers older than 60 seconds
 */
setInterval(() => {
  const now = Date.now();
  const timeout = 60000; // 60 seconds timeout
  
  for (const [id, transfer] of tsvTransfers.entries()) {
    if (now - transfer.startTime > timeout) {
      sendLog(`Cleaning up timed-out transfer: ${transfer.fileName}`, 'warning');
      console.log(`[Main] Cleaning up timed-out transfer: ${transfer.fileName}`);
      tsvTransfers.delete(id);
    }
  }
}, 30000); // Check every 30 seconds

// ==================== Application Lifecycle ====================

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  cleanupConnections();
  
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('before-quit', (event) => {
  cleanupConnections();
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

// Handle app termination signals
process.on('SIGINT', () => {
  cleanupConnections();
  process.exit(0);
});

process.on('SIGTERM', () => {
  cleanupConnections();
  process.exit(0);
});
