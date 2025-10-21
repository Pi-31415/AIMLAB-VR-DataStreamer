////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Logger Implementation
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Implementation of thread-safe logging system.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace AimlabVR {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_minLevel(LogLevel::Info)
    , m_logToFile(false)
    , m_initialized(false) {
}

Logger::~Logger() {
    shutdown();
}

void Logger::initialize(bool logToFile, const std::string& logFilePath, LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized) {
        shutdown();
    }

    m_logToFile = logToFile;
    m_minLevel = minLevel;

    if (m_logToFile && !logFilePath.empty()) {
        m_logFile.open(logFilePath, std::ios::out | std::ios::app);
        if (!m_logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath << std::endl;
            m_logToFile = false;
        }
    }

    m_initialized = true;
    info("Logger initialized", __FUNCTION__, __LINE__);
}

void Logger::log(LogLevel level, const std::string& message, const char* function, int line) {
    if (level < m_minLevel || !m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] "
        << "[" << logLevelToString(level) << "] ";
    
    if (function && function[0] != '\0') {
        oss << "[" << function << ":" << line << "] ";
    }
    
    oss << message;

    std::string logMessage = oss.str();

    // Output to console
    if (level >= LogLevel::Error) {
        std::cerr << logMessage << std::endl;
    } else {
        std::cout << logMessage << std::endl;
    }

    // Output to file
    if (m_logToFile && m_logFile.is_open()) {
        m_logFile << logMessage << std::endl;
    }
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLevel = level;
}

void Logger::flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.flush();
    }
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    m_initialized = false;
}

void Logger::debug(const std::string& message, const char* function, int line) {
    log(LogLevel::Debug, message, function, line);
}

void Logger::info(const std::string& message, const char* function, int line) {
    log(LogLevel::Info, message, function, line);
}

void Logger::warning(const std::string& message, const char* function, int line) {
    log(LogLevel::Warning, message, function, line);
}

void Logger::error(const std::string& message, const char* function, int line) {
    log(LogLevel::Error, message, function, line);
}

void Logger::critical(const std::string& message, const char* function, int line) {
    log(LogLevel::Critical, message, function, line);
}

std::string Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARNING";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &now_c);
#else
    localtime_r(&now_c, &tm_buf);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

} // namespace AimlabVR

