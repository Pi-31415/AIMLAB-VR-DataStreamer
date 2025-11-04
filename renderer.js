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
    dataStream: document.getElementById('dataStream'),
    recordingStatus: document.getElementById('recordingStatus')
};

// ==================== Initialization ====================

/**
 * Initialize application when DOM is loaded
 */
document.addEventListener('DOMContentLoaded', () => {
    setupEventListeners();
    addLog('System initialized', 'success');
    addLog('Ready to connect to Unity and Arduino', 'info');
    addLog('Listening on UDP port 45101 for Unity VR data', 'info');
});

// ==================== Event Listeners Setup ====================

/**
 * Setup all event listeners for buttons and IPC
 */
function setupEventListeners() {
    // Unity Controls
    elements.connectUnity.addEventListener('click', connectToUnity);
    elements.refreshUnity.addEventListener('click', refreshUnityConnection);
    
    // Arduino Controls
    elements.connectArduino.addEventListener('click', connectToArduino);
    elements.testMotor.addEventListener('click', testMotor);
    elements.refreshArduino.addEventListener('click', refreshArduinoConnection);
    
    // Recording Controls
    elements.startRecording.addEventListener('click', startRecording);
    elements.stopRecording.addEventListener('click', stopRecording);
    
    // Utility
    elements.clearLog.addEventListener('click', clearLog);
    
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

// ==================== Arduino Connection Functions ====================

/**
 * Connect to Arduino vibration motor controller
 */
async function connectToArduino() {
    elements.connectArduino.disabled = true;
    addLog('Scanning for Arduino...', 'info');
    
    const result = await window.api.connectToArduino();
    
    if (result.success) {
        elements.connectArduino.textContent = 'Connected';
        elements.testMotor.disabled = false;
        addLog(`Arduino connected on ${result.port}`, 'success');
        addLog('Waiting for Arduino handshake...', 'info');
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
 * Refresh Arduino connection
 */
async function refreshArduinoConnection() {
    addLog('Refreshing Arduino connection...', 'info');
    await window.api.disconnectFromArduino();
    elements.connectArduino.disabled = false;
    elements.connectArduino.textContent = 'Connect Arduino';
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
        addLog(`File location: data/${result.filename}.csv`, 'info');
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
    elements.startRecording.disabled = !status.unity || isRecording;
    elements.testMotor.disabled = !status.arduino;
}

/**
 * Display incoming VR data
 * @param {Object} data - VR data object from Unity
 */
function displayData(data) {
    const display = 
        `Position: [${formatNum(data.positionX)}, ${formatNum(data.positionY)}, ${formatNum(data.positionZ)}]\n` +
        `Rotation: [${formatNum(data.rotationX)}, ${formatNum(data.rotationY)}, ${formatNum(data.rotationZ)}, ${formatNum(data.rotationW)}]\n` +
        `Trigger: ${formatNum(data.triggerValue)} | Grip: ${formatNum(data.gripValue)}\n` +
        `Primary: ${data.primaryButton ? 'ON' : 'OFF'} | Secondary: ${data.secondaryButton ? 'ON' : 'OFF'}\n` +
        `Timestamp: ${data.timestamp}`;
    
    elements.dataStream.textContent = display;
    
    // Add a visual indicator that data is flowing
    elements.dataStream.style.borderLeft = '3px solid #4CAF50';
    setTimeout(() => {
        elements.dataStream.style.borderLeft = 'none';
    }, 100);
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
