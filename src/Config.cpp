////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Configuration Manager Implementation
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Implementation of configuration manager for loading and saving settings.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/Config.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace AimlabVR {

Config::Config() {
    resetToDefaults();
}

bool Config::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: " + filePath);
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    // Simple JSON parsing (in production, use a proper JSON library)
    bool success = parseJSON(content);
    if (success) {
        LOG_INFO("Configuration loaded from: " + filePath);
    } else {
        LOG_ERROR("Failed to parse config file: " + filePath);
    }

    return success;
}

bool Config::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to create config file: " + filePath);
        return false;
    }

    std::string jsonContent = generateJSON();
    file << jsonContent;
    file.close();

    LOG_INFO("Configuration saved to: " + filePath);
    return true;
}

StreamConfig Config::getStreamConfig() const {
    return m_config;
}

void Config::setStreamConfig(const StreamConfig& config) {
    m_config = config;
}

bool Config::validate() const {
    if (m_config.serverAddress.empty()) {
        LOG_ERROR("Server address is empty");
        return false;
    }

    if (m_config.serverPort == 0) {
        LOG_ERROR("Server port is invalid");
        return false;
    }

    if (m_config.updateRate == 0 || m_config.updateRate > 1000) {
        LOG_ERROR("Update rate must be between 1 and 1000 Hz");
        return false;
    }

    return true;
}

void Config::resetToDefaults() {
    m_config.serverAddress = "127.0.0.1";
    m_config.serverPort = 8888;
    m_config.protocol = NetworkProtocol::UDP;
    m_config.updateRate = 90;  // 90 Hz default
    m_config.enableLogging = true;
    m_config.logLevel = LogLevel::Info;
    m_config.logFilePath = "aimlab_vr_streamer.log";
}

// Individual setters
void Config::setServerAddress(const std::string& address) {
    m_config.serverAddress = address;
}

void Config::setServerPort(uint16_t port) {
    m_config.serverPort = port;
}

void Config::setProtocol(NetworkProtocol protocol) {
    m_config.protocol = protocol;
}

void Config::setUpdateRate(uint32_t rate) {
    m_config.updateRate = rate;
}

void Config::setLoggingEnabled(bool enabled) {
    m_config.enableLogging = enabled;
}

void Config::setLogLevel(LogLevel level) {
    m_config.logLevel = level;
}

void Config::setLogFilePath(const std::string& path) {
    m_config.logFilePath = path;
}

// Individual getters
std::string Config::getServerAddress() const {
    return m_config.serverAddress;
}

uint16_t Config::getServerPort() const {
    return m_config.serverPort;
}

NetworkProtocol Config::getProtocol() const {
    return m_config.protocol;
}

uint32_t Config::getUpdateRate() const {
    return m_config.updateRate;
}

bool Config::isLoggingEnabled() const {
    return m_config.enableLogging;
}

LogLevel Config::getLogLevel() const {
    return m_config.logLevel;
}

std::string Config::getLogFilePath() const {
    return m_config.logFilePath;
}

// Simple JSON parsing (minimal implementation)
bool Config::parseJSON(const std::string& content) {
    // This is a simple parser for demonstration
    // In production, use a proper JSON library like nlohmann/json or RapidJSON
    
    auto findValue = [&](const std::string& key) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        
        pos = content.find(":", pos);
        if (pos == std::string::npos) return "";
        
        pos = content.find_first_not_of(" \t\n\r:", pos + 1);
        if (pos == std::string::npos) return "";
        
        size_t endPos;
        if (content[pos] == '"') {
            pos++;
            endPos = content.find('"', pos);
        } else {
            endPos = content.find_first_of(",}", pos);
        }
        
        return content.substr(pos, endPos - pos);
    };

    try {
        std::string val = findValue("serverAddress");
        if (!val.empty()) m_config.serverAddress = val;

        val = findValue("serverPort");
        if (!val.empty()) m_config.serverPort = static_cast<uint16_t>(std::stoi(val));

        val = findValue("updateRate");
        if (!val.empty()) m_config.updateRate = static_cast<uint32_t>(std::stoi(val));

        val = findValue("logFilePath");
        if (!val.empty()) m_config.logFilePath = val;

        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::generateJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"serverAddress\": \"" << m_config.serverAddress << "\",\n";
    oss << "  \"serverPort\": " << m_config.serverPort << ",\n";
    oss << "  \"protocol\": " << static_cast<int>(m_config.protocol) << ",\n";
    oss << "  \"updateRate\": " << m_config.updateRate << ",\n";
    oss << "  \"enableLogging\": " << (m_config.enableLogging ? "true" : "false") << ",\n";
    oss << "  \"logLevel\": " << static_cast<int>(m_config.logLevel) << ",\n";
    oss << "  \"logFilePath\": \"" << m_config.logFilePath << "\"\n";
    oss << "}\n";
    return oss.str();
}

} // namespace AimlabVR

