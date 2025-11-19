/**
 * AIMLAB VR Data Collector - Renderer Process
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 19 November 2025
 * Version: v3.9
 * 
 * Description:
 * Renderer process JavaScript for AIMLAB VR Data Collector.
 * Handles UI interactions for Unity UDP connection, Arduino serial
 * communication, and experiment data folder access.
 * 
 * Changelog:
 * v3.9 - 19 November 2025 - Added mid-experiment save feature
 *        - Added saveMidExperiment function and button
 *        - Button enabled during experiment, disabled when stopped
 *        - Auto-syncs data 4 seconds after mid-save
 *        - Auto-syncs data 4 seconds after stop experiment
 *        - Provides visual feedback for save operations
 * v3.8 - 19 November 2025 - Added ADB sync button and handler with path configuration
 *        - Added syncExperimentData function
 *        - Button triggers ADB pull from Android headset
 *        - Displays sync status in log output
 *        - Added setAdbPath function and button
 *        - User can configure ADB path via file dialog
 * v3.7 - 05 November 2025 - Chunked TSV transfer support
 *        - Added TSV progress tracking and display
 *        - Added success notification with transfer details
 *        - Shows transfer time and file size
 *        - Progress updates every 10 chunks
 * v3.6 - 05 November 2025 - TSV file handling and validation
 *        - Removed CSV creation on experiment start
 *        - Added validateExperimentId function with file checking
 *        - Added TSV saved notification listener
 *        - Experiment ID validated on input change/blur
 *        - Shows modal for existing files
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
    saveMidExperiment: document.getElementById('saveMidExperiment'),
    syncExperimentData: document.getElementById('syncExperimentData'),
    setAdbPath: document.getElementById('setAdbPath'),
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
    elements.saveMidExperiment.addEventListener('click', saveMidExperiment);
    elements.syncExperimentData.addEventListener('click', syncExperimentData);
    elements.setAdbPath.addEventListener('click', setAdbPath);
    
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
    
    // Listen for TSV saved confirmation
    window.api.onTSVSaved((event, data) => {
        const transferTime = data.transferTime || 'N/A';
        const fileSize = data.fileSize ? ` (${(data.fileSize / 1024).toFixed(2)} KB)` : '';
        addLog(`✓ TSV file saved: ${data.fileName} (Transfer time: ${transferTime}s)${fileSize}`, 'success');
        addLog(`File location: ${data.path}`, 'info');
        
        // Show success notification
        showSuccessNotification(data.fileName, transferTime, fileSize);
    });
    
    // Listen for TSV transfer progress
    window.api.onTSVProgress((event, data) => {
        addLog(`TSV transfer: ${data.fileName} - ${data.progress}% (${data.received}/${data.total} chunks)`, 'info');
    });
    
    // Add experiment ID validation on input change
    elements.experimentId.addEventListener('change', async () => {
        await validateExperimentId();
    });
    
    elements.experimentId.addEventListener('blur', async () => {
        await validateExperimentId();
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
 * Validate experiment ID and check for existing files
 */
async function validateExperimentId() {
    const experimentId = elements.experimentId.value.trim();
    
    if (!experimentId) {
        return;
    }
    
    // Validate that experiment ID ends with a number
    if (!/\d$/.test(experimentId)) {
        showModal('Invalid Experiment ID', 'Experiment ID must end with a number.');
        elements.experimentId.value = '';
        elements.startLeftExperiment.disabled = true;
        elements.startRightExperiment.disabled = true;
        return;
    }
    
    // Check for existing files
    try {
        const existingFiles = await window.api.checkExperimentFiles(experimentId);
        
        if (existingFiles.length > 0) {
            const fileList = existingFiles.join('\n');
            showModal(
                'Experiment ID Already Exists',
                `The following files already exist for experiment ID "${experimentId}":\n\n${fileList}\n\nPlease use a different experiment ID.`
            );
            elements.experimentId.value = '';
            elements.startLeftExperiment.disabled = true;
            elements.startRightExperiment.disabled = true;
        } else {
            // Enable start buttons if Unity is connected
            if (unityConnected) {
                elements.startLeftExperiment.disabled = false;
                elements.startRightExperiment.disabled = false;
            }
        }
    } catch (error) {
        addLog(`Error checking experiment files: ${error.message}`, 'error');
    }
}

/**
 * Start experiment in Unity (no CSV creation - wait for Unity TSV)
 * @param {string} hand - Which hand to use ('left' or 'right')
 */
async function startExperiment(hand = 'right') {
    const experimentId = elements.experimentId.value.trim();
    
    // Validate Experiment ID
    if (!experimentId) {
        showModal('Missing Experiment ID', 'Please enter an Experiment ID before starting the experiment.');
        elements.experimentId.focus();
        return;
    }
    
    // Validate that experiment ID ends with a number
    if (!/\d$/.test(experimentId)) {
        showModal('Invalid Experiment ID', 'Experiment ID must end with a number.');
        elements.experimentId.focus();
        return;
    }
    
    if (!unityConnected) {
        addLog('Unity must be connected to start experiment', 'error');
        return;
    }
    
    // DO NOT create CSV file here - wait for Unity to send TSV
    // Just send command to Unity
    const command = hand === 'left' ? 'startLeftExperiment' : 'startRightExperiment';
    addLog(`Sending Start ${hand.charAt(0).toUpperCase() + hand.slice(1)} Hand Experiment command to Unity with ID: ${experimentId}...`, 'info');
    const result = await window.api[command](experimentId);
    
    if (result.success) {
        elements.startLeftExperiment.disabled = true;
        elements.startRightExperiment.disabled = true;
        elements.stopExperiment.disabled = false;
        elements.saveMidExperiment.disabled = false; // Enable mid-save button
        elements.experimentId.disabled = true;
        
        isRecording = true;
        elements.recordingStatus.textContent = `🔴 Recording ${hand.toUpperCase()} hand: ${experimentId}`;
        elements.recordingStatus.classList.add('active');
        
        addLog(`${hand.charAt(0).toUpperCase() + hand.slice(1)} hand experiment started in Unity with ID: ${experimentId}`, 'success');
        addLog('Waiting for Unity to send TSV data...', 'info');
    } else {
        addLog(`Failed to start ${hand} hand experiment: ${result.error}`, 'error');
    }
}

/**
 * Stop experiment in Unity
 */
async function stopExperiment() {
    // Send Stop Experiment command to Unity
    addLog('Sending Stop Experiment command to Unity...', 'info');
    const result = await window.api.stopExperiment();
    
    if (result.success) {
        addLog('Experiment stopped in Unity', 'success');
        addLog('Unity will save TSV file and send it to Electron', 'info');
        
        // Auto-sync after 4 seconds
        setTimeout(async () => {
            addLog('Auto-syncing experiment data...', 'info');
            const syncResult = await window.api.syncExperimentData();
            if (syncResult.success) {
                addLog(`Data synced: ${syncResult.path}`, 'success');
            } else {
                addLog(`Sync failed: ${syncResult.error}`, 'error');
            }
        }, 4000);
    } else {
        addLog(`Failed to stop experiment: ${result.error}`, 'error');
    }
    
    // Update UI
    isRecording = false;
    elements.startLeftExperiment.disabled = false;
    elements.startRightExperiment.disabled = false;
    elements.stopExperiment.disabled = true;
    elements.saveMidExperiment.disabled = true; // Disable mid-save button
    elements.experimentId.disabled = false;
    elements.recordingStatus.textContent = '';
    elements.recordingStatus.classList.remove('active');
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

/**
 * Sync experiment data from headset to ExperimentalData via ADB
 */
async function syncExperimentData() {
    addLog('Syncing experiment data from headset via ADB...', 'info');
    const result = await window.api.syncExperimentData();

    if (result.success) {
        addLog(`Experiment data synced to: ${result.path}`, 'success');
    } else {
        addLog(`Experiment data sync failed: ${result.error}`, 'error');
    }
}

/**
 * Set ADB path by opening file dialog
 */
async function setAdbPath() {
    addLog("Selecting adb.exe...", "info");

    const result = await window.api.setAdbPath();

    if (result.success) {
        addLog(`ADB path saved: ${result.adbPath}`, "success");
    } else {
        addLog(`ADB path not changed: ${result.error}`, "error");
    }
}

/**
 * Save data mid-experiment without stopping
 */
async function saveMidExperiment() {
    if (!unityConnected) {
        addLog('Unity not connected', 'error');
        return;
    }
    
    addLog('Requesting mid-experiment save...', 'info');
    
    try {
        // Send save command to Unity
        const result = await window.api.saveMidExperiment();
        
        if (result.success) {
            addLog('Mid-experiment save initiated', 'success');
            
            // Wait 4 seconds then sync
            setTimeout(async () => {
                addLog('Auto-syncing experiment data...', 'info');
                const syncResult = await window.api.syncExperimentData();
                if (syncResult.success) {
                    addLog(`Data synced: ${syncResult.path}`, 'success');
                } else {
                    addLog(`Sync failed: ${syncResult.error}`, 'error');
                }
            }, 4000);
        } else {
            addLog(`Mid-save failed: ${result.error}`, 'error');
        }
    } catch (error) {
        addLog(`Mid-save error: ${error.message}`, 'error');
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
    showModal(
        'File Already Exists',
        `A file with Experiment ID "${experimentId}.csv" already exists. Please use a different Experiment ID.`
    );
    addLog(`Cannot start: File ${experimentId}.csv already exists`, 'error');
}

/**
 * Show modal with title and message
 * @param {string} title - Modal title
 * @param {string} message - Modal message
 */
function showModal(title, message) {
    const modalTitle = elements.modal.querySelector('h3');
    if (modalTitle) {
        modalTitle.textContent = title;
    }
    elements.modalMessage.textContent = message;
    elements.modal.style.display = 'flex';
}

/**
 * Close modal
 */
function closeModal() {
    elements.modal.style.display = 'none';
}

/**
 * Show success notification for TSV file save
 * @param {string} fileName - Name of saved file
 * @param {string} transferTime - Transfer time in seconds
 * @param {string} fileSize - File size information
 */
function showSuccessNotification(fileName, transferTime, fileSize) {
    const notification = document.createElement('div');
    notification.className = 'success-notification';
    notification.innerHTML = `
        <strong>Data Saved Successfully</strong><br>
        File: ${fileName}<br>
        Transfer time: ${transferTime}s${fileSize}
    `;
    document.body.appendChild(notification);
    
    // Remove after 5 seconds
    setTimeout(() => {
        notification.style.opacity = '0';
        notification.style.transform = 'translateX(100%)';
        setTimeout(() => {
            notification.remove();
        }, 300);
    }, 5000);
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
