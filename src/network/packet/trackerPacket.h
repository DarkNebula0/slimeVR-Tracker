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
                PingPong = 10,
                SensorInfoSl = 15,
            };
        }

        namespace Send {
            enum EID : uint32_t {
                HeartBeat = 0x00,
                Handshake = 0x03,
                RSSI = 19,
                SensorState = 0x04,
                SensorInfoSl = 15,
                RotationData = 17,
            };

            struct SHeartBeat : SPacketHeader<SHeartBeat, EID::HeartBeat> {
            };

            struct SHandshake : SDefaultPacketHeader<SHandshake, EID::Handshake> {
                uint32_t nBoard;
                uint32_t nImu0;
                uint32_t nHardwareMCU;
                uint32_t nUnk[3]; // idk
                uint32_t nBuildVersion; // idk
                uint8_t nFirmwareLength; // 0
                uint8_t achMac[6];
            };

            struct SRSSI : SDefaultPacketHeader<SRSSI, EID::RSSI> {
                int8_t idk;
                int8_t nRSSI;
            };

            struct SSensorState : SDefaultPacketHeader<SSensorState, EID::SensorState> {
                uint8_t nSensor0State;
                uint8_t nSensor1State;
            };

            struct SSensorInfoSl : SDefaultPacketHeader<SSensorInfoSl, EID::SensorInfoSl> {
                uint8_t nSensorId;
                uint8_t nSensorState;
                uint8_t nSensorType;
            };

            struct SRotationData : SDefaultPacketHeader<SRotationData, EID::RotationData> {
                uint8_t nId;
                uint8_t nDataType;
                float rX;
                float rY;
                float rZ;
                float rW;
                uint8_t nAccuracy;
            };

        }
#pragma pack(pop)
    }
}
