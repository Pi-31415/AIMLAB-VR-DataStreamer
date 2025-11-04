/**
 * @file AIMLABVRDataStreamer_Fixed.cs
 * @brief AIMLAB-VR-Data Streamer - Unity Component (Fixed for InputDevice ambiguity)
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 2025
 * 
 * Unity C# component that communicates with the C++ data streamer via UDP.
 * Uses TextMeshPro for UI and Unity's new Input System for VR input.
 * 
 * This version fixes the ambiguous reference between:
 * - UnityEngine.InputSystem.InputDevice
 * - UnityEngine.XR.InputDevice
 * 
 * Requirements:
 * - TextMeshPro package
 * - Input System package
 * - XR Plugin Management (for VR)
 * 
 * Usage: Attach this script to a GameObject in your Unity scene.
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using UnityEngine.UI;
using TMPro;  // TextMeshPro
using UnityEngine.InputSystem;  // New Input System

// Use explicit namespace for XR to avoid ambiguity
using XRInputDevice = UnityEngine.XR.InputDevice;
using XRInputDeviceCharacteristics = UnityEngine.XR.InputDeviceCharacteristics;
using XRInputDevices = UnityEngine.XR.InputDevices;
using XRCommonUsages = UnityEngine.XR.CommonUsages;

namespace AIMLAB.VR
{
    /// <summary>
    /// Main data streamer component for Unity VR applications (Fixed Version)
    /// </summary>
    public class AIMLABVRDataStreamer : MonoBehaviour
    {
        #region Constants

        // Network configuration
        private const int DISCOVERY_BASE_PORT = 45000;
        private const int DATA_PORT = 45101;  // Different from C++ node
        private const int BUFFER_SIZE = 4096;
        private const string NODE_ID = "AIMLAB_UNITY";
        private const string PEER_ID = "AIMLAB_CPP";

        // Protocol messages
        private const string MSG_DISCOVER = "DISCOVER";
        private const string MSG_ACKNOWLEDGE = "ACK";
        private const string MSG_HANDSHAKE = "HANDSHAKE";
        private const string MSG_READY = "READY";
        private const string MSG_COMMAND = "CMD";
        private const string MSG_DATA = "DATA";
        private const string MSG_STATUS = "STATUS";
        private const string MSG_KEEPALIVE = "KEEPALIVE";

        // Commands
        private const string CMD_OPEN_FILE = "OPEN_FILE";
        private const string CMD_CLOSE_FILE = "CLOSE_FILE";
        private const string CMD_STATUS = "GET_STATUS";
        private const string CMD_SHUTDOWN = "SHUTDOWN";

        #endregion

        #region Public Properties

        [Header("Connection Settings")]
        [Tooltip("Auto-start discovery on awake")]
        public bool autoConnect = true;

        [Tooltip("Maximum discovery attempts")]
        public int maxDiscoveryAttempts = 30;

        [Tooltip("Discovery broadcast interval (seconds)")]
        public float discoveryInterval = 2.0f;

        [Header("Data Settings")]
        [Tooltip("Send data at fixed intervals")]
        public bool enablePeriodicData = false;

        [Tooltip("Data send interval (seconds)")]
        public float dataSendInterval = 0.1f;

        [Header("UI References - TextMeshPro")]
        public TextMeshProUGUI statusText;
        public TMP_InputField filenameInput;
        public Button openFileButton;
        public Button closeFileButton;
        public TextMeshProUGUI dataCountText;
        public Toggle autoDataToggle;
        public TextMeshProUGUI connectionInfoText;

        [Header("Input System Actions")]
        [Tooltip("Input action for opening file (e.g., button press)")]
        public InputActionReference openFileAction;

        [Tooltip("Input action for closing file")]
        public InputActionReference closeFileAction;

        [Tooltip("Input action for toggling data recording")]
        public InputActionReference toggleRecordingAction;

        [Header("XR Controller References")]
        [Tooltip("Track left controller")]
        public bool trackLeftController = true;

        [Tooltip("Track right controller")]
        public bool trackRightController = true;

        [Tooltip("Track HMD/Headset")]
        public bool trackHeadset = true;

        [Header("Debug")]
        public bool debugMode = false;
        public TextMeshProUGUI debugText;

        #endregion

        #region Private Fields

        // Network state
        private bool isConnected = false;
        private bool isDiscovering = false;
        private bool fileOpen = false;
        private string currentFilename = "";
        private int dataCount = 0;

        // Sockets
        private UdpClient discoverySocket;
        private UdpClient dataSocket;
        private Thread receiveThread;
        private Thread discoveryThread;

        // Peer information
        private IPEndPoint peerEndpoint;
        private string peerAddress = "";
        private int peerPort = 0;
        private int actualDiscoveryPort = DISCOVERY_BASE_PORT;

        // Thread control
        private volatile bool threadRunning = false;
        private readonly object lockObject = new object();

        // Message queue for main thread
        private Queue<Action> mainThreadQueue = new Queue<Action>();

        // Keepalive
        private float lastKeepalive = 0;
        private float keepaliveInterval = 5.0f;

        // XR Devices - using the XR-specific type
        private List<XRInputDevice> xrDevices = new List<XRInputDevice>();

        #endregion

        #region Unity Lifecycle

        void Awake()
        {
            // Setup input actions if assigned
            SetupInputActions();

            if (autoConnect)
            {
                StartConnection();
            }
        }

        void Start()
        {
            SetupUI();
            InvokeRepeating(nameof(UpdateStatus), 0.5f, 0.5f);

            // Initialize XR device tracking
            if (trackHeadset || trackLeftController || trackRightController)
            {
                XRInputDevices.deviceConnected += OnDeviceConnected;
                XRInputDevices.deviceDisconnected += OnDeviceDisconnected;
                RefreshXRDevices();
            }
        }

        void Update()
        {
            // Process main thread queue
            lock (mainThreadQueue)
            {
                while (mainThreadQueue.Count > 0)
                {
                    mainThreadQueue.Dequeue()?.Invoke();
                }
            }

            // Send keepalive
            if (isConnected && Time.time - lastKeepalive > keepaliveInterval)
            {
                SendKeepalive();
                lastKeepalive = Time.time;
            }

            // Send periodic data if enabled
            if (isConnected && fileOpen && enablePeriodicData)
            {
                SendPeriodicData();
            }
        }

        void OnDestroy()
        {
            StopConnection();

            // Cleanup input actions
            if (openFileAction != null) openFileAction.action.performed -= OnOpenFileAction;
            if (closeFileAction != null) closeFileAction.action.performed -= OnCloseFileAction;
            if (toggleRecordingAction != null) toggleRecordingAction.action.performed -= OnToggleRecordingAction;

            // Cleanup XR
            XRInputDevices.deviceConnected -= OnDeviceConnected;
            XRInputDevices.deviceDisconnected -= OnDeviceDisconnected;
        }

        void OnApplicationPause(bool pauseStatus)
        {
            if (pauseStatus)
            {
                if (fileOpen)
                {
                    SendCommand(CMD_CLOSE_FILE);
                }
            }
        }

        void OnApplicationQuit()
        {
            StopConnection();
        }

        #endregion

        #region Input System Setup

        private void SetupInputActions()
        {
            // Enable and subscribe to input actions
            if (openFileAction != null)
            {
                openFileAction.action.Enable();
                openFileAction.action.performed += OnOpenFileAction;
            }

            if (closeFileAction != null)
            {
                closeFileAction.action.Enable();
                closeFileAction.action.performed += OnCloseFileAction;
            }

            if (toggleRecordingAction != null)
            {
                toggleRecordingAction.action.Enable();
                toggleRecordingAction.action.performed += OnToggleRecordingAction;
            }
        }

        private void OnOpenFileAction(InputAction.CallbackContext context)
        {
            if (isConnected && !fileOpen)
            {
                string filename = filenameInput != null ? filenameInput.text : "";
                OpenFile(filename);
            }
        }

        private void OnCloseFileAction(InputAction.CallbackContext context)
        {
            if (isConnected && fileOpen)
            {
                CloseFile();
            }
        }

        private void OnToggleRecordingAction(InputAction.CallbackContext context)
        {
            if (isConnected)
            {
                if (fileOpen)
                {
                    CloseFile();
                }
                else
                {
                    OpenFile();
                }
            }
        }

        #endregion

        #region XR Device Tracking

        private void RefreshXRDevices()
        {
            xrDevices.Clear();
            XRInputDevices.GetDevices(xrDevices);

            foreach (var device in xrDevices)
            {
                LogMessage($"XR Device found: {device.name} ({device.characteristics})", LogLevel.Debug);
            }
        }

        private void OnDeviceConnected(XRInputDevice device)
        {
            RefreshXRDevices();
            LogMessage($"XR Device connected: {device.name}", LogLevel.Info);
        }

        private void OnDeviceDisconnected(XRInputDevice device)
        {
            RefreshXRDevices();
            LogMessage($"XR Device disconnected: {device.name}", LogLevel.Info);
        }

        private void SendXRDeviceData()
        {
            foreach (var device in xrDevices)
            {
                // Check device type
                bool isHMD = device.characteristics.HasFlag(XRInputDeviceCharacteristics.HeadMounted);
                bool isLeftController = device.characteristics.HasFlag(XRInputDeviceCharacteristics.Left) &&
                                       device.characteristics.HasFlag(XRInputDeviceCharacteristics.Controller);
                bool isRightController = device.characteristics.HasFlag(XRInputDeviceCharacteristics.Right) &&
                                        device.characteristics.HasFlag(XRInputDeviceCharacteristics.Controller);

                // Skip if not tracking this device type
                if (isHMD && !trackHeadset) continue;
                if (isLeftController && !trackLeftController) continue;
                if (isRightController && !trackRightController) continue;

                // Get device name for labeling
                string deviceLabel = isHMD ? "HMD" :
                                    isLeftController ? "LeftController" :
                                    isRightController ? "RightController" :
                                    device.name.Replace(" ", "_");

                // Send position if available
                if (device.TryGetFeatureValue(XRCommonUsages.devicePosition, out Vector3 position))
                {
                    SendVector3($"{deviceLabel}_Position", position);
                }

                // Send rotation if available
                if (device.TryGetFeatureValue(XRCommonUsages.deviceRotation, out Quaternion rotation))
                {
                    SendQuaternion($"{deviceLabel}_Rotation", rotation);
                }

                // Send velocity if available
                if (device.TryGetFeatureValue(XRCommonUsages.deviceVelocity, out Vector3 velocity))
                {
                    SendVector3($"{deviceLabel}_Velocity", velocity);
                }

                // Send button states for controllers
                if (isLeftController || isRightController)
                {
                    // Trigger
                    if (device.TryGetFeatureValue(XRCommonUsages.trigger, out float triggerValue))
                    {
                        SendData($"{deviceLabel}_Trigger", triggerValue.ToString("F3"));
                    }

                    // Grip
                    if (device.TryGetFeatureValue(XRCommonUsages.grip, out float gripValue))
                    {
                        SendData($"{deviceLabel}_Grip", gripValue.ToString("F3"));
                    }

                    // Primary button (A/X)
                    if (device.TryGetFeatureValue(XRCommonUsages.primaryButton, out bool primaryButton))
                    {
                        SendData($"{deviceLabel}_PrimaryButton", primaryButton.ToString());
                    }

                    // Secondary button (B/Y)
                    if (device.TryGetFeatureValue(XRCommonUsages.secondaryButton, out bool secondaryButton))
                    {
                        SendData($"{deviceLabel}_SecondaryButton", secondaryButton.ToString());
                    }

                    // Thumbstick
                    if (device.TryGetFeatureValue(XRCommonUsages.primary2DAxis, out Vector2 thumbstick))
                    {
                        SendData($"{deviceLabel}_Thumbstick", $"{thumbstick.x:F3},{thumbstick.y:F3}");
                    }
                }
            }
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Start connection to C++ node
        /// </summary>
        public void StartConnection()
        {
            if (isDiscovering || isConnected)
            {
                LogMessage("Already connected or discovering", LogLevel.Warning);
                return;
            }

            StartCoroutine(DiscoveryCoroutine());
        }

        /// <summary>
        /// Stop connection and cleanup
        /// </summary>
        public void StopConnection()
        {
            threadRunning = false;

            if (fileOpen)
            {
                SendCommand(CMD_CLOSE_FILE);
            }

            if (isConnected)
            {
                SendCommand(CMD_SHUTDOWN);
            }

            isConnected = false;
            isDiscovering = false;

            try
            {
                receiveThread?.Join(1000);
                discoveryThread?.Join(1000);

                discoverySocket?.Close();
                dataSocket?.Close();
            }
            catch (Exception e)
            {
                LogMessage($"Error stopping connection: {e.Message}", LogLevel.Error);
            }
        }

        /// <summary>
        /// Open a new data file
        /// </summary>
        /// <param name="filename">Optional filename (auto-generates if empty)</param>
        public void OpenFile(string filename = "")
        {
            if (!isConnected)
            {
                LogMessage("Not connected to C++ node", LogLevel.Warning);
                return;
            }

            if (fileOpen)
            {
                LogMessage("File already open", LogLevel.Warning);
                return;
            }

            string command = string.IsNullOrEmpty(filename)
                ? CMD_OPEN_FILE
                : $"{CMD_OPEN_FILE}:{filename}";

            SendCommand(command);
        }

        /// <summary>
        /// Close current data file
        /// </summary>
        public void CloseFile()
        {
            if (!isConnected)
            {
                LogMessage("Not connected to C++ node", LogLevel.Warning);
                return;
            }

            if (!fileOpen)
            {
                LogMessage("No file open", LogLevel.Warning);
                return;
            }

            SendCommand(CMD_CLOSE_FILE);
        }

        /// <summary>
        /// Send data to C++ node
        /// </summary>
        /// <param name="dataType">Type/category of data</param>
        /// <param name="dataValue">Data value as string</param>
        public void SendData(string dataType, string dataValue)
        {
            if (!isConnected)
            {
                return;
            }

            string message = $"{MSG_DATA}:{dataType}:{dataValue}";
            SendMessage(message);

            dataCount++;
        }

        /// <summary>
        /// Send Vector3 data
        /// </summary>
        public void SendVector3(string label, Vector3 vector)
        {
            string data = $"{vector.x:F3},{vector.y:F3},{vector.z:F3}";
            SendData(label, data);
        }

        /// <summary>
        /// Send Vector2 data
        /// </summary>
        public void SendVector2(string label, Vector2 vector)
        {
            string data = $"{vector.x:F3},{vector.y:F3}";
            SendData(label, data);
        }

        /// <summary>
        /// Send Quaternion data
        /// </summary>
        public void SendQuaternion(string label, Quaternion rotation)
        {
            string data = $"{rotation.x:F3},{rotation.y:F3},{rotation.z:F3},{rotation.w:F3}";
            SendData(label, data);
        }

        /// <summary>
        /// Send Transform data
        /// </summary>
        public void SendTransform(string label, Transform transform)
        {
            string posData = $"{transform.position.x:F3},{transform.position.y:F3},{transform.position.z:F3}";
            string rotData = $"{transform.rotation.x:F3},{transform.rotation.y:F3},{transform.rotation.z:F3},{transform.rotation.w:F3}";
            SendData($"{label}_Position", posData);
            SendData($"{label}_Rotation", rotData);
        }

        /// <summary>
        /// Get connection status
        /// </summary>
        public bool IsConnected()
        {
            return isConnected;
        }

        /// <summary>
        /// Get file status
        /// </summary>
        public bool IsFileOpen()
        {
            return fileOpen;
        }

        #endregion

        #region Private Methods - Networking

        private IEnumerator DiscoveryCoroutine()
        {
            isDiscovering = true;
            LogMessage("Starting discovery...", LogLevel.Info);

            // Try to create discovery socket
            for (int portOffset = 0; portOffset < 10; portOffset++)
            {
                try
                {
                    int tryPort = DISCOVERY_BASE_PORT + portOffset;
                    discoverySocket = new UdpClient(tryPort);
                    discoverySocket.EnableBroadcast = true;
                    discoverySocket.Client.ReceiveTimeout = 100;
                    actualDiscoveryPort = tryPort;
                    LogMessage($"Discovery socket bound to port {tryPort}", LogLevel.Info);
                    break;
                }
                catch (Exception e)
                {
                    if (portOffset == 9)
                    {
                        LogMessage($"Failed to bind discovery socket: {e.Message}", LogLevel.Error);
                        isDiscovering = false;
                        yield break;
                    }
                }
            }

            // Start discovery thread
            threadRunning = true;
            discoveryThread = new Thread(DiscoveryThread);
            discoveryThread.Start();

            // Wait for discovery
            float startTime = Time.time;
            float timeout = maxDiscoveryAttempts * discoveryInterval;

            while (isDiscovering && !isConnected && Time.time - startTime < timeout)
            {
                yield return new WaitForSeconds(0.5f);
            }

            if (!isConnected)
            {
                LogMessage("Discovery timeout - C++ node not found", LogLevel.Error);
                threadRunning = false;
                isDiscovering = false;
                discoverySocket?.Close();
                yield break;
            }

            // Create data socket
            try
            {
                dataSocket = new UdpClient(DATA_PORT);
                dataSocket.Client.ReceiveTimeout = 100;
                LogMessage($"Data socket created on port {DATA_PORT}", LogLevel.Info);
            }
            catch (Exception e)
            {
                LogMessage($"Failed to create data socket: {e.Message}", LogLevel.Error);
                isConnected = false;
                yield break;
            }

            // Start receive thread BEFORE handshake (IMPORTANT!)
            receiveThread = new Thread(ReceiveThread);
            receiveThread.Start();
            LogMessage("Receive thread started", LogLevel.Info);
            yield return new WaitForSeconds(0.5f); // Let it initialize

            // THEN perform handshake
            yield return StartCoroutine(HandshakeCoroutine());

            // Check if handshake succeeded
            if (isConnected)
            {
                LogMessage("Connection established!", LogLevel.Success);
            }
        }

        private void DiscoveryThread()
        {
            byte[] buffer = new byte[BUFFER_SIZE];
            IPEndPoint broadcastEndpoint = new IPEndPoint(IPAddress.Broadcast, DISCOVERY_BASE_PORT);
            IPEndPoint receiveEndpoint = new IPEndPoint(IPAddress.Any, 0);

            while (threadRunning && !isConnected)
            {
                try
                {
                    // Send discovery broadcast to multiple ports
                    string discoverMsg = $"{MSG_DISCOVER}:{NODE_ID}:{DATA_PORT}:{actualDiscoveryPort}";
                    byte[] data = Encoding.UTF8.GetBytes(discoverMsg);

                    for (int portOffset = 0; portOffset < 10; portOffset++)
                    {
                        broadcastEndpoint.Port = DISCOVERY_BASE_PORT + portOffset;
                        discoverySocket.Send(data, data.Length, broadcastEndpoint);
                    }

                    RunOnMainThread(() => LogMessage("Broadcasting discovery...", LogLevel.Debug));

                    // Listen for response
                    try
                    {
                        buffer = discoverySocket.Receive(ref receiveEndpoint);
                        string received = Encoding.UTF8.GetString(buffer);

                        string[] parts = received.Split(':');
                        if (parts.Length >= 3)
                        {
                            string msgType = parts[0];
                            string senderId = parts[1];
                            string senderPort = parts[2];

                            if ((msgType == MSG_DISCOVER || msgType == MSG_ACKNOWLEDGE) && senderId == PEER_ID)
                            {
                                peerAddress = receiveEndpoint.Address.ToString();
                                peerPort = int.Parse(senderPort);
                                peerEndpoint = new IPEndPoint(receiveEndpoint.Address, peerPort);

                                RunOnMainThread(() =>
                                {
                                    LogMessage($"C++ node found at {peerAddress}:{peerPort}", LogLevel.Success);
                                    isConnected = true;
                                    isDiscovering = false;
                                });

                                // Send acknowledgment if it was a discover message
                                if (msgType == MSG_DISCOVER)
                                {
                                    string ackMsg = $"{MSG_ACKNOWLEDGE}:{NODE_ID}:{DATA_PORT}";
                                    byte[] ackData = Encoding.UTF8.GetBytes(ackMsg);

                                    if (parts.Length >= 4)
                                    {
                                        int peerDiscPort = int.Parse(parts[3]);
                                        IPEndPoint peerDiscEndpoint = new IPEndPoint(receiveEndpoint.Address, peerDiscPort);
                                        discoverySocket.Send(ackData, ackData.Length, peerDiscEndpoint);
                                    }
                                }

                                break;
                            }
                        }
                    }
                    catch (SocketException)
                    {
                        // Timeout - normal behavior
                    }

                    Thread.Sleep(2000);
                }
                catch (Exception e)
                {
                    RunOnMainThread(() => LogMessage($"Discovery error: {e.Message}", LogLevel.Error));
                    Thread.Sleep(2000);
                }
            }
        }

        private IEnumerator HandshakeCoroutine()
        {
            LogMessage("Starting handshake...", LogLevel.Info);
            LogMessage($"Sending handshake to {peerAddress}:{peerPort}", LogLevel.Info);
            LogMessage($"My data port is {DATA_PORT}", LogLevel.Info);

            string handshakeMsg = $"{MSG_HANDSHAKE}:{NODE_ID}";
            bool handshakeReceived = false;
            int attempts = 0;

            // Make sure receive thread is running BEFORE handshake
            if (receiveThread == null || !receiveThread.IsAlive)
            {
                receiveThread = new Thread(ReceiveThread);
                receiveThread.Start();
                LogMessage("Started receive thread for handshake", LogLevel.Debug);
                yield return new WaitForSeconds(0.5f); // Give it time to start
            }

            while (attempts < 10 && !handshakeReceived)
            {
                SendMessage(handshakeMsg);
                LogMessage($"Sending handshake (attempt {attempts + 1})", LogLevel.Debug);

                // Wait and check for response
                yield return new WaitForSeconds(0.5f);

                // Check if handshake was received (set by receive thread)
                lock (lockObject)
                {
                    if (fileOpen) // Using as a flag for successful handshake
                    {
                        handshakeReceived = true;
                        fileOpen = false; // Reset the flag
                    }
                }

                attempts++;
            }

            if (handshakeReceived)
            {
                LogMessage("Handshake complete!", LogLevel.Success);
                // Keep connection alive
                isConnected = true;
            }
            else
            {
                LogMessage("Handshake failed", LogLevel.Error);
                isConnected = false;
            }
        }


        private void ReceiveThread()
        {
            byte[] buffer = new byte[BUFFER_SIZE];
            IPEndPoint receiveEndpoint = new IPEndPoint(IPAddress.Any, 0);

            // ADD THIS:
            RunOnMainThread(() => LogMessage($"Receive thread started, listening on port {DATA_PORT}", LogLevel.Info));

            while (threadRunning && isConnected)
            {
                try
                {
                    buffer = dataSocket.Receive(ref receiveEndpoint);
                    string received = Encoding.UTF8.GetString(buffer);

                    // ADD THIS DEBUG:
                    RunOnMainThread(() => LogMessage($"Received from {receiveEndpoint}: {received}", LogLevel.Debug));

                    ProcessReceivedMessage(received);
                }
                catch (SocketException)
                {
                    // Timeout - normal behavior
                }
                catch (Exception e)
                {
                    RunOnMainThread(() => LogMessage($"Receive error: {e.Message}", LogLevel.Error));
                }

                Thread.Sleep(1);
            }
        }

        private void ProcessReceivedMessage(string message)
        {
            string[] parts = message.Split(new[] { ':' }, 2);
            if (parts.Length < 1) return;

            string msgType = parts[0];

            switch (msgType)
            {
                case MSG_HANDSHAKE:
                    // Respond with ready
                    SendMessage($"{MSG_READY}:{NODE_ID}");
                    lock (lockObject)
                    {
                        fileOpen = true; // Using as handshake flag
                    }
                    break;

                case MSG_READY:
                    lock (lockObject)
                    {
                        fileOpen = true; // Using as handshake flag
                    }
                    break;

                case MSG_STATUS:
                    if (parts.Length > 1)
                    {
                        ProcessStatusMessage(parts[1]);
                    }
                    break;

                case MSG_KEEPALIVE:
                    // Connection still alive
                    break;

                default:
                    RunOnMainThread(() => LogMessage($"Unknown message type: {msgType}", LogLevel.Warning));
                    break;
            }
        }

        private void ProcessStatusMessage(string status)
        {
            RunOnMainThread(() =>
            {
                if (status.StartsWith("FILE_OPENED:"))
                {
                    fileOpen = true;
                    currentFilename = status.Substring(12);
                    LogMessage($"File opened: {currentFilename}", LogLevel.Success);
                }
                else if (status == "FILE_CLOSED")
                {
                    fileOpen = false;
                    currentFilename = "";
                    LogMessage("File closed", LogLevel.Info);
                }
                else if (status == "FILE_OPEN_FAILED")
                {
                    LogMessage("Failed to open file", LogLevel.Error);
                }
                else
                {
                    LogMessage($"Status: {status}", LogLevel.Info);
                }
            });
        }

        private void SendMessage(string message)
        {
            if (dataSocket == null || peerEndpoint == null) return;

            try
            {
                byte[] data = Encoding.UTF8.GetBytes(message);
                dataSocket.Send(data, data.Length, peerEndpoint);
            }
            catch (Exception e)
            {
                LogMessage($"Send error: {e.Message}", LogLevel.Error);
            }
        }

        private void SendCommand(string command)
        {
            string message = $"{MSG_COMMAND}:{command}";
            SendMessage(message);
            LogMessage($"Sent command: {command}", LogLevel.Debug);
        }

        private void SendKeepalive()
        {
            SendMessage($"{MSG_KEEPALIVE}:{NODE_ID}");
        }

        #endregion

        #region Private Methods - UI

        private void SetupUI()
        {
            if (openFileButton != null)
            {
                openFileButton.onClick.AddListener(() =>
                {
                    string filename = filenameInput != null ? filenameInput.text : "";
                    OpenFile(filename);
                });
            }

            if (closeFileButton != null)
            {
                closeFileButton.onClick.AddListener(CloseFile);
            }

            if (autoDataToggle != null)
            {
                autoDataToggle.onValueChanged.AddListener((value) =>
                {
                    enablePeriodicData = value;
                });
            }
        }

        private void UpdateStatus()
        {
            if (statusText != null)
            {
                string status = isConnected ? "<color=#00FF00>Connected</color>" :
                               (isDiscovering ? "<color=#FFFF00>Discovering...</color>" :
                                "<color=#FF0000>Disconnected</color>");
                if (isConnected && fileOpen)
                {
                    status += $" | File: <color=#00FFFF>{currentFilename}</color>";
                }
                statusText.text = $"Status: {status}";
            }

            if (connectionInfoText != null && isConnected)
            {
                connectionInfoText.text = $"Connected to: {peerAddress}:{peerPort}";
            }

            if (dataCountText != null)
            {
                dataCountText.text = $"Data Sent: <b>{dataCount}</b>";
            }

            if (openFileButton != null)
            {
                openFileButton.interactable = isConnected && !fileOpen;
            }

            if (closeFileButton != null)
            {
                closeFileButton.interactable = isConnected && fileOpen;
            }
        }

        #endregion

        #region Private Methods - Data Generation

        private float lastDataSend = 0;

        private void SendPeriodicData()
        {
            if (Time.time - lastDataSend < dataSendInterval) return;

            // Send XR device data if available
            SendXRDeviceData();

            // Send timestamp
            SendData("Timestamp", Time.time.ToString("F3"));

            // Send frame count
            SendData("Frame", Time.frameCount.ToString());

            // Send performance metrics
            float fps = 1.0f / Time.deltaTime;
            SendData("FPS", fps.ToString("F1"));

            lastDataSend = Time.time;
        }

        #endregion

        #region Helper Methods

        private void RunOnMainThread(Action action)
        {
            lock (mainThreadQueue)
            {
                mainThreadQueue.Enqueue(action);
            }
        }

        private void LogMessage(string message, LogLevel level)
        {
            if (!debugMode && level == LogLevel.Debug) return;

            string prefix = $"[AIMLAB-VR] ";

            // Log to debug text if available
            if (debugText != null && level != LogLevel.Debug)
            {
                string coloredMessage = level switch
                {
                    LogLevel.Success => $"<color=#00FF00>{message}</color>",
                    LogLevel.Warning => $"<color=#FFFF00>{message}</color>",
                    LogLevel.Error => $"<color=#FF0000>{message}</color>",
                    _ => message
                };

                debugText.text = $"{DateTime.Now:HH:mm:ss} - {coloredMessage}\n" + debugText.text;

                // Limit debug text to last 10 lines
                string[] lines = debugText.text.Split('\n');
                if (lines.Length > 10)
                {
                    debugText.text = string.Join("\n", lines, 0, 10);
                }
            }

            // Log to console
            switch (level)
            {
                case LogLevel.Info:
                    Debug.Log(prefix + message);
                    break;
                case LogLevel.Success:
                    Debug.Log($"<color=green>{prefix}{message}</color>");
                    break;
                case LogLevel.Warning:
                    Debug.LogWarning(prefix + message);
                    break;
                case LogLevel.Error:
                    Debug.LogError(prefix + message);
                    break;
                case LogLevel.Debug:
                    Debug.Log($"<color=gray>{prefix}[DEBUG] {message}</color>");
                    break;
            }
        }

        private enum LogLevel
        {
            Debug,
            Info,
            Success,
            Warning,
            Error
        }

        #endregion
    }

    /// <summary>
    /// Example VR controller data sender using new Input System
    /// </summary>
    public class VRControllerDataSender : MonoBehaviour
    {
        [Header("Data Streamer Reference")]
        public AIMLABVRDataStreamer dataStreamer;

        [Header("Settings")]
        public bool sendControllerData = true;
        public float sendRate = 0.1f;

        [Header("Input Actions")]
        public InputActionReference positionAction;
        public InputActionReference rotationAction;
        public InputActionReference triggerAction;
        public InputActionReference gripAction;
        public InputActionReference primaryButtonAction;
        public InputActionReference secondaryButtonAction;
        public InputActionReference thumbstickAction;

        private float lastSend = 0;

        void Start()
        {
            // Enable all input actions
            if (positionAction != null) positionAction.action.Enable();
            if (rotationAction != null) rotationAction.action.Enable();
            if (triggerAction != null) triggerAction.action.Enable();
            if (gripAction != null) gripAction.action.Enable();
            if (primaryButtonAction != null) primaryButtonAction.action.Enable();
            if (secondaryButtonAction != null) secondaryButtonAction.action.Enable();
            if (thumbstickAction != null) thumbstickAction.action.Enable();
        }

        void Update()
        {
            if (!sendControllerData || dataStreamer == null || !dataStreamer.IsFileOpen())
                return;

            if (Time.time - lastSend < sendRate)
                return;

            string controllerName = $"Controller_{gameObject.name}";

            // Send position if available
            if (positionAction != null)
            {
                Vector3 position = positionAction.action.ReadValue<Vector3>();
                dataStreamer.SendVector3($"{controllerName}_Position", position);
            }

            // Send rotation if available
            if (rotationAction != null)
            {
                Quaternion rotation = rotationAction.action.ReadValue<Quaternion>();
                dataStreamer.SendQuaternion($"{controllerName}_Rotation", rotation);
            }

            // Send trigger value
            if (triggerAction != null)
            {
                float triggerValue = triggerAction.action.ReadValue<float>();
                dataStreamer.SendData($"{controllerName}_Trigger", triggerValue.ToString("F3"));
            }

            // Send grip value
            if (gripAction != null)
            {
                float gripValue = gripAction.action.ReadValue<float>();
                dataStreamer.SendData($"{controllerName}_Grip", gripValue.ToString("F3"));
            }

            // Send button states
            if (primaryButtonAction != null)
            {
                bool primaryPressed = primaryButtonAction.action.ReadValue<float>() > 0.5f;
                dataStreamer.SendData($"{controllerName}_PrimaryButton", primaryPressed.ToString());
            }

            if (secondaryButtonAction != null)
            {
                bool secondaryPressed = secondaryButtonAction.action.ReadValue<float>() > 0.5f;
                dataStreamer.SendData($"{controllerName}_SecondaryButton", secondaryPressed.ToString());
            }

            // Send thumbstick
            if (thumbstickAction != null)
            {
                Vector2 thumbstick = thumbstickAction.action.ReadValue<Vector2>();
                dataStreamer.SendVector2($"{controllerName}_Thumbstick", thumbstick);
            }

            lastSend = Time.time;
        }

        void OnDestroy()
        {
            // Disable input actions
            if (positionAction != null) positionAction.action.Disable();
            if (rotationAction != null) rotationAction.action.Disable();
            if (triggerAction != null) triggerAction.action.Disable();
            if (gripAction != null) gripAction.action.Disable();
            if (primaryButtonAction != null) primaryButtonAction.action.Disable();
            if (secondaryButtonAction != null) secondaryButtonAction.action.Disable();
            if (thumbstickAction != null) thumbstickAction.action.Disable();
        }
    }
}
