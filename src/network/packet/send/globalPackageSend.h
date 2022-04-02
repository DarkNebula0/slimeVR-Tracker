#pragma once

#include <stdint.h>


namespace Packet {
    namespace Global {
        void SendHeartbeat();

        void SendHandshakeRequest();

        void SendRSSI(int8_t i_nRSSI);

        void SendSensorInfo(uint8_t i_nSensorId, uint8_t i_nSensorState, uint8_t i_nSensorType);

        void SendSensorState(uint8_t i_nSensorState0, uint8_t i_nSensorState1);
    }
}