/**
 * AIMLAB VR Data Collector - Renderer Process
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 05 November 2025
 * Version: v3.3
 * 
 * Description:
 * Renderer process JavaScript for AIMLAB VR Data Collector.
 * Handles UI interactions for Unity UDP connection, Arduino serial
 * communication, and experiment data folder access.
 * 
 * Changelog:
 * v3.3 - 05 November 2025 - Integrated recording with experiments
 *        - Added Experiment ID textbox for file naming
 *        - Start Experiment automatically starts recording
 *        - Stop Experiment automatically stops recording
 *        - Added file existence validation before starting
 * v3.2 - 05 November 2025 - Removed recording buttons
 *        - Replaced Start/Stop Recording buttons with Open Experiment Data Folder button
 *        - Removed recording state management
 *        - Added folder opening functionality
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
    openDataFolder: document.getElementById('openDataFolder'),
    clearLog: document.getElementById('clearLog'),
    
    // Inputs
    experimentId: document.getElementById('experimentId'),
    
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
    
    // Data Folder
    elements.openDataFolder.addEventListener('click', openDataFolder);
    
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
 * Start experiment in Unity and begin recording
 */
async function startExperiment() {
    const experimentId = elements.experimentId.value.trim();
    
    // Validate Experiment ID
    if (!experimentId) {
        addLog('Please enter an Experiment ID', 'error');
        elements.experimentId.focus();
        return;
    }
    
    if (!unityConnected) {
        addLog('Unity must be connected to start experiment', 'error');
        return;
    }
    
    // Check if file already exists
    const checkResult = await window.api.checkFileExists(experimentId);
    if (checkResult.exists) {
        // Show modal alert
        showFileExistsModal(experimentId);
        return;
    }
    
    // Start recording first
    addLog(`Starting recording with Experiment ID: ${experimentId}...`, 'info');
    const recordResult = await window.api.startRecording(experimentId);
    
    if (!recordResult.success) {
        addLog(`Failed to start recording: ${recordResult.error}`, 'error');
        return;
    }
    
    isRecording = true;
    elements.recordingStatus.textContent = `🔴 Recording: ${recordResult.filename}.csv`;
    elements.recordingStatus.classList.add('active');
    addLog(`Recording started: ${recordResult.filename}.csv`, 'success');
    
    // Then send Start Experiment command to Unity
    addLog('Sending Start Experiment command to Unity...', 'info');
    const result = await window.api.startExperiment();
    
    if (result.success) {
        elements.startExperiment.disabled = true;
        elements.stopExperiment.disabled = false;
        elements.experimentId.disabled = true;
        addLog('Experiment started in Unity', 'success');
        addLog('Unity should now be sending VR data', 'info');
    } else {
        addLog(`Failed to start experiment: ${result.error}`, 'error');
        // Stop recording if experiment failed to start
        await stopRecordingOnly();
    }
}

/**
 * Stop experiment in Unity and stop recording
 */
async function stopExperiment() {
    // Send Stop Experiment command to Unity first
    addLog('Sending Stop Experiment command to Unity...', 'info');
    const result = await window.api.stopExperiment();
    
    if (result.success) {
        addLog('Experiment stopped in Unity', 'success');
    } else {
        addLog(`Failed to stop experiment: ${result.error}`, 'error');
    }
    
    // Stop recording regardless of Unity result
    await stopRecordingOnly();
    
    elements.startExperiment.disabled = false;
    elements.stopExperiment.disabled = true;
    elements.experimentId.disabled = false;
}

/**
 * Internal function to stop recording without UI side effects
 */
async function stopRecordingOnly() {
    if (!isRecording) return;
    
    const result = await window.api.stopRecording();
    
    if (result.success) {
        isRecording = false;
        elements.recordingStatus.textContent = '';
        elements.recordingStatus.classList.remove('active');
        addLog(`Recording saved: ${result.filename}.csv`, 'success');
        addLog(`File location: ExperimentalData/${result.filename}.csv`, 'info');
    } else {
        addLog(`Failed to stop recording: ${result.error}`, 'error');
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

// ==================== Data Folder Functions ====================

/**
 * Open Experiment Data Folder in Windows Explorer
 */
async function openDataFolder() {
    addLog('Opening Experiment Data folder...', 'info');
    const result = await window.api.openDataFolder();
    
    if (result.success) {
        addLog(`Opened folder: ${result.path}`, 'success');
    } else {
        addLog(`Failed to open folder: ${result.error}`, 'error');
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
 * Show file exists modal alert
 * @param {string} experimentId - The experiment ID that already exists
 */
function showFileExistsModal(experimentId) {
    elements.modalMessage.textContent = 
        `A file with Experiment ID "${experimentId}.csv" already exists. Please use a different Experiment ID.`;
    elements.modal.style.display = 'flex';
    addLog(`Cannot start: File ${experimentId}.csv already exists`, 'error');
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
    // Ctrl+O or Cmd+O - Open Data Folder
    if ((event.ctrlKey || event.metaKey) && event.key === 'o') {
        event.preventDefault();
        openDataFolder();
    }
    
    // Ctrl+L or Cmd+L - Clear Log
    if ((event.ctrlKey || event.metaKey) && event.key === 'l') {
        event.preventDefault();
        clearLog();
    }
});
