//
// Created by app12 on 22.03.2022.
//

#include "globalPackageSend.h"
#include "../../wifi/wifihandler.h"
#include "../trackerPacket.h"
#include "defines.h"
#include "../../../sensors/sensorManager.h"

using namespace Packet::TrackerPacket::Send;

namespace Packet {
    namespace Global {
        void SendHeartbeat() {
            SHeartBeat grHeartBeat;
            SendStruct(grHeartBeat);
        }

        void SendHandshakeRequest() {
            SHandshake grHandshake;
            grHandshake.nImu0 = IMU;
            grHandshake.nBoard = 5; //BOARD_WROOM32
            grHandshake.nHardwareMCU = 2; // ESP32
            grHandshake.nBuildVersion = 10;
            WiFi.macAddress(grHandshake.achMac);
            SendStruct(grHandshake);
        }

        void SendRSSI(int8_t i_nRSSI) {
            SRSSI grRSSI;
            grRSSI.idk = -1;
            grRSSI.nRSSI = i_nRSSI;
            SendStruct(grRSSI);
        }

        void SendSensorInfo(uint8_t i_nSensorId, uint8_t i_nSensorState, uint8_t i_nSensorType) {
            SSensorInfoSl grSensorInfo;
            grSensorInfo.nSensorId = i_nSensorId;
            grSensorInfo.nSensorState = i_nSensorState;
            grSensorInfo.nSensorType = i_nSensorType;
            SendStruct(grSensorInfo);
        }

        void SendSensorState(uint8_t i_nSensorState0, uint8_t i_nSensorState1) {
            SSensorState grSensorState;
            grSensorState.nSensor0State = i_nSensorState0;
            grSensorState.nSensor1State = i_nSensorState1;
            SendStruct(grSensorState);
        }
    }
}