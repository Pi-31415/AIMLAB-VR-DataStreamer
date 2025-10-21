////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Logger
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Thread-safe logging system for AIMLAB VR Data Streamer.
//   Supports multiple log levels, file and console output, and timestamp
//   formatting.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_LOGGER_H
#define AIMLAB_VR_LOGGER_H

#include "Types.h"
#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace AimlabVR {

/**
 * @class Logger
 * @brief Thread-safe logging system for VR data streaming operations
 * 
 * Provides formatted logging with multiple severity levels. Supports both
 * console and file output with thread-safe operations.
 */
class Logger {
public:
    /**
     * @brief Get singleton instance of Logger
     * @return Reference to the Logger instance
     */
    static Logger& getInstance();

    /**
     * @brief Initialize the logger with configuration
     * @param logToFile Enable file logging
     * @param logFilePath Path to log file
     * @param minLevel Minimum log level to record
     */
    void initialize(bool logToFile, const std::string& logFilePath, LogLevel minLevel);

    /**
     * @brief Log a message at the specified level
     * @param level Log level
     * @param message Message to log
     * @param function Function name where log was called
     * @param line Line number where log was called
     */
    void log(LogLevel level, const std::string& message, const char* function, int line);

    /**
     * @brief Set minimum log level
     * @param level New minimum log level
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief Flush log buffers to disk
     */
    void flush();

    /**
     * @brief Shutdown the logger
     */
    void shutdown();

    // Convenience methods
    void debug(const std::string& message, const char* function = "", int line = 0);
    void info(const std::string& message, const char* function = "", int line = 0);
    void warning(const std::string& message, const char* function = "", int line = 0);
    void error(const std::string& message, const char* function = "", int line = 0);
    void critical(const std::string& message, const char* function = "", int line = 0);

private:
    Logger();
    ~Logger();
    
    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Convert log level to string
     */
    std::string logLevelToString(LogLevel level) const;

    /**
     * @brief Get current timestamp as string
     */
    std::string getCurrentTimestamp() const;

    std::mutex m_mutex;
    std::ofstream m_logFile;
    LogLevel m_minLevel;
    bool m_logToFile;
    bool m_initialized;
};

// Convenience macros for logging with automatic function and line info
#define LOG_DEBUG(msg) AimlabVR::Logger::getInstance().debug(msg, __FUNCTION__, __LINE__)
#define LOG_INFO(msg) AimlabVR::Logger::getInstance().info(msg, __FUNCTION__, __LINE__)
#define LOG_WARNING(msg) AimlabVR::Logger::getInstance().warning(msg, __FUNCTION__, __LINE__)
#define LOG_ERROR(msg) AimlabVR::Logger::getInstance().error(msg, __FUNCTION__, __LINE__)
#define LOG_CRITICAL(msg) AimlabVR::Logger::getInstance().critical(msg, __FUNCTION__, __LINE__)

} // namespace AimlabVR

#endif // AIMLAB_VR_LOGGER_H

