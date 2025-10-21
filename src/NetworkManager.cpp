////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Network Manager Implementation
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Platform-agnostic network communication implementation with support for
//   UDP and TCP protocols.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/NetworkManager.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <thread>
#include <atomic>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
    #define INVALID_SOCKET_VALUE INVALID_SOCKET
    #define SOCKET_ERROR_VALUE SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    typedef int SocketType;
    #define INVALID_SOCKET_VALUE -1
    #define SOCKET_ERROR_VALUE -1
    #define closesocket close
#endif

namespace AimlabVR {

/**
 * @brief Private implementation class for NetworkManager
 */
class NetworkManager::Impl {
public:
    SocketType socket;
    std::string serverAddress;
    uint16_t serverPort;
    NetworkProtocol protocol;
    ConnectionStatus status;
    std::string lastError;
    PacketFormat packetFormat;
    
    std::thread receiveThread;
    std::atomic<bool> isReceiving;
    ReceiveCallback receiveCallback;
    
    struct sockaddr_in serverAddr;

    Impl()
        : socket(INVALID_SOCKET_VALUE)
        , serverPort(0)
        , protocol(NetworkProtocol::UDP)
        , status(ConnectionStatus::Disconnected)
        , packetFormat(PacketFormat::Binary)
        , isReceiving(false) {
    }

    ~Impl() {
        cleanup();
    }

    void cleanup() {
        if (socket != INVALID_SOCKET_VALUE) {
            closesocket(socket);
            socket = INVALID_SOCKET_VALUE;
        }
        
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

NetworkManager::NetworkManager()
    : m_pImpl(std::make_unique<Impl>()) {
}

NetworkManager::~NetworkManager() {
    disconnect();
    stopReceiving();
}

bool NetworkManager::initialize(const std::string& serverAddress, uint16_t port, NetworkProtocol protocol) {
    LOG_INFO("Initializing NetworkManager");

    m_pImpl->serverAddress = serverAddress;
    m_pImpl->serverPort = port;
    m_pImpl->protocol = protocol;

#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        m_pImpl->lastError = "WSAStartup failed: " + std::to_string(result);
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }
#endif

    if (!initializeSocket()) {
        return false;
    }

    LOG_INFO("NetworkManager initialized successfully");
    return true;
}

bool NetworkManager::initializeSocket() {
    int sockType = (m_pImpl->protocol == NetworkProtocol::UDP) ? SOCK_DGRAM : SOCK_STREAM;
    int sockProto = (m_pImpl->protocol == NetworkProtocol::UDP) ? IPPROTO_UDP : IPPROTO_TCP;

    m_pImpl->socket = socket(AF_INET, sockType, sockProto);
    if (m_pImpl->socket == INVALID_SOCKET_VALUE) {
        m_pImpl->lastError = "Failed to create socket";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    // Setup server address
    m_pImpl->serverAddr.sin_family = AF_INET;
    m_pImpl->serverAddr.sin_port = htons(m_pImpl->serverPort);
    
#ifdef _WIN32
    m_pImpl->serverAddr.sin_addr.s_addr = inet_addr(m_pImpl->serverAddress.c_str());
#else
    inet_pton(AF_INET, m_pImpl->serverAddress.c_str(), &m_pImpl->serverAddr.sin_addr);
#endif

    return true;
}

void NetworkManager::cleanupSocket() {
    m_pImpl->cleanup();
}

bool NetworkManager::connect() {
    LOG_INFO("Connecting to " + m_pImpl->serverAddress + ":" + std::to_string(m_pImpl->serverPort));
    
    m_pImpl->status = ConnectionStatus::Connecting;

    if (m_pImpl->protocol == NetworkProtocol::TCP) {
        int result = ::connect(m_pImpl->socket, 
                              (struct sockaddr*)&m_pImpl->serverAddr, 
                              sizeof(m_pImpl->serverAddr));
        if (result == SOCKET_ERROR_VALUE) {
            m_pImpl->lastError = "TCP connect failed";
            LOG_ERROR(m_pImpl->lastError);
            m_pImpl->status = ConnectionStatus::Error;
            return false;
        }
    }

    m_pImpl->status = ConnectionStatus::Connected;
    LOG_INFO("Connected successfully");
    return true;
}

void NetworkManager::disconnect() {
    if (m_pImpl->status == ConnectionStatus::Disconnected) {
        return;
    }

    LOG_INFO("Disconnecting from server");
    stopReceiving();
    cleanupSocket();
    m_pImpl->status = ConnectionStatus::Disconnected;
}

bool NetworkManager::send(const DataPacket& packet) {
    if (m_pImpl->status != ConnectionStatus::Connected) {
        m_pImpl->lastError = "Not connected";
        return false;
    }

    std::vector<uint8_t> data = packet.serialize();
    
    bool success = false;
    if (m_pImpl->protocol == NetworkProtocol::UDP) {
        success = sendUDP(data);
    } else {
        success = sendTCP(data);
    }

    return success;
}

bool NetworkManager::sendUDP(const std::vector<uint8_t>& data) {
    int result = sendto(m_pImpl->socket, 
                       reinterpret_cast<const char*>(data.data()), 
                       static_cast<int>(data.size()), 
                       0,
                       (struct sockaddr*)&m_pImpl->serverAddr,
                       sizeof(m_pImpl->serverAddr));
    
    if (result == SOCKET_ERROR_VALUE) {
        m_pImpl->lastError = "UDP send failed";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    return true;
}

bool NetworkManager::sendTCP(const std::vector<uint8_t>& data) {
    int result = ::send(m_pImpl->socket,
                       reinterpret_cast<const char*>(data.data()),
                       static_cast<int>(data.size()),
                       0);
    
    if (result == SOCKET_ERROR_VALUE) {
        m_pImpl->lastError = "TCP send failed";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    return true;
}

bool NetworkManager::receive(DataPacket& packet, uint32_t timeoutMs) {
    std::vector<uint8_t> data;
    
    bool success = false;
    if (m_pImpl->protocol == NetworkProtocol::UDP) {
        success = receiveUDP(data, timeoutMs);
    } else {
        success = receiveTCP(data, timeoutMs);
    }

    if (success && !data.empty()) {
        packet.setFormat(m_pImpl->packetFormat);
        return packet.deserialize(data);
    }

    return false;
}

bool NetworkManager::receiveUDP(std::vector<uint8_t>& data, uint32_t timeoutMs) {
    char buffer[65536];
    struct sockaddr_in fromAddr;
    
#ifdef _WIN32
    int fromLen = sizeof(fromAddr);
#else
    socklen_t fromLen = sizeof(fromAddr);
#endif

    int result = recvfrom(m_pImpl->socket,
                         buffer,
                         sizeof(buffer),
                         0,
                         (struct sockaddr*)&fromAddr,
                         &fromLen);

    if (result > 0) {
        data.assign(buffer, buffer + result);
        return true;
    }

    return false;
}

bool NetworkManager::receiveTCP(std::vector<uint8_t>& data, uint32_t timeoutMs) {
    char buffer[65536];
    
    int result = recv(m_pImpl->socket,
                     buffer,
                     sizeof(buffer),
                     0);

    if (result > 0) {
        data.assign(buffer, buffer + result);
        return true;
    }

    return false;
}

void NetworkManager::setReceiveCallback(ReceiveCallback callback) {
    m_pImpl->receiveCallback = callback;
}

bool NetworkManager::startReceiving() {
    if (m_pImpl->isReceiving) {
        LOG_WARNING("Already receiving");
        return false;
    }

    LOG_INFO("Starting receive thread");
    m_pImpl->isReceiving = true;
    m_pImpl->receiveThread = std::thread(&NetworkManager::receiveThreadFunc, this);
    
    return true;
}

void NetworkManager::stopReceiving() {
    if (!m_pImpl->isReceiving) {
        return;
    }

    LOG_INFO("Stopping receive thread");
    m_pImpl->isReceiving = false;
    
    if (m_pImpl->receiveThread.joinable()) {
        m_pImpl->receiveThread.join();
    }
}

void NetworkManager::receiveThreadFunc() {
    LOG_INFO("Receive thread started");

    while (m_pImpl->isReceiving) {
        DataPacket packet;
        if (receive(packet, 100)) {
            if (m_pImpl->receiveCallback) {
                m_pImpl->receiveCallback(packet);
            }
        }
    }

    LOG_INFO("Receive thread stopped");
}

ConnectionStatus NetworkManager::getStatus() const {
    return m_pImpl->status;
}

bool NetworkManager::isConnected() const {
    return m_pImpl->status == ConnectionStatus::Connected;
}

std::string NetworkManager::getLastError() const {
    return m_pImpl->lastError;
}

void NetworkManager::setPacketFormat(PacketFormat format) {
    m_pImpl->packetFormat = format;
}

} // namespace AimlabVR

