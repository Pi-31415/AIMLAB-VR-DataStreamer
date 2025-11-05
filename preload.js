/**
 * AIMLAB VR Streamer - Preload Script
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 05 November 2025
 * Version: v3.4
 * 
 * Description:
 * Preload script for Electron contextBridge to safely expose IPC functionality
 * for Unity UDP data streaming and Arduino serial communication.
 * 
 * Changelog:
 * v3.4 - 05 November 2025 - Added left/right hand experiment support
 *        - Added startLeftExperiment and startRightExperiment functions
 *        - Separate commands for left and right hand experiments
 * v3.3 - 05 November 2025 - Added file existence checking
 *        - Added checkFileExists function for validation before recording
 *        - Exposed recording handlers for experiment integration
 * v3.2 - 05 November 2025 - Added folder opening functionality
 *        - Added openDataFolder function for direct folder access
 *        - Retained recording handlers for internal use
 * v2.0 - 04 November 2025 - Major update for VR data collection
 *        - Added Unity UDP connection handlers
 *        - Added Arduino serial communication handlers
 *        - Added CSV recording functionality
 *        - Added status update listeners
 * v1.0 - 04 November 2025 - Initial implementation
 *        - Set up contextBridge API
 *        - Exposed streaming controls to renderer
 */

const { contextBridge, ipcRenderer } = require('electron');

/**
 * Expose protected methods that allow the renderer process to use
 * ipcRenderer without exposing the entire object
 */
contextBridge.exposeInMainWorld('api', {
  // Unity UDP Connection
  /**
   * Connect to Unity VR application via UDP
   * @param {number} port - UDP port to listen on (default: 8888)
   * @returns {Promise<Object>} Result with success status
   */
  connectToUnity: (port) => ipcRenderer.invoke('connect-unity', port),
  
  /**
   * Disconnect from Unity
   * @returns {Promise<Object>} Result with success status
   */
  disconnectFromUnity: () => ipcRenderer.invoke('disconnect-unity'),
  
  // File Operations
  /**
   * Start recording VR data to CSV file
   * @param {string} filename - Name of the CSV file (without extension)
   * @returns {Promise<Object>} Result with success status and filename
   */
  startRecording: (filename) => ipcRenderer.invoke('start-recording', filename),
  
  /**
   * Stop recording and save CSV file
   * @returns {Promise<Object>} Result with success status
   */
  stopRecording: () => ipcRenderer.invoke('stop-recording'),
  
  // Arduino Serial Connection
  /**
   * Connect to Arduino vibration motor controller
   * @returns {Promise<Object>} Result with success status and port name
   */
  connectToArduino: () => ipcRenderer.invoke('connect-arduino'),
  
  /**
   * Disconnect from Arduino
   * @returns {Promise<Object>} Result with success status
   */
  disconnectFromArduino: () => ipcRenderer.invoke('disconnect-arduino'),
  
  /**
   * Send test command to vibration motor
   * @returns {Promise<Object>} Result with success status
   */
  testMotor: () => ipcRenderer.invoke('test-motor'),
  
  // Status Updates (Event Listeners)
  /**
   * Listen for connection status updates
   * @param {Function} callback - Callback function receiving status object
   */
  onStatusUpdate: (callback) => ipcRenderer.on('status-update', callback),
  
  /**
   * Listen for incoming VR data
   * @param {Function} callback - Callback function receiving data object
   */
  onDataReceived: (callback) => ipcRenderer.on('data-received', callback),
  
  /**
   * Listen for log messages
   * @param {Function} callback - Callback function receiving log object
   */
  onLog: (callback) => ipcRenderer.on('log-message', callback),
  
  // Experiment Control
  /**
   * Start left hand experiment in Unity
   * @returns {Promise<Object>} Result with success status
   */
  startLeftExperiment: () => ipcRenderer.invoke('start-left-experiment'),
  
  /**
   * Start right hand experiment in Unity
   * @returns {Promise<Object>} Result with success status
   */
  startRightExperiment: () => ipcRenderer.invoke('start-right-experiment'),
  
  /**
   * Stop experiment in Unity
   * @returns {Promise<Object>} Result with success status
   */
  stopExperiment: () => ipcRenderer.invoke('stop-experiment'),
  
  /**
   * Listen for file rename notifications
   * @param {Function} callback - Callback function receiving rename data
   */
  onFileRenamed: (callback) => ipcRenderer.on('file-renamed', callback),
  
  // Folder Operations
  /**
   * Open Experiment Data folder in Windows Explorer
   * @returns {Promise<Object>} Result with success status and path
   */
  openDataFolder: () => ipcRenderer.invoke('open-data-folder'),
  
  /**
   * Check if a file with the given experiment ID already exists
   * @param {string} experimentId - Experiment ID to check
   * @returns {Promise<Object>} Result with exists boolean
   */
  checkFileExists: (experimentId) => ipcRenderer.invoke('check-file-exists', experimentId)
});

