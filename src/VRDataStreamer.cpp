////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Main VR Data Streamer Implementation
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Main implementation of the VR data streaming system.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/VRDataStreamer.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <thread>
#include <atomic>
#include <chrono>

namespace AimlabVR {

/**
 * @brief Private implementation class for VRDataStreamer
 */
class VRDataStreamer::Impl {
public:
    Config config;
    std::unique_ptr<VRDevice> vrDevice;
    std::unique_ptr<NetworkManager> networkManager;
    
    std::thread streamingThread;
    std::atomic<bool> isStreaming;
    std::atomic<bool> shouldStop;
    
    ErrorCallback errorCallback;
    DataSentCallback dataSentCallback;
    
    std::string lastError;
    
    // Statistics
    uint64_t totalPacketsSent;
    std::chrono::high_resolution_clock::time_point startTime;
    uint32_t sequenceNumber;

    Impl()
        : isStreaming(false)
        , shouldStop(false)
        , totalPacketsSent(0)
        , sequenceNumber(0) {
    }

    ~Impl() {
        cleanup();
    }

    void cleanup() {
        if (vrDevice) {
            vrDevice->shutdown();
        }
        if (networkManager) {
            networkManager->disconnect();
        }
    }
};

VRDataStreamer::VRDataStreamer()
    : m_pImpl(std::make_unique<Impl>()) {
}

VRDataStreamer::~VRDataStreamer() {
    shutdown();
}

bool VRDataStreamer::initialize(const std::string& configPath, bool useMockDevice) {
    LOG_INFO("Initializing VRDataStreamer");

    // Load configuration
    if (!configPath.empty()) {
        if (!m_pImpl->config.loadFromFile(configPath)) {
            m_pImpl->lastError = "Failed to load configuration from: " + configPath;
            LOG_WARNING(m_pImpl->lastError + ", using defaults");
        }
    }

    return initialize(m_pImpl->config.getStreamConfig(), useMockDevice);
}

bool VRDataStreamer::initialize(const StreamConfig& config, bool useMockDevice) {
    LOG_INFO("Initializing VRDataStreamer with provided config");

    m_pImpl->config.setStreamConfig(config);

    if (!m_pImpl->config.validate()) {
        m_pImpl->lastError = "Invalid configuration";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    // Initialize logger
    initializeLogger();

    // Create VR device
    m_pImpl->vrDevice = createVRDevice(useMockDevice);
    if (!m_pImpl->vrDevice) {
        m_pImpl->lastError = "Failed to create VR device";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    if (!m_pImpl->vrDevice->initialize()) {
        m_pImpl->lastError = "Failed to initialize VR device";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    // Create network manager
    m_pImpl->networkManager = std::make_unique<NetworkManager>();
    if (!m_pImpl->networkManager->initialize(
            config.serverAddress,
            config.serverPort,
            config.protocol)) {
        m_pImpl->lastError = "Failed to initialize network manager";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    // Connect to server
    if (!m_pImpl->networkManager->connect()) {
        m_pImpl->lastError = "Failed to connect to server: " + m_pImpl->networkManager->getLastError();
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    LOG_INFO("VRDataStreamer initialized successfully");
    return true;
}

void VRDataStreamer::initializeLogger() {
    StreamConfig config = m_pImpl->config.getStreamConfig();
    Logger::getInstance().initialize(
        config.enableLogging,
        config.logFilePath,
        config.logLevel
    );
}

void VRDataStreamer::shutdown() {
    LOG_INFO("Shutting down VRDataStreamer");
    
    stopStreaming();
    m_pImpl->cleanup();
    
    Logger::getInstance().shutdown();
}

bool VRDataStreamer::startStreaming() {
    if (m_pImpl->isStreaming) {
        LOG_WARNING("Already streaming");
        return false;
    }

    if (!m_pImpl->vrDevice || !m_pImpl->networkManager) {
        m_pImpl->lastError = "System not initialized";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    if (!m_pImpl->networkManager->isConnected()) {
        m_pImpl->lastError = "Not connected to server";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    LOG_INFO("Starting VR data streaming");
    
    m_pImpl->isStreaming = true;
    m_pImpl->shouldStop = false;
    m_pImpl->totalPacketsSent = 0;
    m_pImpl->sequenceNumber = 0;
    m_pImpl->startTime = std::chrono::high_resolution_clock::now();
    
    m_pImpl->streamingThread = std::thread(&VRDataStreamer::streamingThreadFunc, this);

    LOG_INFO("VR data streaming started");
    return true;
}

void VRDataStreamer::stopStreaming() {
    if (!m_pImpl->isStreaming) {
        return;
    }

    LOG_INFO("Stopping VR data streaming");
    
    m_pImpl->shouldStop = true;
    m_pImpl->isStreaming = false;

    if (m_pImpl->streamingThread.joinable()) {
        m_pImpl->streamingThread.join();
    }

    LOG_INFO("VR data streaming stopped");
}

bool VRDataStreamer::isStreaming() const {
    return m_pImpl->isStreaming;
}

bool VRDataStreamer::sendFrame() {
    if (!m_pImpl->vrDevice || !m_pImpl->networkManager) {
        return false;
    }

    // Update VR device state
    if (!m_pImpl->vrDevice->update()) {
        m_pImpl->lastError = "Failed to update VR device";
        LOG_ERROR(m_pImpl->lastError);
        return false;
    }

    // Get device data
    std::vector<DeviceData> devices = m_pImpl->vrDevice->getDevices();

    // Create data packet
    DataPacket packet;
    packet.setSequenceNumber(m_pImpl->sequenceNumber++);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    packet.setTimestamp(timestamp);

    for (const auto& device : devices) {
        packet.addDevice(device);
    }

    // Send packet
    if (!m_pImpl->networkManager->send(packet)) {
        m_pImpl->lastError = "Failed to send packet: " + m_pImpl->networkManager->getLastError();
        LOG_ERROR(m_pImpl->lastError);
        
        if (m_pImpl->errorCallback) {
            m_pImpl->errorCallback(m_pImpl->lastError);
        }
        
        return false;
    }

    m_pImpl->totalPacketsSent++;

    if (m_pImpl->dataSentCallback) {
        m_pImpl->dataSentCallback(packet);
    }

    return true;
}

void VRDataStreamer::streamingThreadFunc() {
    LOG_INFO("Streaming thread started");

    StreamConfig config = m_pImpl->config.getStreamConfig();
    auto frameDuration = std::chrono::microseconds(1000000 / config.updateRate);
    auto nextFrameTime = std::chrono::high_resolution_clock::now();

    while (!m_pImpl->shouldStop) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        // Send one frame
        if (!sendFrame()) {
            // On error, wait a bit before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Calculate next frame time
        nextFrameTime += frameDuration;
        auto now = std::chrono::high_resolution_clock::now();

        // Sleep until next frame
        if (nextFrameTime > now) {
            std::this_thread::sleep_until(nextFrameTime);
        } else {
            // We're running behind, reset timing
            nextFrameTime = now;
        }
    }

    LOG_INFO("Streaming thread stopped");
}

StreamConfig VRDataStreamer::getConfig() const {
    return m_pImpl->config.getStreamConfig();
}

void VRDataStreamer::setConfig(const StreamConfig& config) {
    if (m_pImpl->isStreaming) {
        LOG_WARNING("Cannot change config while streaming");
        return;
    }
    m_pImpl->config.setStreamConfig(config);
}

std::vector<DeviceData> VRDataStreamer::getCurrentDevices() const {
    if (!m_pImpl->vrDevice) {
        return std::vector<DeviceData>();
    }
    return m_pImpl->vrDevice->getDevices();
}

ConnectionStatus VRDataStreamer::getConnectionStatus() const {
    if (!m_pImpl->networkManager) {
        return ConnectionStatus::Disconnected;
    }
    return m_pImpl->networkManager->getStatus();
}

void VRDataStreamer::setErrorCallback(ErrorCallback callback) {
    m_pImpl->errorCallback = callback;
}

void VRDataStreamer::setDataSentCallback(DataSentCallback callback) {
    m_pImpl->dataSentCallback = callback;
}

std::string VRDataStreamer::getLastError() const {
    return m_pImpl->lastError;
}

void VRDataStreamer::getStatistics(uint64_t& totalPacketsSent, double& packetsPerSecond, double& uptime) const {
    totalPacketsSent = m_pImpl->totalPacketsSent;
    
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - m_pImpl->startTime;
    uptime = elapsed.count();
    
    if (uptime > 0.0) {
        packetsPerSecond = static_cast<double>(totalPacketsSent) / uptime;
    } else {
        packetsPerSecond = 0.0;
    }
}

} // namespace AimlabVR

