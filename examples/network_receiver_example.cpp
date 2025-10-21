////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Network Receiver Example
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Example server application that receives VR data packets over the network.
//   Can be used to test the streaming functionality.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/NetworkManager.h"
#include "aimlab-vr-datastreamer/DataPacket.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <iostream>
#include <iomanip>
#include <csignal>
#include <atomic>

using namespace AimlabVR;

std::atomic<bool> g_running(true);

void signalHandler(int signal) {
    std::cout << "\nReceived interrupt signal, shutting down..." << std::endl;
    g_running = false;
}

void printPacketInfo(const DataPacket& packet) {
    std::cout << "\n--- Received Packet ---" << std::endl;
    std::cout << "Sequence Number: " << packet.getSequenceNumber() << std::endl;
    std::cout << "Timestamp: " << packet.getTimestamp() << std::endl;
    std::cout << "Device Count: " << packet.getDevices().size() << std::endl;
    std::cout << "Packet Size: " << packet.getSize() << " bytes" << std::endl;
    
    for (const auto& device : packet.getDevices()) {
        std::cout << "\n  Device " << device.deviceId << " (" << device.deviceName << "):" << std::endl;
        std::cout << "    Type: ";
        switch (device.deviceType) {
            case DeviceType::HeadMountedDisplay: std::cout << "HMD"; break;
            case DeviceType::LeftController: std::cout << "Left Controller"; break;
            case DeviceType::RightController: std::cout << "Right Controller"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << std::endl;
        
        std::cout << "    Position: (" << std::fixed << std::setprecision(3)
                  << device.pose.position.x << ", "
                  << device.pose.position.y << ", "
                  << device.pose.position.z << ")" << std::endl;
        
        std::cout << "    Rotation: (" << std::fixed << std::setprecision(3)
                  << device.pose.rotation.w << ", "
                  << device.pose.rotation.x << ", "
                  << device.pose.rotation.y << ", "
                  << device.pose.rotation.z << ")" << std::endl;
    }
    std::cout << "----------------------" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== AIMLAB VR Data Receiver Example ===" << std::endl;
    std::cout << "Author: Pi Ko (pi.ko@nyu.edu)" << std::endl;
    std::cout << "Date: 21 October 2025" << std::endl;
    std::cout << std::endl;

    // Setup signal handler
    std::signal(SIGINT, signalHandler);

    // Initialize logger
    Logger::getInstance().initialize(true, "receiver_example.log", LogLevel::Info);

    // Parse command line arguments
    std::string bindAddress = "0.0.0.0";
    uint16_t bindPort = 8888;
    
    if (argc > 1) {
        bindPort = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Bind Address: " << bindAddress << std::endl;
    std::cout << "  Port: " << bindPort << std::endl;
    std::cout << "  Protocol: UDP" << std::endl;
    std::cout << std::endl;

    // Note: This is a simplified receiver example
    // In a real implementation, you would need to bind a socket in server mode
    // For now, this demonstrates the packet reception API
    
    NetworkManager receiver;
    
    std::cout << "Initializing network receiver..." << std::endl;
    if (!receiver.initialize(bindAddress, bindPort, NetworkProtocol::UDP)) {
        std::cerr << "ERROR: Failed to initialize receiver: " << receiver.getLastError() << std::endl;
        return 1;
    }

    std::cout << "Receiver initialized successfully!" << std::endl;
    std::cout << "Waiting for packets... (Press Ctrl+C to stop)" << std::endl;
    std::cout << std::endl;

    uint64_t packetsReceived = 0;
    auto startTime = std::chrono::steady_clock::now();

    // Reception loop
    while (g_running) {
        DataPacket packet;
        
        // Try to receive a packet (with 1 second timeout)
        if (receiver.receive(packet, 1000)) {
            packetsReceived++;
            
            // Print summary every 10 packets
            if (packetsReceived % 10 == 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
                double rate = elapsed > 0 ? static_cast<double>(packetsReceived) / elapsed : 0.0;
                
                std::cout << "\rPackets received: " << packetsReceived 
                         << " | Rate: " << std::fixed << std::setprecision(2) << rate << " pps    " 
                         << std::flush;
            }
            
            // Print detailed info for first few packets
            if (packetsReceived <= 3) {
                printPacketInfo(packet);
            }
        }
    }

    // Final statistics
    auto endTime = std::chrono::steady_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    double avgRate = totalTime > 0 ? static_cast<double>(packetsReceived) / totalTime : 0.0;

    std::cout << "\n\n=== Reception Statistics ===" << std::endl;
    std::cout << "Total Packets Received: " << packetsReceived << std::endl;
    std::cout << "Average Reception Rate: " << std::fixed << std::setprecision(2) 
              << avgRate << " packets/second" << std::endl;
    std::cout << "Total Time: " << totalTime << " seconds" << std::endl;

    std::cout << "\nShutting down..." << std::endl;
    receiver.disconnect();
    Logger::getInstance().shutdown();

    std::cout << "Example completed successfully!" << std::endl;
    return 0;
}

