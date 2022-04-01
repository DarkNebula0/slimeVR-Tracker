#pragma once

#include "../../Singleton.h"
#include "packetBase.h"

#include <cstdint>
#include <cstddef>
#include <map>

namespace Packet {
    class CPacketHandler : public CSingleton<CPacketHandler> {
        friend class CSingleton<CPacketHandler>;

    private:
        CPacketHandler() = default;

        ~CPacketHandler() override = default;

    private:
        __inline void addPacketOperation(SOperation &i_grOperation) {
            this->m_afnPacket[i_grOperation.chID] = std::move(i_grOperation);
        }

    public:
        void handlePacket(const uint8_t *i_pBuffer, const size_t &i_nSize);

        void initialize();

    private:
        std::map<uint32_t, SOperation> m_afnPacket;
    };
}