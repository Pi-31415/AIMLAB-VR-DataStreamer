////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Common Types and Definitions
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Common type definitions, enumerations, and structures used throughout
//   the AIMLAB VR Data Streamer library. Provides platform-agnostic types
//   and data structures for VR tracking data.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_TYPES_H
#define AIMLAB_VR_TYPES_H

#include <cstdint>
#include <string>
#include <chrono>

namespace AimlabVR {

/**
 * @brief 3D Vector structure for position and orientation data
 */
struct Vector3 {
    float x;
    float y;
    float z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

/**
 * @brief Quaternion structure for rotation data
 */
struct Quaternion {
    float w;
    float x;
    float y;
    float z;

    Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    Quaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}
};

/**
 * @brief Pose structure combining position and orientation
 */
struct Pose {
    Vector3 position;
    Quaternion rotation;
    uint64_t timestamp;  // Microseconds since epoch

    Pose() : timestamp(0) {}
};

/**
 * @brief VR device types
 */
enum class DeviceType {
    Unknown = 0,
    HeadMountedDisplay,
    LeftController,
    RightController,
    Tracker,
    BaseStation
};

/**
 * @brief VR tracking state
 */
enum class TrackingState {
    NotTracked = 0,
    Tracked,
    TrackingLost
};

/**
 * @brief Network protocol types
 */
enum class NetworkProtocol {
    UDP = 0,
    TCP,
    WebSocket
};

/**
 * @brief Connection status
 */
enum class ConnectionStatus {
    Disconnected = 0,
    Connecting,
    Connected,
    Error
};

/**
 * @brief Log levels
 */
enum class LogLevel {
    Debug = 0,
    Info,
    Warning,
    Error,
    Critical
};

/**
 * @brief Button states for VR controllers
 */
struct ButtonState {
    bool trigger;
    bool grip;
    bool menu;
    bool touchpad;
    bool thumbstick;
    float triggerValue;      // 0.0 to 1.0
    float gripValue;         // 0.0 to 1.0
    Vector3 touchpadAxis;    // x, y, pressed
    Vector3 thumbstickAxis;  // x, y, pressed
};

/**
 * @brief Complete device data structure
 */
struct DeviceData {
    uint32_t deviceId;
    DeviceType deviceType;
    TrackingState trackingState;
    Pose pose;
    ButtonState buttons;  // Only relevant for controllers
    bool isConnected;
    std::string deviceName;
};

/**
 * @brief Configuration structure
 */
struct StreamConfig {
    std::string serverAddress;
    uint16_t serverPort;
    NetworkProtocol protocol;
    uint32_t updateRate;     // Hz
    bool enableLogging;
    LogLevel logLevel;
    std::string logFilePath;
};

// Type aliases for convenience
using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double>;

} // namespace AimlabVR

#endif // AIMLAB_VR_TYPES_H

