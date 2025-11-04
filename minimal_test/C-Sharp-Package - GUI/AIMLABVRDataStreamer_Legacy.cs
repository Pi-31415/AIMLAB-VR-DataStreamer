/**
 * @file AIMLABVRDataStreamer_Legacy.cs
 * @brief AIMLAB-VR-Data Streamer - Unity Component (Legacy Version)
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 02 November 2025
 * @version v1.0 (Legacy)
 * 
 * Unity C# component that communicates with the C++ data streamer via UDP.
 * Handles auto-discovery, command sending, and real-time data transmission.
 * 
 * Note: This is the legacy version. For new projects, use AIMLABVRDataStreamer.cs (v2.0)
 *       which includes TextMeshPro and modern Input System support.
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

namespace AIMLAB.VR
{
    /// <summary>
    /// Main data streamer component for Unity VR applications
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
        
        [Header("UI References (Optional)")]
        public Text statusText;
        public InputField filenameInput;
        public Button openFileButton;
        public Button closeFileButton;
        public Text dataCountText;
        public Toggle autoDataToggle;
        
        [Header("Debug")]
        public bool debugMode = false;
        
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
        
        #endregion
        
        #region Unity Lifecycle
        
        void Awake()
        {
            if (autoConnect)
            {
                StartConnection();
            }
        }
        
        void Start()
        {
            SetupUI();
            InvokeRepeating(nameof(UpdateStatus), 0.5f, 0.5f);
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
            
            // Perform handshake
            yield return StartCoroutine(HandshakeCoroutine());
            
            if (isConnected)
            {
                // Start receive thread
                receiveThread = new Thread(ReceiveThread);
                receiveThread.Start();
                
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
            
            string handshakeMsg = $"{MSG_HANDSHAKE}:{NODE_ID}";
            bool handshakeSent = false;
            bool handshakeReceived = false;
            int attempts = 0;
            
            while (attempts < 10 && (!handshakeSent || !handshakeReceived))
            {
                if (!handshakeSent)
                {
                    SendMessage(handshakeMsg);
                    LogMessage($"Sending handshake (attempt {attempts + 1})", LogLevel.Debug);
                }
                
                // Check for response (handled in receive thread)
                yield return new WaitForSeconds(0.5f);
                
                // This would be set by receive thread
                lock (lockObject)
                {
                    if (fileOpen) // Using as a flag for successful handshake
                    {
                        handshakeReceived = true;
                        handshakeSent = true;
                        fileOpen = false; // Reset
                    }
                }
                
                attempts++;
            }
            
            if (handshakeSent && handshakeReceived)
            {
                LogMessage("Handshake complete!", LogLevel.Success);
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
            
            while (threadRunning && isConnected)
            {
                try
                {
                    buffer = dataSocket.Receive(ref receiveEndpoint);
                    string received = Encoding.UTF8.GetString(buffer);
                    
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
                string status = isConnected ? "Connected" : (isDiscovering ? "Discovering..." : "Disconnected");
                if (isConnected && fileOpen)
                {
                    status += $" | File: {currentFilename}";
                }
                statusText.text = $"Status: {status}";
            }
            
            if (dataCountText != null)
            {
                dataCountText.text = $"Data Sent: {dataCount}";
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
            
            // Example: Send headset transform
            if (Camera.main != null)
            {
                SendTransform("Headset", Camera.main.transform);
            }
            
            // Example: Send timestamp
            SendData("Timestamp", Time.time.ToString("F3"));
            
            // Example: Send frame count
            SendData("Frame", Time.frameCount.ToString());
            
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
    /// Example VR controller data sender
    /// </summary>
    public class VRControllerDataSender : MonoBehaviour
    {
        public AIMLABVRDataStreamer dataStreamer;
        public bool sendControllerData = true;
        public float sendRate = 0.1f;
        
        private float lastSend = 0;
        
        void Update()
        {
            if (!sendControllerData || dataStreamer == null || !dataStreamer.IsFileOpen())
                return;
                
            if (Time.time - lastSend < sendRate)
                return;
            
            // Send controller transform
            dataStreamer.SendTransform($"Controller_{gameObject.name}", transform);
            
            // Example: Send button states (replace with actual VR input)
            bool triggerPressed = Input.GetKey(KeyCode.Space);
            dataStreamer.SendData($"Controller_{gameObject.name}_Trigger", triggerPressed.ToString());
            
            lastSend = Time.time;
        }
    }
}
