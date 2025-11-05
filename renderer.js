/**
 * AIMLAB VR Data Collector - Renderer Process
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 05 November 2025
 * Version: v3.5
 * 
 * Description:
 * Renderer process JavaScript for AIMLAB VR Data Collector.
 * Handles UI interactions for Unity UDP connection, Arduino serial
 * communication, and experiment data folder access.
 * 
 * Changelog:
 * v3.5 - 05 November 2025 - Pass experiment ID to Unity
 *        - Added validation that experiment ID must end with a number
 *        - Experiment ID now sent to Unity in command
 *        - Unity receives ID for internal experiment tracking
 * v3.4 - 05 November 2025 - Added left/right hand experiment support
 *        - Split Start Experiment into two separate buttons
 *        - Filenames prefixed with LEFT_ or RIGHT_
 *        - Separate Unity commands for each hand
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
    startLeftExperiment: document.getElementById('startLeftExperiment'),
    startRightExperiment: document.getElementById('startRightExperiment'),
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
    // Show splash screen for 3 seconds (0.3s delay + 2s logo animation + 0.7s hold)
    setTimeout(() => {
        // Start fade out of splash screen
        const splashScreen = document.getElementById('splashScreen');
        const mainContent = document.getElementById('mainContent');
        
        splashScreen.classList.add('fade-out');
        
        // After splash fades out, show main content and remove splash
        setTimeout(() => {
            mainContent.classList.add('visible');
            // Remove splash screen from DOM after animation completes
            setTimeout(() => {
                splashScreen.remove();
            }, 800);
        }, 100);
        
        // Initialize app after splash starts fading
        setupEventListeners();
        addLog('AIMLAB VR Data Collector initialized', 'success');
        addLog('Ready to connect to Unity and Vibration Motor', 'info');
        addLog('Discovery port: 45001 | Data port: 45102', 'info');
    }, 3000);
});

// ==================== Event Listeners Setup ====================

/**
 * Setup all event listeners for buttons and IPC
 */
function setupEventListeners() {
    // Unity Controls
    elements.connectUnity.addEventListener('click', connectToUnity);
    elements.refreshUnity.addEventListener('click', refreshUnityConnection);
    elements.startLeftExperiment.addEventListener('click', () => startExperiment('left'));
    elements.startRightExperiment.addEventListener('click', () => startExperiment('right'));
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
 * @param {string} hand - Which hand to use ('left' or 'right')
 */
async function startExperiment(hand = 'right') {
    const experimentId = elements.experimentId.value.trim();
    
    // Validate Experiment ID
    if (!experimentId) {
        addLog('Please enter an Experiment ID', 'error');
        elements.experimentId.focus();
        return;
    }
    
    // Validate that experiment ID ends with a number
    if (!/\d$/.test(experimentId)) {
        addLog('Experiment ID must end with a number', 'error');
        elements.experimentId.focus();
        return;
    }
    
    if (!unityConnected) {
        addLog('Unity must be connected to start experiment', 'error');
        return;
    }
    
    // Modify filename based on hand
    const modifiedId = `${hand.toUpperCase()}_${experimentId}`;
    
    // Check if file already exists
    const checkResult = await window.api.checkFileExists(modifiedId);
    if (checkResult.exists) {
        showFileExistsModal(modifiedId);
        return;
    }
    
    // Start recording first
    addLog(`Starting ${hand} hand recording with ID: ${modifiedId}...`, 'info');
    const recordResult = await window.api.startRecording(modifiedId);
    
    if (!recordResult.success) {
        addLog(`Failed to start recording: ${recordResult.error}`, 'error');
        return;
    }
    
    isRecording = true;
    elements.recordingStatus.textContent = `🔴 Recording: ${recordResult.filename}.csv`;
    elements.recordingStatus.classList.add('active');
    addLog(`Recording started: ${recordResult.filename}.csv`, 'success');
    
    // Send appropriate command based on hand WITH experiment ID
    const command = hand === 'left' ? 'startLeftExperiment' : 'startRightExperiment';
    addLog(`Sending Start ${hand.charAt(0).toUpperCase() + hand.slice(1)} Hand Experiment command to Unity with ID: ${experimentId}...`, 'info');
    const result = await window.api[command](experimentId);  // Pass the experiment ID
    
    if (result.success) {
        elements.startLeftExperiment.disabled = true;
        elements.startRightExperiment.disabled = true;
        elements.stopExperiment.disabled = false;
        elements.experimentId.disabled = true;
        addLog(`${hand.charAt(0).toUpperCase() + hand.slice(1)} hand experiment started in Unity with ID: ${experimentId}`, 'success');
        addLog('Unity should now be sending VR data', 'info');
    } else {
        addLog(`Failed to start ${hand} hand experiment: ${result.error}`, 'error');
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
    
    elements.startLeftExperiment.disabled = false;
    elements.startRightExperiment.disabled = false;
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
    elements.startLeftExperiment.disabled = !status.unity;
    elements.startRightExperiment.disabled = !status.unity;
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
