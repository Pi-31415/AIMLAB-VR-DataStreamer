////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Logger Unit Test
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Unit tests for Logger functionality.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/Logger.h"
#include <iostream>
#include <fstream>

using namespace AimlabVR;

bool testLoggerInitialization() {
    std::cout << "Testing logger initialization..." << std::endl;
    
    Logger::getInstance().initialize(true, "test_log.log", LogLevel::Debug);
    LOG_INFO("Logger initialized for testing");
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testLogLevels() {
    std::cout << "Testing different log levels..." << std::endl;
    
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARNING("This is a warning message");
    LOG_ERROR("This is an error message");
    LOG_CRITICAL("This is a critical message");
    
    std::cout << "  PASSED (check log file for messages)" << std::endl;
    return true;
}

bool testLogLevelFiltering() {
    std::cout << "Testing log level filtering..." << std::endl;
    
    Logger::getInstance().setLogLevel(LogLevel::Warning);
    LOG_DEBUG("This debug should NOT appear");
    LOG_INFO("This info should NOT appear");
    LOG_WARNING("This warning SHOULD appear");
    LOG_ERROR("This error SHOULD appear");
    
    Logger::getInstance().setLogLevel(LogLevel::Debug);
    
    std::cout << "  PASSED (check log file to verify filtering)" << std::endl;
    return true;
}

bool testLoggerFlush() {
    std::cout << "Testing logger flush..." << std::endl;
    
    LOG_INFO("Message before flush");
    Logger::getInstance().flush();
    LOG_INFO("Message after flush");
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testLogFileCreation() {
    std::cout << "Testing log file creation..." << std::endl;
    
    // Check if log file exists
    std::ifstream logFile("test_log.log");
    bool exists = logFile.good();
    logFile.close();
    
    if (!exists) {
        std::cerr << "  FAILED: Log file was not created" << std::endl;
        return false;
    }
    
    std::cout << "  PASSED (log file exists)" << std::endl;
    return true;
}

int main() {
    std::cout << "=== Logger Unit Tests ===" << std::endl;
    std::cout << "Author: Pi Ko (pi.ko@nyu.edu)" << std::endl;
    std::cout << "Date: 21 October 2025" << std::endl;
    std::cout << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    try {
        if (testLoggerInitialization()) passed++; else failed++;
        if (testLogLevels()) passed++; else failed++;
        if (testLogLevelFiltering()) passed++; else failed++;
        if (testLoggerFlush()) passed++; else failed++;
        if (testLogFileCreation()) passed++; else failed++;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        failed++;
    }
    
    Logger::getInstance().shutdown();
    
    std::cout << std::endl;
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "\nCheck 'test_log.log' for detailed log output" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}

