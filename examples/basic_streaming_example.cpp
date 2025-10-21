////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Basic Streaming Example
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Simple example demonstrating basic VR data streaming functionality.
//   Uses mock VR device and streams data to a local server.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/VRDataStreamer.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace AimlabVR;

int main(int argc, char* argv[]) {
    std::cout << "=== AIMLAB VR Data Streamer - Basic Example ===" << std::endl;
    std::cout << "Author: Pi Ko (pi.ko@nyu.edu)" << std::endl;
    std::cout << "Date: 21 October 2025" << std::endl;
    std::cout << std::endl;

    // Create configuration
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 90;  // 90 Hz
    config.enableLogging = true;
    config.logLevel = LogLevel::Info;
    config.logFilePath = "basic_example.log";

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Server: " << config.serverAddress << ":" << config.serverPort << std::endl;
    std::cout << "  Protocol: UDP" << std::endl;
    std::cout << "  Update Rate: " << config.updateRate << " Hz" << std::endl;
    std::cout << std::endl;

    // Create VR data streamer
    VRDataStreamer streamer;

    // Initialize with mock VR device
    std::cout << "Initializing VR Data Streamer..." << std::endl;
    if (!streamer.initialize(config, true)) {  // true = use mock device
        std::cerr << "ERROR: Failed to initialize: " << streamer.getLastError() << std::endl;
        return 1;
    }
    std::cout << "Initialization successful!" << std::endl;
    std::cout << std::endl;

    // Set callbacks
    streamer.setErrorCallback([](const std::string& error) {
        std::cerr << "Streaming Error: " << error << std::endl;
    });

    uint64_t packetCount = 0;
    streamer.setDataSentCallback([&packetCount](const DataPacket& packet) {
        packetCount++;
        if (packetCount % 90 == 0) {  // Print every second at 90 Hz
            std::cout << "Packets sent: " << packetCount 
                     << " | Devices: " << packet.getDevices().size() << std::endl;
        }
    });

    // Start streaming
    std::cout << "Starting VR data streaming..." << std::endl;
    if (!streamer.startStreaming()) {
        std::cerr << "ERROR: Failed to start streaming: " << streamer.getLastError() << std::endl;
        return 1;
    }
    std::cout << "Streaming started! Press Ctrl+C or wait 10 seconds to stop." << std::endl;
    std::cout << std::endl;

    // Stream for 10 seconds
    auto startTime = std::chrono::steady_clock::now();
    while (true) {
        auto elapsed = std::chrono::steady_clock::now() - startTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
        
        if (seconds >= 10) {
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Get statistics
    uint64_t totalPackets;
    double packetsPerSec;
    double uptime;
    streamer.getStatistics(totalPackets, packetsPerSec, uptime);

    std::cout << std::endl;
    std::cout << "=== Streaming Statistics ===" << std::endl;
    std::cout << "Total Packets Sent: " << totalPackets << std::endl;
    std::cout << "Packets/Second: " << packetsPerSec << std::endl;
    std::cout << "Uptime: " << uptime << " seconds" << std::endl;

    // Stop streaming
    std::cout << std::endl;
    std::cout << "Stopping streaming..." << std::endl;
    streamer.stopStreaming();

    // Shutdown
    std::cout << "Shutting down..." << std::endl;
    streamer.shutdown();

    std::cout << "Example completed successfully!" << std::endl;
    return 0;
}

