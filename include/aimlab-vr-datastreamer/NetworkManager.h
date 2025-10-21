////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Network Manager
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Platform-agnostic network manager for sending and receiving VR tracking
//   data over UDP, TCP, or WebSocket protocols. Handles connection management
//   and data transmission.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_NETWORKMANAGER_H
#define AIMLAB_VR_NETWORKMANAGER_H

#include "Types.h"
#include "DataPacket.h"
#include <string>
#include <memory>
#include <functional>

namespace AimlabVR {

/**
 * @class NetworkManager
 * @brief Manages network communication for VR data streaming
 * 
 * Provides platform-agnostic network communication using various protocols.
 * Handles connection management, data transmission, and error handling.
 */
class NetworkManager {
public:
    /**
     * @brief Callback function for received data
     * @param packet Received data packet
     */
    using ReceiveCallback = std::function<void(const DataPacket& packet)>;

    /**
     * @brief Constructor
     */
    NetworkManager();

    /**
     * @brief Destructor
     */
    ~NetworkManager();

    /**
     * @brief Initialize network manager with configuration
     * @param serverAddress Server IP address or hostname
     * @param port Server port
     * @param protocol Network protocol to use
     * @return True if successful, false otherwise
     */
    bool initialize(const std::string& serverAddress, uint16_t port, NetworkProtocol protocol);

    /**
     * @brief Connect to server
     * @return True if successful, false otherwise
     */
    bool connect();

    /**
     * @brief Disconnect from server
     */
    void disconnect();

    /**
     * @brief Send data packet to server
     * @param packet Data packet to send
     * @return True if successful, false otherwise
     */
    bool send(const DataPacket& packet);

    /**
     * @brief Receive data packet from server (blocking)
     * @param packet Output packet
     * @param timeoutMs Timeout in milliseconds
     * @return True if successful, false otherwise
     */
    bool receive(DataPacket& packet, uint32_t timeoutMs = 1000);

    /**
     * @brief Set callback for asynchronous data reception
     * @param callback Callback function
     */
    void setReceiveCallback(ReceiveCallback callback);

    /**
     * @brief Start asynchronous receiving thread
     * @return True if successful, false otherwise
     */
    bool startReceiving();

    /**
     * @brief Stop asynchronous receiving thread
     */
    void stopReceiving();

    /**
     * @brief Get current connection status
     * @return Connection status
     */
    ConnectionStatus getStatus() const;

    /**
     * @brief Check if connected
     * @return True if connected, false otherwise
     */
    bool isConnected() const;

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string getLastError() const;

    /**
     * @brief Set packet format for transmission
     * @param format Packet format
     */
    void setPacketFormat(PacketFormat format);

private:
    /**
     * @brief Platform-specific socket initialization
     */
    bool initializeSocket();

    /**
     * @brief Platform-specific socket cleanup
     */
    void cleanupSocket();

    /**
     * @brief Receive thread function
     */
    void receiveThreadFunc();

    /**
     * @brief Send via UDP
     */
    bool sendUDP(const std::vector<uint8_t>& data);

    /**
     * @brief Send via TCP
     */
    bool sendTCP(const std::vector<uint8_t>& data);

    /**
     * @brief Receive via UDP
     */
    bool receiveUDP(std::vector<uint8_t>& data, uint32_t timeoutMs);

    /**
     * @brief Receive via TCP
     */
    bool receiveTCP(std::vector<uint8_t>& data, uint32_t timeoutMs);

    class Impl;
    std::unique_ptr<Impl> m_pImpl;  // PIMPL idiom for platform-specific implementation
};

} // namespace AimlabVR

#endif // AIMLAB_VR_NETWORKMANAGER_H

