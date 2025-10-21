////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - VR Device Implementation
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Implementation of VR device interface and mock device for testing.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/VRDevice.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <cmath>
#include <chrono>

namespace AimlabVR {

// MockVRDevice Implementation

MockVRDevice::MockVRDevice()
    : m_isActive(false)
    , m_frameCount(0) {
}

bool MockVRDevice::initialize() {
    LOG_INFO("Initializing MockVRDevice");
    
    m_mockDevices.clear();
    m_frameCount = 0;
    
    // Create mock devices
    DeviceData hmd;
    hmd.deviceId = 0;
    hmd.deviceType = DeviceType::HeadMountedDisplay;
    hmd.deviceName = "Mock HMD";
    hmd.isConnected = true;
    hmd.trackingState = TrackingState::Tracked;
    m_mockDevices.push_back(hmd);

    DeviceData leftController;
    leftController.deviceId = 1;
    leftController.deviceType = DeviceType::LeftController;
    leftController.deviceName = "Mock Left Controller";
    leftController.isConnected = true;
    leftController.trackingState = TrackingState::Tracked;
    m_mockDevices.push_back(leftController);

    DeviceData rightController;
    rightController.deviceId = 2;
    rightController.deviceType = DeviceType::RightController;
    rightController.deviceName = "Mock Right Controller";
    rightController.isConnected = true;
    rightController.trackingState = TrackingState::Tracked;
    m_mockDevices.push_back(rightController);

    m_isActive = true;
    LOG_INFO("MockVRDevice initialized with " + std::to_string(m_mockDevices.size()) + " devices");
    
    return true;
}

void MockVRDevice::shutdown() {
    LOG_INFO("Shutting down MockVRDevice");
    m_isActive = false;
    m_mockDevices.clear();
}

bool MockVRDevice::update() {
    if (!m_isActive) {
        return false;
    }

    generateMockData();
    m_frameCount++;
    
    return true;
}

void MockVRDevice::generateMockData() {
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();

    float time = m_frameCount * 0.016f;  // Simulate 60 FPS

    for (auto& device : m_mockDevices) {
        device.pose.timestamp = timestamp;

        switch (device.deviceType) {
            case DeviceType::HeadMountedDisplay:
                // Simulate slight head movement
                device.pose.position.x = 0.0f;
                device.pose.position.y = 1.7f + std::sin(time * 0.5f) * 0.05f;
                device.pose.position.z = 0.0f;
                device.pose.rotation.w = std::cos(std::sin(time * 0.3f) * 0.1f);
                device.pose.rotation.x = std::sin(std::sin(time * 0.3f) * 0.1f);
                device.pose.rotation.y = 0.0f;
                device.pose.rotation.z = 0.0f;
                break;

            case DeviceType::LeftController:
                // Simulate controller movement
                device.pose.position.x = -0.3f + std::sin(time) * 0.1f;
                device.pose.position.y = 1.2f + std::cos(time * 1.5f) * 0.1f;
                device.pose.position.z = -0.3f;
                device.pose.rotation.w = 0.707f;
                device.pose.rotation.x = 0.0f;
                device.pose.rotation.y = 0.707f;
                device.pose.rotation.z = 0.0f;
                
                // Simulate button presses
                device.buttons.trigger = (static_cast<int>(m_frameCount / 60) % 2) == 0;
                device.buttons.triggerValue = device.buttons.trigger ? 1.0f : 0.0f;
                device.buttons.grip = false;
                device.buttons.gripValue = 0.0f;
                break;

            case DeviceType::RightController:
                // Simulate controller movement
                device.pose.position.x = 0.3f + std::sin(time + 1.5f) * 0.1f;
                device.pose.position.y = 1.2f + std::cos(time * 1.5f + 1.0f) * 0.1f;
                device.pose.position.z = -0.3f;
                device.pose.rotation.w = 0.707f;
                device.pose.rotation.x = 0.0f;
                device.pose.rotation.y = -0.707f;
                device.pose.rotation.z = 0.0f;
                
                device.buttons.grip = (static_cast<int>(m_frameCount / 90) % 2) == 0;
                device.buttons.gripValue = device.buttons.grip ? 1.0f : 0.0f;
                device.buttons.trigger = false;
                device.buttons.triggerValue = 0.0f;
                break;

            default:
                break;
        }

        device.trackingState = TrackingState::Tracked;
    }
}

std::vector<DeviceData> MockVRDevice::getDevices() const {
    return m_mockDevices;
}

bool MockVRDevice::getDevice(uint32_t deviceId, DeviceData& deviceData) const {
    for (const auto& device : m_mockDevices) {
        if (device.deviceId == deviceId) {
            deviceData = device;
            return true;
        }
    }
    return false;
}

bool MockVRDevice::getHMD(DeviceData& deviceData) const {
    for (const auto& device : m_mockDevices) {
        if (device.deviceType == DeviceType::HeadMountedDisplay) {
            deviceData = device;
            return true;
        }
    }
    return false;
}

bool MockVRDevice::getLeftController(DeviceData& deviceData) const {
    for (const auto& device : m_mockDevices) {
        if (device.deviceType == DeviceType::LeftController) {
            deviceData = device;
            return true;
        }
    }
    return false;
}

bool MockVRDevice::getRightController(DeviceData& deviceData) const {
    for (const auto& device : m_mockDevices) {
        if (device.deviceType == DeviceType::RightController) {
            deviceData = device;
            return true;
        }
    }
    return false;
}

bool MockVRDevice::isActive() const {
    return m_isActive;
}

std::string MockVRDevice::getSystemName() const {
    return "MockVR System";
}

// Factory function
std::unique_ptr<VRDevice> createVRDevice(bool useMock) {
    if (useMock) {
        LOG_INFO("Creating MockVRDevice");
        return std::make_unique<MockVRDevice>();
    }
    
    // In production, detect and create appropriate VR device
    // For now, default to mock device
    LOG_WARNING("Real VR device not implemented, using MockVRDevice");
    return std::make_unique<MockVRDevice>();
}

} // namespace AimlabVR

