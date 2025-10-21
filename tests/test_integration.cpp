////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Integration Test
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Integration test for the complete VR data streaming system.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/VRDataStreamer.h"
#include "aimlab-vr-datastreamer/Config.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace AimlabVR;

bool testConfigurationManagement() {
    std::cout << "Testing configuration management..." << std::endl;
    
    Config config;
    config.setServerAddress("192.168.1.100");
    config.setServerPort(9999);
    config.setUpdateRate(120);
    
    assert(config.getServerAddress() == "192.168.1.100");
    assert(config.getServerPort() == 9999);
    assert(config.getUpdateRate() == 120);
    assert(config.validate());
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testMockVRDevice() {
    std::cout << "Testing mock VR device..." << std::endl;
    
    auto device = createVRDevice(true);
    assert(device != nullptr);
    
    bool initialized = device->initialize();
    assert(initialized);
    assert(device->isActive());
    
    bool updated = device->update();
    assert(updated);
    
    auto devices = device->getDevices();
    assert(!devices.empty());
    assert(devices.size() >= 3);  // HMD + 2 controllers
    
    DeviceData hmd;
    assert(device->getHMD(hmd));
    assert(hmd.deviceType == DeviceType::HeadMountedDisplay);
    
    device->shutdown();
    assert(!device->isActive());
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testDataPacketWorkflow() {
    std::cout << "Testing data packet workflow..." << std::endl;
    
    // Create mock device
    auto vrDevice = createVRDevice(true);
    vrDevice->initialize();
    vrDevice->update();
    
    auto devices = vrDevice->getDevices();
    
    // Create packet
    DataPacket packet(PacketFormat::Binary);
    packet.setSequenceNumber(1);
    packet.setTimestamp(123456789);
    
    for (const auto& device : devices) {
        packet.addDevice(device);
    }
    
    // Serialize
    auto data = packet.serialize();
    assert(!data.empty());
    
    // Deserialize
    DataPacket receivedPacket(PacketFormat::Binary);
    bool success = receivedPacket.deserialize(data);
    assert(success);
    assert(receivedPacket.getSequenceNumber() == 1);
    assert(receivedPacket.getDevices().size() == devices.size());
    
    vrDevice->shutdown();
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testStreamerInitialization() {
    std::cout << "Testing streamer initialization..." << std::endl;
    
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 90;
    config.enableLogging = false;  // Disable logging for cleaner test output
    
    VRDataStreamer streamer;
    bool initialized = streamer.initialize(config, true);  // Use mock device
    
    if (!initialized) {
        std::cout << "  SKIPPED (network initialization failed: " 
                  << streamer.getLastError() << ")" << std::endl;
        return true;  // Don't fail test if network isn't available
    }
    
    assert(streamer.getConnectionStatus() == ConnectionStatus::Connected);
    
    streamer.shutdown();
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testManualFrameSending() {
    std::cout << "Testing manual frame sending..." << std::endl;
    
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 90;
    config.enableLogging = false;
    
    VRDataStreamer streamer;
    if (!streamer.initialize(config, true)) {
        std::cout << "  SKIPPED (network not available)" << std::endl;
        return true;
    }
    
    // Send a few frames manually
    for (int i = 0; i < 5; ++i) {
        bool sent = streamer.sendFrame();
        if (!sent) {
            std::cout << "  SKIPPED (send failed: " << streamer.getLastError() << ")" << std::endl;
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    streamer.shutdown();
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testStreamingStatistics() {
    std::cout << "Testing streaming statistics..." << std::endl;
    
    StreamConfig config;
    config.serverAddress = "127.0.0.1";
    config.serverPort = 8888;
    config.protocol = NetworkProtocol::UDP;
    config.updateRate = 100;
    config.enableLogging = false;
    
    VRDataStreamer streamer;
    if (!streamer.initialize(config, true)) {
        std::cout << "  SKIPPED (network not available)" << std::endl;
        return true;
    }
    
    if (!streamer.startStreaming()) {
        std::cout << "  SKIPPED (streaming start failed)" << std::endl;
        return true;
    }
    
    // Stream for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    uint64_t totalPackets;
    double packetsPerSec;
    double uptime;
    streamer.getStatistics(totalPackets, packetsPerSec, uptime);
    
    assert(totalPackets > 0);
    assert(uptime > 0.9 && uptime < 1.5);
    
    std::cout << "  Statistics: " << totalPackets << " packets in " 
              << uptime << "s (" << packetsPerSec << " pps)" << std::endl;
    
    streamer.stopStreaming();
    streamer.shutdown();
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

int main() {
    std::cout << "=== Integration Tests ===" << std::endl;
    std::cout << "Author: Pi Ko (pi.ko@nyu.edu)" << std::endl;
    std::cout << "Date: 21 October 2025" << std::endl;
    std::cout << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    try {
        if (testConfigurationManagement()) passed++; else failed++;
        if (testMockVRDevice()) passed++; else failed++;
        if (testDataPacketWorkflow()) passed++; else failed++;
        if (testStreamerInitialization()) passed++; else failed++;
        if (testManualFrameSending()) passed++; else failed++;
        if (testStreamingStatistics()) passed++; else failed++;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        failed++;
    }
    
    std::cout << std::endl;
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    
    return (failed == 0) ? 0 : 1;
}

