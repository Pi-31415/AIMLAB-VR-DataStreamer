////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - VR Device Interface
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Abstract interface for VR device integration. Provides a unified API
//   for accessing VR hardware from different platforms (OpenVR, Oculus, etc.)
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_VRDEVICE_H
#define AIMLAB_VR_VRDEVICE_H

#include "Types.h"
#include <vector>
#include <memory>
#include <string>

namespace AimlabVR {

/**
 * @class VRDevice
 * @brief Abstract interface for VR device integration
 * 
 * Provides a unified API for accessing VR hardware regardless of the
 * underlying VR system (OpenVR, Oculus SDK, OpenXR, etc.)
 */
class VRDevice {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~VRDevice() = default;

    /**
     * @brief Initialize VR device system
     * @return True if successful, false otherwise
     */
    virtual bool initialize() = 0;

    /**
     * @brief Shutdown VR device system
     */
    virtual void shutdown() = 0;

    /**
     * @brief Update device states (call once per frame)
     * @return True if successful, false otherwise
     */
    virtual bool update() = 0;

    /**
     * @brief Get all connected devices
     * @return Vector of device data
     */
    virtual std::vector<DeviceData> getDevices() const = 0;

    /**
     * @brief Get specific device by ID
     * @param deviceId Device identifier
     * @param deviceData Output device data
     * @return True if device found, false otherwise
     */
    virtual bool getDevice(uint32_t deviceId, DeviceData& deviceData) const = 0;

    /**
     * @brief Get HMD device data
     * @param deviceData Output device data
     * @return True if HMD found, false otherwise
     */
    virtual bool getHMD(DeviceData& deviceData) const = 0;

    /**
     * @brief Get left controller device data
     * @param deviceData Output device data
     * @return True if controller found, false otherwise
     */
    virtual bool getLeftController(DeviceData& deviceData) const = 0;

    /**
     * @brief Get right controller device data
     * @param deviceData Output device data
     * @return True if controller found, false otherwise
     */
    virtual bool getRightController(DeviceData& deviceData) const = 0;

    /**
     * @brief Check if VR system is active
     * @return True if active, false otherwise
     */
    virtual bool isActive() const = 0;

    /**
     * @brief Get VR system name
     * @return System name string
     */
    virtual std::string getSystemName() const = 0;
};

/**
 * @class MockVRDevice
 * @brief Mock implementation for testing without VR hardware
 * 
 * Provides simulated VR device data for testing and development
 * without requiring actual VR hardware.
 */
class MockVRDevice : public VRDevice {
public:
    MockVRDevice();
    virtual ~MockVRDevice() = default;

    bool initialize() override;
    void shutdown() override;
    bool update() override;
    std::vector<DeviceData> getDevices() const override;
    bool getDevice(uint32_t deviceId, DeviceData& deviceData) const override;
    bool getHMD(DeviceData& deviceData) const override;
    bool getLeftController(DeviceData& deviceData) const override;
    bool getRightController(DeviceData& deviceData) const override;
    bool isActive() const override;
    std::string getSystemName() const override;

private:
    void generateMockData();
    
    std::vector<DeviceData> m_mockDevices;
    bool m_isActive;
    uint64_t m_frameCount;
};

/**
 * @brief Factory function to create VR device instance
 * @param useMock Use mock device for testing
 * @return Unique pointer to VR device instance
 */
std::unique_ptr<VRDevice> createVRDevice(bool useMock = false);

} // namespace AimlabVR

#endif // AIMLAB_VR_VRDEVICE_H

