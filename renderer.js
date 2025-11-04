/**
 * AIMLAB VR Data Collector - Renderer Process
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 November 2025
 * Version: v2.0
 * 
 * Description:
 * Renderer process JavaScript for AIMLAB VR Data Collector.
 * Handles UI interactions for Unity UDP connection, Arduino serial
 * communication, and CSV recording controls.
 * 
 * Changelog:
 * v2.0 - 04 November 2025 - Data collection interface
 *        - Unity UDP connection management
 *        - Arduino serial communication
 *        - CSV recording controls
 *        - Debug console and live data preview
 * v1.0 - 04 November 2025 - Initial UI logic
 *        - Basic event handlers and simulated data
 */

// ==================== State Management ====================
let unityConnected = false;
let arduinoConnected = false;
let isRecording = false;

// ==================== DOM Elements ====================
const elements = {
    // Status indicators
    unityStatus: document.getElementById('unityStatus'),
    arduinoStatus: document.getElementById('arduinoStatus'),
    
    // Buttons
    connectUnity: document.getElementById('connectUnity'),
    refreshUnity: document.getElementById('refreshUnity'),
    startExperiment: document.getElementById('startExperiment'),
    stopExperiment: document.getElementById('stopExperiment'),
    connectArduino: document.getElementById('connectArduino'),
    testMotor: document.getElementById('testMotor'),
    refreshArduino: document.getElementById('refreshArduino'),
    startRecording: document.getElementById('startRecording'),
    stopRecording: document.getElementById('stopRecording'),
    clearLog: document.getElementById('clearLog'),
    
    // Inputs
    filename: document.getElementById('filename'),
    
    // Output areas
    logOutput: document.getElementById('logOutput'),
    recordingStatus: document.getElementById('recordingStatus'),
    
    // Modal
    modal: document.getElementById('modal'),
    modalMessage: document.getElementById('modalMessage'),
    modalClose: document.getElementById('modalClose')
};

// ==================== Initialization ====================

/**
 * Initialize application when DOM is loaded
 */
document.addEventListener('DOMContentLoaded', () => {
    setupEventListeners();
    addLog('AIMLAB VR Data Collector initialized', 'success');
    addLog('Ready to connect to Unity and Vibration Motor', 'info');
    addLog('Discovery port: 45001 | Data port: 45102', 'info');
});

// ==================== Event Listeners Setup ====================

/**
 * Setup all event listeners for buttons and IPC
 */
function setupEventListeners() {
    // Unity Controls
    elements.connectUnity.addEventListener('click', connectToUnity);
    elements.refreshUnity.addEventListener('click', refreshUnityConnection);
    elements.startExperiment.addEventListener('click', startExperiment);
    elements.stopExperiment.addEventListener('click', stopExperiment);
    
    // Arduino Controls
    elements.connectArduino.addEventListener('click', connectToArduino);
    elements.testMotor.addEventListener('click', testMotor);
    elements.refreshArduino.addEventListener('click', refreshArduinoConnection);
    
    // Recording Controls
    elements.startRecording.addEventListener('click', startRecording);
    elements.stopRecording.addEventListener('click', stopRecording);
    
    // Utility
    elements.clearLog.addEventListener('click', clearLog);
    elements.modalClose.addEventListener('click', closeModal);
    
    // IPC Listeners
    window.api.onStatusUpdate((event, status) => {
        updateConnectionStatus(status);
    });
    
    window.api.onDataReceived((event, data) => {
        displayData(data);
    });
    
    window.api.onLog((event, log) => {
        addLog(log.message, log.type);
    });
    
    window.api.onFileRenamed((event, data) => {
        showFileRenameModal(data);
    });
}

// ==================== Unity Connection Functions ====================

/**
 * Connect to Unity VR application
 */
async function connectToUnity() {
    elements.connectUnity.disabled = true;
    addLog('Connecting to Unity on port 45101...', 'info');
    
    const result = await window.api.connectToUnity(45101);
    
    if (result.success) {
        elements.connectUnity.textContent = 'Connected';
        addLog('Unity UDP listener started', 'success');
        addLog('Waiting for Unity to send HANDSHAKE...', 'info');
    } else {
        elements.connectUnity.disabled = false;
        addLog(`Failed to connect: ${result.error}`, 'error');
    }
}

/**
 * Refresh Unity connection
 */
async function refreshUnityConnection() {
    addLog('Refreshing Unity connection...', 'info');
    await window.api.disconnectFromUnity();
    elements.connectUnity.disabled = false;
    elements.connectUnity.textContent = 'Connect to Unity';
    updateConnectionStatus({ unity: false, arduino: arduinoConnected });
    setTimeout(() => connectToUnity(), 500);
}

/**
 * Start experiment in Unity
 */
async function startExperiment() {
    addLog('Sending Start Experiment command to Unity...', 'info');
    const result = await window.api.startExperiment();
    
    if (result.success) {
        elements.startExperiment.disabled = true;
        elements.stopExperiment.disabled = false;
        addLog('Experiment started in Unity', 'success');
        addLog('Unity should now be sending VR data', 'info');
    } else {
        addLog(`Failed to start experiment: ${result.error}`, 'error');
    }
}

/**
 * Stop experiment in Unity
 */
async function stopExperiment() {
    addLog('Sending Stop Experiment command to Unity...', 'info');
    const result = await window.api.stopExperiment();
    
    if (result.success) {
        elements.startExperiment.disabled = false;
        elements.stopExperiment.disabled = true;
        addLog('Experiment stopped in Unity', 'success');
    } else {
        addLog(`Failed to stop experiment: ${result.error}`, 'error');
    }
}

// ==================== Vibration Motor Connection Functions ====================

/**
 * Connect to vibration motor controller
 */
async function connectToArduino() {
    elements.connectArduino.disabled = true;
    addLog('Scanning for vibration motor controller...', 'info');
    
    const result = await window.api.connectToArduino();
    
    if (result.success) {
        elements.connectArduino.textContent = 'Connected';
        elements.testMotor.disabled = false;
        addLog(`Vibration motor connected on ${result.port}`, 'success');
        addLog('Waiting for motor handshake...', 'info');
    } else {
        elements.connectArduino.disabled = false;
        addLog(`Failed to connect: ${result.error}`, 'error');
    }
}

/**
 * Test vibration motor
 */
async function testMotor() {
    addLog('Sending test command to motor...', 'info');
    const result = await window.api.testMotor();
    
    if (!result.success) {
        addLog(`Motor test failed: ${result.error}`, 'error');
    }
}

/**
 * Refresh vibration motor connection
 */
async function refreshArduinoConnection() {
    addLog('Refreshing motor connection...', 'info');
    await window.api.disconnectFromArduino();
    elements.connectArduino.disabled = false;
    elements.connectArduino.textContent = 'Connect Motor';
    elements.testMotor.disabled = true;
    updateConnectionStatus({ unity: unityConnected, arduino: false });
    setTimeout(() => connectToArduino(), 500);
}

// ==================== Recording Functions ====================

/**
 * Start CSV recording
 */
async function startRecording() {
    const filename = elements.filename.value.trim();
    
    if (!filename) {
        addLog('Please enter a filename', 'error');
        elements.filename.focus();
        return;
    }
    
    if (!unityConnected) {
        addLog('Unity must be connected to start recording', 'error');
        return;
    }
    
    const result = await window.api.startRecording(filename);
    
    if (result.success) {
        isRecording = true;
        elements.startRecording.disabled = true;
        elements.stopRecording.disabled = false;
        elements.filename.disabled = true;
        elements.recordingStatus.textContent = `🔴 Recording to ${result.filename}.csv`;
        elements.recordingStatus.classList.add('active');
        addLog(`Recording started: ${result.filename}.csv`, 'success');
    } else {
        addLog(`Failed to start recording: ${result.error}`, 'error');
    }
}

/**
 * Stop CSV recording
 */
async function stopRecording() {
    const result = await window.api.stopRecording();
    
    if (result.success) {
        isRecording = false;
        elements.startRecording.disabled = false;
        elements.stopRecording.disabled = true;
        elements.filename.disabled = false;
        elements.recordingStatus.textContent = '';
        elements.recordingStatus.classList.remove('active');
        addLog(`Recording saved: ${result.filename}.csv`, 'success');
        
        // --- ADD THIS LINE ---
        // Log the full path returned from the main process
        if (result.path) {
            addLog(`File saved in: ${result.path}`, 'info');
        } else {
            addLog(`File location: ExperimentalData/${result.filename}.csv`, 'info');
        }
        // --- END ADDITION ---
    } else {
        addLog(`Failed to stop recording: ${result.error}`, 'error');
    }
}

// ==================== UI Update Functions ====================

/**
 * Update connection status indicators
 * @param {Object} status - Status object with unity and arduino properties
 */
function updateConnectionStatus(status) {
    unityConnected = status.unity;
    arduinoConnected = status.arduino;
    
    // Update status indicators
    elements.unityStatus.className = `status-indicator ${status.unity ? 'connected' : 'disconnected'}`;
    elements.arduinoStatus.className = `status-indicator ${status.arduino ? 'connected' : 'disconnected'}`;
    
    // Update button states
    elements.startExperiment.disabled = !status.unity;
    elements.stopExperiment.disabled = !status.unity;
    elements.startRecording.disabled = !status.unity || isRecording;
    elements.testMotor.disabled = !status.arduino;
}

/**
 * Display incoming VR data
 * @param {Object} data - VR data object from Unity
 */
function displayData(data) {
    // Data is being received and will be saved to CSV
    // Visual indicator removed (no longer displaying live stream)
}

/**
 * Format number to 2 decimal places
 * @param {number} num - Number to format
 * @returns {string} Formatted number
 */
function formatNum(num) {
    return num !== undefined ? num.toFixed(2) : 'N/A';
}

/**
 * Add log entry to debug console
 * @param {string} message - Log message
 * @param {string} type - Log type (info, success, error, warning, arduino)
 */
function addLog(message, type = 'info') {
    const timestamp = new Date().toLocaleTimeString();
    const logEntry = document.createElement('div');
    logEntry.className = `log-entry log-${type}`;
    
    const timeSpan = document.createElement('span');
    timeSpan.className = 'log-time';
    timeSpan.textContent = timestamp;
    
    const messageSpan = document.createElement('span');
    messageSpan.textContent = message;
    
    logEntry.appendChild(timeSpan);
    logEntry.appendChild(messageSpan);
    
    elements.logOutput.appendChild(logEntry);
    elements.logOutput.scrollTop = elements.logOutput.scrollHeight;
}

/**
 * Clear debug console log
 */
function clearLog() {
    elements.logOutput.innerHTML = '';
    addLog('Log cleared', 'info');
}

/**
 * Show file rename modal
 * @param {Object} data - Rename notification data
 */
function showFileRenameModal(data) {
    elements.modalMessage.textContent = 
        `File "${data.original}.csv" already exists. Saving as "${data.renamed}.csv"`;
    elements.modal.style.display = 'flex';
    addLog(`File renamed to ${data.renamed}.csv (original exists)`, 'warning');
}

/**
 * Close modal
 */
function closeModal() {
    elements.modal.style.display = 'none';
}

// ==================== Keyboard Shortcuts ====================

/**
 * Handle keyboard shortcuts
 */
document.addEventListener('keydown', (event) => {
    // Ctrl+R or Cmd+R - Start/Stop Recording
    if ((event.ctrlKey || event.metaKey) && event.key === 'r') {
        event.preventDefault();
        if (!isRecording && unityConnected && elements.filename.value.trim()) {
            startRecording();
        } else if (isRecording) {
            stopRecording();
        }
    }
    
    // Ctrl+L or Cmd+L - Clear Log
    if ((event.ctrlKey || event.metaKey) && event.key === 'l') {
        event.preventDefault();
        clearLog();
    }
});
