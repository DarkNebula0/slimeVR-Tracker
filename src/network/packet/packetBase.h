#pragma once

#include <cstdint>
#include <cstring>
#include <functional>

namespace Packet {
#pragma pack(push, 1)

    template<typename TPacket = uint8_t, uint32_t TID = 0>
    struct SPacketHeader {
    private:
        uint16_t shLength = sizeof(TPacket);
        uint8_t chVerifier = 0xF0;
        uint32_t nID = TID;
    public:
        [[nodiscard]] uint32_t id() const { return this->nID; }

        [[nodiscard]] constexpr uint16_t length() const { return this->shLength; }

        [[nodiscard]] uint8_t verifier() const { return this->chVerifier; }

    public:
        explicit SPacketHeader(const bool i_bClear = true) {
            if (i_bClear) {
                std::memset(reinterpret_cast<char *>(this) + sizeof(SPacketHeader), 0,
                            sizeof(TPacket) - sizeof(SPacketHeader));
            }
        };
    };

#pragma pack(pop)

    using fnFunctionBase = std::function<void(const uint8_t *i_pBuffer, const uint16_t &i_nSize)>;

    struct SOperation {
    public:
        uint32_t chID;
        fnFunctionBase fnFunction;
    public:
        SOperation() : chID(0) {}

        SOperation(const SOperation &i_oOther) {
            this->chID = i_oOther.chID;
            this->fnFunction = i_oOther.fnFunction;
        }

        SOperation(SOperation &&) = default;

        virtual ~SOperation() = default;

    public:
        SOperation &operator=(SOperation &&i_oOther) noexcept {
            chID = i_oOther.chID;
            fnFunction = std::move(i_oOther.fnFunction);
            return *this;
        }
    };
}

template<typename TPacket = uint8_t, uint32_t TID = 0>
using SDefaultPacketHeader = Packet::SPacketHeader<TPacket, TID>;