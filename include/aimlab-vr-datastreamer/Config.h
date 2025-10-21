////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Configuration Manager
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Configuration manager for loading and saving AIMLAB VR Data Streamer
//   settings from JSON or INI files. Provides default configurations and
//   validation.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_CONFIG_H
#define AIMLAB_VR_CONFIG_H

#include "Types.h"
#include <string>
#include <memory>

namespace AimlabVR {

/**
 * @class Config
 * @brief Configuration manager for VR data streaming system
 * 
 * Handles loading, saving, and validating configuration parameters
 * for the VR data streaming system.
 */
class Config {
public:
    /**
     * @brief Constructor with default configuration
     */
    Config();

    /**
     * @brief Load configuration from file
     * @param filePath Path to configuration file
     * @return True if successful, false otherwise
     */
    bool loadFromFile(const std::string& filePath);

    /**
     * @brief Save current configuration to file
     * @param filePath Path to save configuration
     * @return True if successful, false otherwise
     */
    bool saveToFile(const std::string& filePath) const;

    /**
     * @brief Get current stream configuration
     * @return StreamConfig structure
     */
    StreamConfig getStreamConfig() const;

    /**
     * @brief Set stream configuration
     * @param config New configuration
     */
    void setStreamConfig(const StreamConfig& config);

    /**
     * @brief Validate current configuration
     * @return True if valid, false otherwise
     */
    bool validate() const;

    /**
     * @brief Reset to default configuration
     */
    void resetToDefaults();

    // Individual parameter getters/setters
    void setServerAddress(const std::string& address);
    void setServerPort(uint16_t port);
    void setProtocol(NetworkProtocol protocol);
    void setUpdateRate(uint32_t rate);
    void setLoggingEnabled(bool enabled);
    void setLogLevel(LogLevel level);
    void setLogFilePath(const std::string& path);

    std::string getServerAddress() const;
    uint16_t getServerPort() const;
    NetworkProtocol getProtocol() const;
    uint32_t getUpdateRate() const;
    bool isLoggingEnabled() const;
    LogLevel getLogLevel() const;
    std::string getLogFilePath() const;

private:
    /**
     * @brief Parse JSON configuration file
     */
    bool parseJSON(const std::string& content);

    /**
     * @brief Generate JSON configuration string
     */
    std::string generateJSON() const;

    StreamConfig m_config;
};

} // namespace AimlabVR

#endif // AIMLAB_VR_CONFIG_H

