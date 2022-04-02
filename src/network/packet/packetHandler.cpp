//
// Created by app12 on 22.03.2022.
//

#include "packetHandler.h"
#include "packetBase.h"
#include "trackerPacket.h"
#include "../../logging/Logger.h"
#include "receive/globalPackage.h"

#define AddOperation() (this->addPacketOperation(grOperation))

namespace Packet {
    void CPacketHandler::handlePacket(const uint8_t *i_pBuffer, const size_t &i_nSize) {
        size_t nCursorPos = 0;

        while (i_nSize - nCursorPos >= sizeof(SPacketHeader<>) && nCursorPos < i_nSize) {
            const uint8_t *pPointer = i_pBuffer + nCursorPos;
            const auto *pHeader = reinterpret_cast<const SPacketHeader<> *>(pPointer);

            if (pHeader->length() >= 7 || i_nSize - nCursorPos >= pHeader->length()) {
                if (pHeader->verifier() == 0xF0) {
                    const SOperation &grOperation = this->m_afnPacket[pHeader->id()];
                    if (grOperation.fnFunction) {
                        grOperation.fnFunction(i_pBuffer + nCursorPos, pHeader->length());
                    } else {
                        CLogger::instance()->warn("PacketHandler-Function Handler not found! ID: 0x%02X",
                                                  pHeader->id());
                    }
                } else {
                    CLogger::instance()->warn("PacketHandler-Function Invalid verifier");
                    break;
                }
                nCursorPos += pHeader->length();
            } else {
                CLogger::instance()->warn("PacketHandler-Function Packet is not big enough!");
                break;
            }
        }
    }

    void CPacketHandler::initialize() {
        SOperation grOperation;

        // Pairing
        grOperation.chID = TrackerPacket::Receive::EID::Pairing;
        grOperation.fnFunction = &Global::PairingRequest;
        AddOperation();

        // HeartBeat
        grOperation.chID = TrackerPacket::Receive::EID::HeartBeat;
        grOperation.fnFunction = &Global::HardBeatRequest;
        AddOperation();

        // Handshake
        grOperation.chID = TrackerPacket::Receive::EID::Handshake;
        grOperation.fnFunction = &Global::HandshakeRequest;
        AddOperation();

        // Sensor info
        grOperation.chID = TrackerPacket::Receive::EID::SensorInfoSl;
        grOperation.fnFunction = &Global::SensorInfoRequest;
        AddOperation();

        // Ping pong
        grOperation.chID = TrackerPacket::Receive::EID::PingPong;
        grOperation.fnFunction = &Global::PingPong;
        AddOperation();


        CLogger::instance()->info("%u Handler were registered", this->m_afnPacket.size());
    }
}