#pragma once


#include "packetBase.h"

namespace Packet {
    namespace TrackerPacket {
#pragma pack(push, 1)

        namespace Receive {
            enum EID : uint32_t {
                Pairing = 0x00,
                HeartBeat = 0x01,
                Handshake = 0x02,
            };

        }

        namespace Send {
            enum EID : uint32_t {
                HeartBeat = 0x01,
                Handshake = 0x02,
                RSSI = 0x03,
                SensorState = 0x04,
                RotationData = 0x05,
            };

            struct SHeartBeat : SPacketHeader<SHeartBeat, EID::HeartBeat> {
            };

            struct SHandshake : SDefaultPacketHeader<SHandshake, EID::Handshake> {
                uint32_t nImu0;
                uint32_t nImu1;
                uint8_t achMac[6];
            };

            struct SRSSI : SDefaultPacketHeader<SRSSI, EID::RSSI> {
                int8_t nRSSI;
            };

            struct SSensorState : SDefaultPacketHeader<SSensorState, EID::SensorState> {
                uint8_t nSensor0State;
                uint8_t nSensor1State;
            };

            struct SRotationData : SDefaultPacketHeader<SRotationData, EID::RotationData> {
                uint8_t nId;
                uint32_t nAccuracy;
                double dSensorRotation;
                double dX;
                double dY;
                double dZ;
                double dW;
            };

        }
#pragma pack(pop)
    }
}
