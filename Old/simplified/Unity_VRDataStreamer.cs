/**
 * @file Unity_VRDataStreamer.cs
 * @brief Unity VR Data Streamer for AIMLAB VR experiments
 * @author Pi Ko (pi.ko@nyu.edu)
 * @date 04 November 2025
 * @version v1.0
 * 
 * Unity script for streaming VR headset and controller data to the console application.
 * Handles network discovery, TCP streaming, and coordinate system conversion.
 * 
 * Setup Instructions:
 * 1. Attach this script to an empty GameObject in your VR scene
 * 2. Assign XR camera and controllers in the Inspector
 * 3. Ensure the PC running the console app is on the same network
 * 4. Run your VR application - it will auto-broadcast for discovery
 * 
 * Network Protocol:
 * - Discovery: UDP broadcast on port 55001
 * - Data Stream: TCP on port 55000
 * - Handshake: Receives "DATA_RECEIVER_READY" → Sends "VR_HEADSET_READY"
 * 
 * Changelog:
 * v1.0 - 04 November 2025 - Initial implementation
 *   - UDP discovery broadcast
 *   - TCP data streaming
 *   - XR tracking data collection
 */

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using UnityEngine.XR;

public class VRDataStreamer : MonoBehaviour
{
    [Header("VR Tracking Objects")]
    [Tooltip("Main VR camera (headset)")]
    public Transform headTransform;
    
    [Tooltip("Left hand controller")]
    public Transform leftHandTransform;
    
    [Tooltip("Right hand controller")]
    public Transform rightHandTransform;

    [Header("Network Settings")]
    [Tooltip("TCP port for data streaming")]
    public int dataPort = 55000;
    
    [Tooltip("UDP port for discovery broadcast")]
    public int discoveryPort = 55001;
    
    [Tooltip("Data streaming rate (packets per second)")]
    public int streamRate = 90;

    [Header("Status")]
    public bool isConnected = false;
    public string connectedClientIP = "None";

    // Network components
    private TcpListener tcpListener;
    private TcpClient connectedClient;
    private NetworkStream networkStream;
    private UdpClient udpBroadcast;
    private Thread discoveryThread;
    private Thread connectionThread;
    private bool isRunning = true;

    // Timing
    private float streamInterval;
    private float lastStreamTime;

    /**
     * @brief Initialize VR tracking and network components
     */
    void Start()
    {
        // Auto-find VR components if not assigned
        if (headTransform == null)
        {
            GameObject mainCamera = GameObject.FindGameObjectWithTag("MainCamera");
            if (mainCamera != null)
                headTransform = mainCamera.transform;
        }

        // Calculate stream interval
        streamInterval = 1.0f / streamRate;
        lastStreamTime = Time.time;

        // Start network services
        StartNetworkServices();
        
        Debug.Log("[VR Data Streamer] Started - Waiting for connection...");
    }

    /**
     * @brief Start UDP discovery and TCP listener
     */
    void StartNetworkServices()
    {
        try
        {
            // Start UDP discovery broadcast
            discoveryThread = new Thread(DiscoveryBroadcastLoop);
            discoveryThread.IsBackground = true;
            discoveryThread.Start();

            // Start TCP listener
            connectionThread = new Thread(ConnectionListenerLoop);
            connectionThread.IsBackground = true;
            connectionThread.Start();
        }
        catch (Exception e)
        {
            Debug.LogError($"[VR Data Streamer] Failed to start network services: {e.Message}");
        }
    }

    /**
     * @brief UDP broadcast loop for discovery
     */
    void DiscoveryBroadcastLoop()
    {
        try
        {
            udpBroadcast = new UdpClient();
            udpBroadcast.EnableBroadcast = true;
            IPEndPoint broadcastEndpoint = new IPEndPoint(IPAddress.Broadcast, discoveryPort);

            byte[] message = Encoding.UTF8.GetBytes("VR_HEADSET_DISCOVERY");

            while (isRunning)
            {
                if (!isConnected)
                {
                    udpBroadcast.Send(message, message.Length, broadcastEndpoint);
                }
                Thread.Sleep(1000);  // Broadcast every second
            }
        }
        catch (Exception e)
        {
            Debug.LogError($"[VR Data Streamer] Discovery broadcast error: {e.Message}");
        }
    }

    /**
     * @brief TCP listener loop for incoming connections
     */
    void ConnectionListenerLoop()
    {
        try
        {
            tcpListener = new TcpListener(IPAddress.Any, dataPort);
            tcpListener.Start();
            Debug.Log($"[VR Data Streamer] TCP listener started on port {dataPort}");

            while (isRunning)
            {
                if (!isConnected && tcpListener.Pending())
                {
                    connectedClient = tcpListener.AcceptTcpClient();
                    networkStream = connectedClient.GetStream();

                    // Handshake
                    byte[] buffer = new byte[256];
                    int bytesRead = networkStream.Read(buffer, 0, buffer.Length);
                    string handshake = Encoding.UTF8.GetString(buffer, 0, bytesRead).Trim();

                    if (handshake == "DATA_RECEIVER_READY")
                    {
                        byte[] response = Encoding.UTF8.GetBytes("VR_HEADSET_READY\n");
                        networkStream.Write(response, 0, response.Length);
                        networkStream.Flush();

                        isConnected = true;
                        connectedClientIP = ((IPEndPoint)connectedClient.Client.RemoteEndPoint).Address.ToString();
                        Debug.Log($"[VR Data Streamer] Connected to {connectedClientIP}");
                    }
                }
                Thread.Sleep(100);
            }
        }
        catch (Exception e)
        {
            Debug.LogError($"[VR Data Streamer] Connection listener error: {e.Message}");
        }
    }

    /**
     * @brief Update loop - stream VR data at specified rate
     */
    void Update()
    {
        if (isConnected && networkStream != null)
        {
            // Check if it's time to send data
            if (Time.time - lastStreamTime >= streamInterval)
            {
                StreamVRData();
                lastStreamTime = Time.time;
            }

            // Check for incoming messages
            if (networkStream.DataAvailable)
            {
                byte[] buffer = new byte[256];
                int bytesRead = networkStream.Read(buffer, 0, buffer.Length);
                string message = Encoding.UTF8.GetString(buffer, 0, bytesRead).Trim();
                Debug.Log($"[VR Data Streamer] Received: {message}");
            }

            // Check if client is still connected
            if (!IsClientConnected())
            {
                DisconnectClient();
            }
        }
    }

    /**
     * @brief Stream current VR tracking data
     */
    void StreamVRData()
    {
        try
        {
            StringBuilder data = new StringBuilder();

            // Head position and rotation
            if (headTransform != null)
            {
                Vector3 headPos = headTransform.position;
                Quaternion headRot = headTransform.rotation;
                data.AppendFormat("{0},{1},{2},{3},{4},{5},{6}",
                    headPos.x, headPos.y, headPos.z,
                    headRot.x, headRot.y, headRot.z, headRot.w);
            }
            else
            {
                data.Append("0,0,0,0,0,0,1");
            }

            data.Append(",");

            // Left hand position and rotation
            if (leftHandTransform != null)
            {
                Vector3 leftPos = leftHandTransform.position;
                Quaternion leftRot = leftHandTransform.rotation;
                data.AppendFormat("{0},{1},{2},{3},{4},{5},{6}",
                    leftPos.x, leftPos.y, leftPos.z,
                    leftRot.x, leftRot.y, leftRot.z, leftRot.w);
            }
            else
            {
                data.Append("0,0,0,0,0,0,1");
            }

            data.Append(",");

            // Right hand position and rotation
            if (rightHandTransform != null)
            {
                Vector3 rightPos = rightHandTransform.position;
                Quaternion rightRot = rightHandTransform.rotation;
                data.AppendFormat("{0},{1},{2},{3},{4},{5},{6}",
                    rightPos.x, rightPos.y, rightPos.z,
                    rightRot.x, rightRot.y, rightRot.z, rightRot.w);
            }
            else
            {
                data.Append("0,0,0,0,0,0,1");
            }

            data.Append("\n");

            // Send data
            byte[] bytes = Encoding.UTF8.GetBytes(data.ToString());
            networkStream.Write(bytes, 0, bytes.Length);
            networkStream.Flush();
        }
        catch (Exception e)
        {
            Debug.LogError($"[VR Data Streamer] Error streaming data: {e.Message}");
            DisconnectClient();
        }
    }

    /**
     * @brief Check if TCP client is still connected
     */
    bool IsClientConnected()
    {
        try
        {
            return !(connectedClient.Client.Poll(1, SelectMode.SelectRead) && 
                     connectedClient.Client.Available == 0);
        }
        catch
        {
            return false;
        }
    }

    /**
     * @brief Disconnect current client
     */
    void DisconnectClient()
    {
        isConnected = false;
        connectedClientIP = "None";

        if (networkStream != null)
        {
            networkStream.Close();
            networkStream = null;
        }

        if (connectedClient != null)
        {
            connectedClient.Close();
            connectedClient = null;
        }

        Debug.Log("[VR Data Streamer] Client disconnected");
    }

    /**
     * @brief Cleanup on application quit
     */
    void OnApplicationQuit()
    {
        isRunning = false;

        DisconnectClient();

        if (discoveryThread != null && discoveryThread.IsAlive)
            discoveryThread.Abort();

        if (connectionThread != null && connectionThread.IsAlive)
            connectionThread.Abort();

        if (tcpListener != null)
            tcpListener.Stop();

        if (udpBroadcast != null)
            udpBroadcast.Close();

        Debug.Log("[VR Data Streamer] Stopped");
    }

    /**
     * @brief Display status in Unity Editor
     */
    void OnGUI()
    {
        GUIStyle style = new GUIStyle();
        style.fontSize = 20;
        style.normal.textColor = isConnected ? Color.green : Color.red;
        
        string status = isConnected 
            ? $"Connected to: {connectedClientIP}" 
            : "Waiting for connection...";
        
        GUI.Label(new Rect(10, 10, 400, 30), status, style);
    }
}

