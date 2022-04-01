#pragma once

#include <cstdint>

namespace Packet {
    namespace Global {
        void PairingRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize);

        void HardBeatRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize);

        void HandshakeRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize);
    }
}