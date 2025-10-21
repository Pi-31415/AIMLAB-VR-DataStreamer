////////////////////////////////////////////////////////////////////////////////
// AIMLAB-VR-DataStreamer - Data Packet
//
// Author: Pi Ko (pi.ko@nyu.edu)
// Date: 21 October 2025
// Version: v1.0
//
// Description:
//   Data packet structure for serializing and deserializing VR tracking data
//   for network transmission. Supports multiple data formats and compression.
//
// Changelog:
//   v1.0 - 21 October 2025 - Initial creation
////////////////////////////////////////////////////////////////////////////////

#ifndef AIMLAB_VR_DATAPACKET_H
#define AIMLAB_VR_DATAPACKET_H

#include "Types.h"
#include <vector>
#include <cstdint>

namespace AimlabVR {

/**
 * @brief Packet format types
 */
enum class PacketFormat {
    Binary = 0,
    JSON,
    MessagePack
};

/**
 * @class DataPacket
 * @brief Container for VR tracking data with serialization support
 * 
 * Encapsulates VR device data and provides methods for serialization
 * and deserialization for network transmission.
 */
class DataPacket {
public:
    /**
     * @brief Default constructor
     */
    DataPacket();

    /**
     * @brief Constructor with format specification
     * @param format Packet format to use
     */
    explicit DataPacket(PacketFormat format);

    /**
     * @brief Add device data to packet
     * @param deviceData Device data to add
     */
    void addDevice(const DeviceData& deviceData);

    /**
     * @brief Clear all device data from packet
     */
    void clear();

    /**
     * @brief Get all device data
     * @return Vector of device data
     */
    const std::vector<DeviceData>& getDevices() const;

    /**
     * @brief Set packet timestamp
     * @param timestamp Timestamp in microseconds
     */
    void setTimestamp(uint64_t timestamp);

    /**
     * @brief Get packet timestamp
     * @return Timestamp in microseconds
     */
    uint64_t getTimestamp() const;

    /**
     * @brief Serialize packet to byte array
     * @return Serialized data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize packet from byte array
     * @param data Serialized data
     * @return True if successful, false otherwise
     */
    bool deserialize(const std::vector<uint8_t>& data);

    /**
     * @brief Set packet format
     * @param format New packet format
     */
    void setFormat(PacketFormat format);

    /**
     * @brief Get packet format
     * @return Current packet format
     */
    PacketFormat getFormat() const;

    /**
     * @brief Get packet size in bytes
     * @return Size in bytes
     */
    size_t getSize() const;

    /**
     * @brief Set sequence number for packet ordering
     * @param sequenceNum Sequence number
     */
    void setSequenceNumber(uint32_t sequenceNum);

    /**
     * @brief Get sequence number
     * @return Sequence number
     */
    uint32_t getSequenceNumber() const;

private:
    /**
     * @brief Serialize to binary format
     */
    std::vector<uint8_t> serializeBinary() const;

    /**
     * @brief Serialize to JSON format
     */
    std::vector<uint8_t> serializeJSON() const;

    /**
     * @brief Deserialize from binary format
     */
    bool deserializeBinary(const std::vector<uint8_t>& data);

    /**
     * @brief Deserialize from JSON format
     */
    bool deserializeJSON(const std::vector<uint8_t>& data);

    std::vector<DeviceData> m_devices;
    uint64_t m_timestamp;
    uint32_t m_sequenceNumber;
    PacketFormat m_format;
};

} // namespace AimlabVR

#endif // AIMLAB_VR_DATAPACKET_H

