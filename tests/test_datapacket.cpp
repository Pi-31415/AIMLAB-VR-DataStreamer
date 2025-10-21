////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - DataPacket Unit Test
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Unit tests for DataPacket serialization and deserialization.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/DataPacket.h"
#include <iostream>
#include <cassert>

using namespace AimlabVR;

bool testBasicPacket() {
    std::cout << "Testing basic packet creation..." << std::endl;
    
    DataPacket packet;
    packet.setSequenceNumber(42);
    packet.setTimestamp(123456789);
    
    assert(packet.getSequenceNumber() == 42);
    assert(packet.getTimestamp() == 123456789);
    assert(packet.getDevices().empty());
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testAddDevice() {
    std::cout << "Testing adding devices..." << std::endl;
    
    DataPacket packet;
    
    DeviceData hmd;
    hmd.deviceId = 0;
    hmd.deviceType = DeviceType::HeadMountedDisplay;
    hmd.deviceName = "Test HMD";
    hmd.pose.position = Vector3(0.0f, 1.7f, 0.0f);
    hmd.pose.rotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    
    packet.addDevice(hmd);
    
    assert(packet.getDevices().size() == 1);
    assert(packet.getDevices()[0].deviceId == 0);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testSerialization() {
    std::cout << "Testing binary serialization..." << std::endl;
    
    DataPacket packet(PacketFormat::Binary);
    packet.setSequenceNumber(100);
    packet.setTimestamp(999888777);
    
    DeviceData device;
    device.deviceId = 1;
    device.deviceType = DeviceType::LeftController;
    device.pose.position = Vector3(1.0f, 2.0f, 3.0f);
    device.pose.rotation = Quaternion(0.707f, 0.0f, 0.707f, 0.0f);
    device.buttons.trigger = true;
    device.buttons.triggerValue = 0.8f;
    
    packet.addDevice(device);
    
    std::vector<uint8_t> data = packet.serialize();
    assert(!data.empty());
    assert(data.size() > 20);  // Should have header + device data
    
    std::cout << "  Serialized size: " << data.size() << " bytes" << std::endl;
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testDeserialization() {
    std::cout << "Testing binary deserialization..." << std::endl;
    
    // Create and serialize a packet
    DataPacket originalPacket(PacketFormat::Binary);
    originalPacket.setSequenceNumber(42);
    originalPacket.setTimestamp(123456789);
    
    DeviceData device;
    device.deviceId = 5;
    device.deviceType = DeviceType::RightController;
    device.pose.position = Vector3(1.5f, 1.2f, -0.3f);
    device.pose.rotation = Quaternion(0.5f, 0.5f, 0.5f, 0.5f);
    
    originalPacket.addDevice(device);
    
    std::vector<uint8_t> data = originalPacket.serialize();
    
    // Deserialize into new packet
    DataPacket deserializedPacket(PacketFormat::Binary);
    bool success = deserializedPacket.deserialize(data);
    
    assert(success);
    assert(deserializedPacket.getSequenceNumber() == 42);
    assert(deserializedPacket.getTimestamp() == 123456789);
    assert(deserializedPacket.getDevices().size() == 1);
    assert(deserializedPacket.getDevices()[0].deviceId == 5);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

bool testClear() {
    std::cout << "Testing packet clear..." << std::endl;
    
    DataPacket packet;
    packet.setSequenceNumber(10);
    
    DeviceData device;
    device.deviceId = 1;
    packet.addDevice(device);
    
    assert(packet.getDevices().size() == 1);
    
    packet.clear();
    
    assert(packet.getDevices().empty());
    assert(packet.getTimestamp() == 0);
    
    std::cout << "  PASSED" << std::endl;
    return true;
}

int main() {
    std::cout << "=== DataPacket Unit Tests ===" << std::endl;
    std::cout << "Author: Pi Ko (pi.ko@nyu.edu)" << std::endl;
    std::cout << "Date: 21 October 2025" << std::endl;
    std::cout << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    try {
        if (testBasicPacket()) passed++; else failed++;
        if (testAddDevice()) passed++; else failed++;
        if (testSerialization()) passed++; else failed++;
        if (testDeserialization()) passed++; else failed++;
        if (testClear()) passed++; else failed++;
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

