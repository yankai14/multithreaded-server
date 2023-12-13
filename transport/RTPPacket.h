//
// Created by Yan Kai Lim on 13/12/23.
//

#ifndef MULTITHREADED_SERVER_RTPPACKET_H
#define MULTITHREADED_SERVER_RTPPACKET_H

#include <cstdint>
#include <vector>

namespace Transport {

    class RTPPacket {
    private:
        std::vector<std::byte> header;
        std::vector<std::byte> payload;
        const uint8_t HEADER_SIZE = 12;
    public:
        RTPPacket();
        void encode(uint8_t version, bool padding, bool extension, uint8_t cc, bool marker, uint8_t payloadType, uint16_t seq, uint32_t ssrc);
        void decode(std::vector<std::byte>& byteStream);

        uint8_t version();
        uint16_t seq();
        uint32_t time();
        uint8_t payloadType();
        const std::vector<std::byte>& getPayload() const;
        void getPacket(std::vector<std::byte>& packet) const;
    };

} // Transport

#endif //MULTITHREADED_SERVER_RTPPACKET_H
