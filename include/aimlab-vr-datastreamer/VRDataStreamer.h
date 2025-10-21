////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Main VR Data Streamer
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Main facade class for AIMLAB VR Data Streamer. Provides high-level API
//   for initializing, configuring, and streaming VR tracking data over network.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_DATASTREAMER_H
#define AIMLAB_VR_DATASTREAMER_H

#include "Types.h"
#include "Config.h"
#include "VRDevice.h"
#include "NetworkManager.h"
#include "DataPacket.h"
#include <memory>
#include <string>
#include <functional>

namespace AimlabVR {

/**
 * @class VRDataStreamer
 * @brief Main facade for VR data streaming system
 * 
 * High-level interface that coordinates VR device tracking, data packetization,
 * and network transmission. Provides simple API for streaming VR data.
 */
class VRDataStreamer {
public:
    /**
     * @brief Callback function for streaming errors
     * @param errorMessage Error description
     */
    using ErrorCallback = std::function<void(const std::string& errorMessage)>;

    /**
     * @brief Callback function for data sent events
     * @param packet Data packet that was sent
     */
    using DataSentCallback = std::function<void(const DataPacket& packet)>;

    /**
     * @brief Constructor
     */
    VRDataStreamer();

    /**
     * @brief Destructor
     */
    ~VRDataStreamer();

    /**
     * @brief Initialize with configuration file
     * @param configPath Path to configuration file
     * @param useMockDevice Use mock VR device for testing
     * @return True if successful, false otherwise
     */
    bool initialize(const std::string& configPath = "", bool useMockDevice = false);

    /**
     * @brief Initialize with configuration object
     * @param config Configuration object
     * @param useMockDevice Use mock VR device for testing
     * @return True if successful, false otherwise
     */
    bool initialize(const StreamConfig& config, bool useMockDevice = false);

    /**
     * @brief Shutdown streaming system
     */
    void shutdown();

    /**
     * @brief Start streaming VR data
     * @return True if successful, false otherwise
     */
    bool startStreaming();

    /**
     * @brief Stop streaming VR data
     */
    void stopStreaming();

    /**
     * @brief Check if currently streaming
     * @return True if streaming, false otherwise
     */
    bool isStreaming() const;

    /**
     * @brief Update and send one frame of VR data (manual mode)
     * @return True if successful, false otherwise
     */
    bool sendFrame();

    /**
     * @brief Get current configuration
     * @return Stream configuration
     */
    StreamConfig getConfig() const;

    /**
     * @brief Update configuration (requires restart if streaming)
     * @param config New configuration
     */
    void setConfig(const StreamConfig& config);

    /**
     * @brief Get current VR device data
     * @return Vector of device data
     */
    std::vector<DeviceData> getCurrentDevices() const;

    /**
     * @brief Get connection status
     * @return Current connection status
     */
    ConnectionStatus getConnectionStatus() const;

    /**
     * @brief Set error callback
     * @param callback Error callback function
     */
    void setErrorCallback(ErrorCallback callback);

    /**
     * @brief Set data sent callback
     * @param callback Data sent callback function
     */
    void setDataSentCallback(DataSentCallback callback);

    /**
     * @brief Get last error message
     * @return Error message string
     */
    std::string getLastError() const;

    /**
     * @brief Get streaming statistics
     * @param totalPacketsSent Output total packets sent
     * @param packetsPerSecond Output current packets per second
     * @param uptime Output streaming uptime in seconds
     */
    void getStatistics(uint64_t& totalPacketsSent, double& packetsPerSecond, double& uptime) const;

private:
    /**
     * @brief Streaming thread function
     */
    void streamingThreadFunc();

    /**
     * @brief Initialize logger
     */
    void initializeLogger();

    class Impl;
    std::unique_ptr<Impl> m_pImpl;  // PIMPL idiom
};

} // namespace AimlabVR

#endif // AIMLAB_VR_DATASTREAMER_H

