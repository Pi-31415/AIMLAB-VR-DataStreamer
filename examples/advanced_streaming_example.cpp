////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Advanced Streaming Example
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Advanced example demonstrating configuration loading, device monitoring,
//   and detailed streaming statistics.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/VRDataStreamer.h"
#include "aimlab-vr-datastreamer/Config.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace AimlabVR;

void printDeviceInfo(const std::vector<DeviceData>& devices) {
    std::cout << "\n--- VR Device Information ---" << std::endl;
    for (const auto& device : devices) {
        std::cout << "Device ID: " << device.deviceId << std::endl;
        std::cout << "  Name: " << device.deviceName << std::endl;
        std::cout << "  Type: ";
        switch (device.deviceType) {
            case DeviceType::HeadMountedDisplay: std::cout << "HMD"; break;
            case DeviceType::LeftController: std::cout << "Left Controller"; break;
            case DeviceType::RightController: std::cout << "Right Controller"; break;
            case DeviceType::Tracker: std::cout << "Tracker"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << std::endl;
        
        std::cout << "  Position: (" << std::fixed << std::setprecision(3)
                  << device.pose.position.x << ", "
                  << device.pose.position.y << ", "
                  << device.pose.position.z << ")" << std::endl;
        
        std::cout << "  Rotation: (" << std::fixed << std::setprecision(3)
                  << device.pose.rotation.w << ", "
                  << device.pose.rotation.x << ", "
                  << device.pose.rotation.y << ", "
                  << device.pose.rotation.z << ")" << std::endl;
        
        if (device.deviceType == DeviceType::LeftController || 
            device.deviceType == DeviceType::RightController) {
            std::cout << "  Trigger: " << (device.buttons.trigger ? "PRESSED" : "released")
                     << " (" << device.buttons.triggerValue << ")" << std::endl;
            std::cout << "  Grip: " << (device.buttons.grip ? "PRESSED" : "released")
                     << " (" << device.buttons.gripValue << ")" << std::endl;
        }
    }
    std::cout << "-----------------------------\n" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== AIMLAB VR Data Streamer - Advanced Example ===" << std::endl;
    std::cout << "Author: Pi Ko (pi.ko@nyu.edu)" << std::endl;
    std::cout << "Date: 21 October 2025" << std::endl;
    std::cout << std::endl;

    // Create and save a sample configuration
    Config config;
    config.setServerAddress("127.0.0.1");
    config.setServerPort(8888);
    config.setProtocol(NetworkProtocol::UDP);
    config.setUpdateRate(90);
    config.setLoggingEnabled(true);
    config.setLogLevel(LogLevel::Info);
    config.setLogFilePath("advanced_example.log");

    std::string configFile = "../config/streamer_config.json";
    std::cout << "Saving configuration to: " << configFile << std::endl;
    if (!config.saveToFile(configFile)) {
        std::cout << "Warning: Could not save config file" << std::endl;
    }

    // Create VR data streamer
    VRDataStreamer streamer;

    // Initialize (try loading config file, fallback to defaults)
    std::cout << "Initializing VR Data Streamer..." << std::endl;
    if (!streamer.initialize(configFile, true)) {  // true = use mock device
        std::cerr << "ERROR: Failed to initialize: " << streamer.getLastError() << std::endl;
        return 1;
    }
    std::cout << "Initialization successful!" << std::endl;

    // Display current devices
    auto devices = streamer.getCurrentDevices();
    printDeviceInfo(devices);

    // Set error callback
    streamer.setErrorCallback([](const std::string& error) {
        std::cerr << "ERROR: " << error << std::endl;
    });

    // Set packet sent callback with detailed logging
    uint64_t packetsSent = 0;
    auto lastReportTime = std::chrono::steady_clock::now();
    
    streamer.setDataSentCallback([&](const DataPacket& packet) {
        packetsSent++;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastReportTime);
        
        if (elapsed.count() >= 2) {  // Report every 2 seconds
            std::cout << "\r[" << std::chrono::system_clock::now().time_since_epoch().count() / 1000000000
                     << "] Packets: " << packetsSent 
                     << " | Seq: " << packet.getSequenceNumber()
                     << " | Devices: " << packet.getDevices().size()
                     << " | Size: " << packet.getSize() << " bytes    " << std::flush;
            lastReportTime = now;
        }
    });

    // Start streaming
    std::cout << "Starting VR data streaming..." << std::endl;
    if (!streamer.startStreaming()) {
        std::cerr << "ERROR: Failed to start streaming: " << streamer.getLastError() << std::endl;
        return 1;
    }
    std::cout << "Streaming started! (will run for 15 seconds)" << std::endl;
    std::cout << std::endl;

    // Monitor streaming for 15 seconds
    auto startTime = std::chrono::steady_clock::now();
    int lastSecond = -1;

    while (true) {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        
        if (seconds >= 15) {
            break;
        }

        // Print device state every 5 seconds
        if (seconds % 5 == 0 && seconds != lastSecond) {
            devices = streamer.getCurrentDevices();
            std::cout << std::endl;
            printDeviceInfo(devices);
            lastSecond = seconds;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Final statistics
    uint64_t totalPackets;
    double packetsPerSec;
    double uptime;
    streamer.getStatistics(totalPackets, packetsPerSec, uptime);

    std::cout << "\n\n=== Final Streaming Statistics ===" << std::endl;
    std::cout << "Total Packets Sent: " << totalPackets << std::endl;
    std::cout << "Average Packets/Second: " << std::fixed << std::setprecision(2) 
              << packetsPerSec << std::endl;
    std::cout << "Total Uptime: " << uptime << " seconds" << std::endl;
    std::cout << "Connection Status: " 
              << (streamer.getConnectionStatus() == ConnectionStatus::Connected ? "Connected" : "Disconnected")
              << std::endl;

    // Stop and shutdown
    std::cout << "\nStopping streaming..." << std::endl;
    streamer.stopStreaming();
    
    std::cout << "Shutting down..." << std::endl;
    streamer.shutdown();

    std::cout << "Example completed successfully!" << std::endl;
    return 0;
}

