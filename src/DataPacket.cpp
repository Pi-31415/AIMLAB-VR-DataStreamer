////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Data Packet Implementation
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Implementation of data packet serialization and deserialization.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#include "aimlab-vr-datastreamer/DataPacket.h"
#include "aimlab-vr-datastreamer/Logger.h"
#include <cstring>
#include <sstream>

namespace AimlabVR {

DataPacket::DataPacket()
    : m_timestamp(0)
    , m_sequenceNumber(0)
    , m_format(PacketFormat::Binary) {
}

DataPacket::DataPacket(PacketFormat format)
    : m_timestamp(0)
    , m_sequenceNumber(0)
    , m_format(format) {
}

void DataPacket::addDevice(const DeviceData& deviceData) {
    m_devices.push_back(deviceData);
}

void DataPacket::clear() {
    m_devices.clear();
    m_timestamp = 0;
}

const std::vector<DeviceData>& DataPacket::getDevices() const {
    return m_devices;
}

void DataPacket::setTimestamp(uint64_t timestamp) {
    m_timestamp = timestamp;
}

uint64_t DataPacket::getTimestamp() const {
    return m_timestamp;
}

std::vector<uint8_t> DataPacket::serialize() const {
    switch (m_format) {
        case PacketFormat::Binary:
            return serializeBinary();
        case PacketFormat::JSON:
            return serializeJSON();
        default:
            return serializeBinary();
    }
}

bool DataPacket::deserialize(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return false;
    }

    switch (m_format) {
        case PacketFormat::Binary:
            return deserializeBinary(data);
        case PacketFormat::JSON:
            return deserializeJSON(data);
        default:
            return deserializeBinary(data);
    }
}

void DataPacket::setFormat(PacketFormat format) {
    m_format = format;
}

PacketFormat DataPacket::getFormat() const {
    return m_format;
}

size_t DataPacket::getSize() const {
    return serialize().size();
}

void DataPacket::setSequenceNumber(uint32_t sequenceNum) {
    m_sequenceNumber = sequenceNum;
}

uint32_t DataPacket::getSequenceNumber() const {
    return m_sequenceNumber;
}

std::vector<uint8_t> DataPacket::serializeBinary() const {
    std::vector<uint8_t> data;
    
    // Header: [Magic(4)] [Version(2)] [SeqNum(4)] [Timestamp(8)] [DeviceCount(4)]
    const uint32_t magic = 0x41494D4C;  // "AIML"
    const uint16_t version = 1;
    const uint32_t deviceCount = static_cast<uint32_t>(m_devices.size());

    // Reserve approximate size
    data.reserve(22 + deviceCount * 128);

    // Write header
    auto writeUint32 = [&](uint32_t val) {
        data.push_back((val >> 24) & 0xFF);
        data.push_back((val >> 16) & 0xFF);
        data.push_back((val >> 8) & 0xFF);
        data.push_back(val & 0xFF);
    };

    auto writeUint16 = [&](uint16_t val) {
        data.push_back((val >> 8) & 0xFF);
        data.push_back(val & 0xFF);
    };

    auto writeUint64 = [&](uint64_t val) {
        for (int i = 7; i >= 0; --i) {
            data.push_back((val >> (i * 8)) & 0xFF);
        }
    };

    auto writeFloat = [&](float val) {
        uint32_t intVal;
        std::memcpy(&intVal, &val, sizeof(float));
        writeUint32(intVal);
    };

    writeUint32(magic);
    writeUint16(version);
    writeUint32(m_sequenceNumber);
    writeUint64(m_timestamp);
    writeUint32(deviceCount);

    // Write device data
    for (const auto& device : m_devices) {
        writeUint32(device.deviceId);
        data.push_back(static_cast<uint8_t>(device.deviceType));
        data.push_back(static_cast<uint8_t>(device.trackingState));
        data.push_back(device.isConnected ? 1 : 0);
        
        // Position
        writeFloat(device.pose.position.x);
        writeFloat(device.pose.position.y);
        writeFloat(device.pose.position.z);
        
        // Rotation
        writeFloat(device.pose.rotation.w);
        writeFloat(device.pose.rotation.x);
        writeFloat(device.pose.rotation.y);
        writeFloat(device.pose.rotation.z);
        
        // Timestamp
        writeUint64(device.pose.timestamp);
        
        // Button states (simplified)
        data.push_back(device.buttons.trigger ? 1 : 0);
        data.push_back(device.buttons.grip ? 1 : 0);
        writeFloat(device.buttons.triggerValue);
        writeFloat(device.buttons.gripValue);
    }

    return data;
}

std::vector<uint8_t> DataPacket::serializeJSON() const {
    std::ostringstream oss;
    oss << "{\"seq\":" << m_sequenceNumber << ",\"ts\":" << m_timestamp 
        << ",\"devices\":[";
    
    for (size_t i = 0; i < m_devices.size(); ++i) {
        const auto& dev = m_devices[i];
        if (i > 0) oss << ",";
        oss << "{\"id\":" << dev.deviceId 
            << ",\"type\":" << static_cast<int>(dev.deviceType)
            << ",\"pos\":[" << dev.pose.position.x << "," 
            << dev.pose.position.y << "," << dev.pose.position.z << "]"
            << ",\"rot\":[" << dev.pose.rotation.w << "," 
            << dev.pose.rotation.x << "," << dev.pose.rotation.y << "," 
            << dev.pose.rotation.z << "]}";
    }
    oss << "]}";

    std::string jsonStr = oss.str();
    return std::vector<uint8_t>(jsonStr.begin(), jsonStr.end());
}

bool DataPacket::deserializeBinary(const std::vector<uint8_t>& data) {
    if (data.size() < 22) {
        return false;
    }

    size_t offset = 0;

    auto readUint32 = [&]() -> uint32_t {
        uint32_t val = (static_cast<uint32_t>(data[offset]) << 24) |
                       (static_cast<uint32_t>(data[offset + 1]) << 16) |
                       (static_cast<uint32_t>(data[offset + 2]) << 8) |
                       static_cast<uint32_t>(data[offset + 3]);
        offset += 4;
        return val;
    };

    auto readUint64 = [&]() -> uint64_t {
        uint64_t val = 0;
        for (int i = 0; i < 8; ++i) {
            val = (val << 8) | static_cast<uint64_t>(data[offset++]);
        }
        return val;
    };

    auto readFloat = [&]() -> float {
        uint32_t intVal = readUint32();
        float floatVal;
        std::memcpy(&floatVal, &intVal, sizeof(float));
        return floatVal;
    };

    // Read header
    uint32_t magic = readUint32();
    if (magic != 0x41494D4C) {
        LOG_ERROR("Invalid packet magic number");
        return false;
    }

    offset += 2;  // Skip version
    m_sequenceNumber = readUint32();
    m_timestamp = readUint64();
    uint32_t deviceCount = readUint32();

    m_devices.clear();
    m_devices.reserve(deviceCount);

    // Read devices (simplified parsing)
    for (uint32_t i = 0; i < deviceCount && offset < data.size(); ++i) {
        DeviceData device;
        device.deviceId = readUint32();
        device.deviceType = static_cast<DeviceType>(data[offset++]);
        device.trackingState = static_cast<TrackingState>(data[offset++]);
        device.isConnected = data[offset++] != 0;
        
        device.pose.position.x = readFloat();
        device.pose.position.y = readFloat();
        device.pose.position.z = readFloat();
        
        device.pose.rotation.w = readFloat();
        device.pose.rotation.x = readFloat();
        device.pose.rotation.y = readFloat();
        device.pose.rotation.z = readFloat();
        
        device.pose.timestamp = readUint64();
        
        device.buttons.trigger = data[offset++] != 0;
        device.buttons.grip = data[offset++] != 0;
        device.buttons.triggerValue = readFloat();
        device.buttons.gripValue = readFloat();
        
        m_devices.push_back(device);
    }

    return true;
}

bool DataPacket::deserializeJSON(const std::vector<uint8_t>& data) {
    // Minimal JSON parsing - in production use proper JSON library
    std::string jsonStr(data.begin(), data.end());
    
    // Simple parsing (demonstration only)
    m_devices.clear();
    LOG_WARNING("JSON deserialization not fully implemented");
    
    return false;
}

} // namespace AimlabVR

