//
// Created by Yan Kai Lim on 13/12/23.
//

#include "RTPPacket.h"

namespace Transport {
    RTPPacket::RTPPacket() {
        header.resize(12, std::byte(0));
    }

    void RTPPacket::encode(uint8_t version, bool padding, bool extension, uint8_t cc, bool marker, uint8_t payloadType, uint16_t seq, uint32_t ssrc) {
        uint32_t timestamp = static_cast<uint32_t>(std::time(nullptr));
        // Set RTP header fields
        header[0] = static_cast<std::byte>((version << 6) | (padding << 5) | (extension << 4) | cc);
        header[1] = static_cast<std::byte>((marker << 7) | payloadType);
        header[2] = static_cast<std::byte>(seq >> 8);
        header[3] = static_cast<std::byte>(seq & 0xFF);
        header[4] = static_cast<std::byte>((timestamp >> 24) & 0xFF);
        header[5] = static_cast<std::byte>((timestamp >> 16) & 0xFF);
        header[6] = static_cast<std::byte>((timestamp >> 8) & 0xFF);
        header[7] = static_cast<std::byte>(timestamp & 0xFF);
        header[8] = static_cast<std::byte>(ssrc >> 24);
        header[9] = static_cast<std::byte>((ssrc >> 16) & 0xFF);
        header[10] = static_cast<std::byte>((ssrc >> 8) & 0xFF);
        header[11] = static_cast<std::byte>(ssrc & 0xFF);
    }

    void RTPPacket::decode(std::vector<std::byte>& byteStream) {
        header.assign(byteStream.begin(), byteStream.begin() + HEADER_SIZE);
        payload.assign(byteStream.begin() + HEADER_SIZE, byteStream.end());
    }

    uint8_t RTPPacket::version() {
        return static_cast<uint8_t>(header[0] >> 6);
    }

    uint16_t RTPPacket::seq() {
        return static_cast<uint16_t>((std::to_integer<int>(header[2]) << 8) | std::to_integer<int>(header[3]));
    }

    uint32_t RTPPacket::time() {
        return static_cast<uint32_t>(
            (std::to_integer<int>(header[4]) << 24) |
            (std::to_integer<int>(header[5]) << 16) |
            (std::to_integer<int>(header[6]) << 8) |
            std::to_integer<int>(header[7])
        );
    }

    uint8_t RTPPacket::payloadType() {
        return static_cast<uint8_t>(header[1] & std::byte{127});
    }

    const std::vector<std::byte>& RTPPacket::getPayload() const {
        return payload;
    }

    void RTPPacket::getPacket(std::vector<std::byte>& packet) const {
        packet.insert(packet.end(), header.begin(), header.end());
        packet.insert(packet.end(), payload.begin(), payload.end());
    }
} // Transport