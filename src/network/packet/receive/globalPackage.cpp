//
// Created by app12 on 22.03.2022.
//

#include "globalPackage.h"
#include "../../../sensors/sensorManager.h"
#include "../../../logging/Logger.h"
#include "../../wifi/wifihandler.h"
#include "../send/globalPackageSend.h"

namespace Packet {
    namespace Global {
        void PairingRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize) {
            if (!CWiFiNetwork::instance()->getIsSlimeServerConnected()) {
                SendHandshakeRequest();
            }
        }

        void HardBeatRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize) {
            if (CWiFiNetwork::instance()->getIsSlimeServerConnected()) {
                CWiFiNetwork::instance()->resetHeartbeat();
                SendHeartbeat();
            }
        }

        void HandshakeRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize) {
            CLogger::instance()->info("Handshake successful");
            CWiFiNetwork::instance()->resetHeartbeat();
            CWiFiNetwork::instance()->setIsSlimeServerConnected(true);
        }

        void PingPong(const uint8_t *i_pBuffer, const uint16_t &i_nSize) {
            CWiFiNetwork::instance()->send(reinterpret_cast<const void *>(i_pBuffer), i_nSize);
        }

        void SensorInfoRequest(const uint8_t *i_pBuffer, const uint16_t &i_nSize) {
            if (IMU != IMU_NONE) {
                SendSensorInfo(0, static_cast<uint8_t>(CSensorManager::instance()->getLastSensorState0()), IMU);
            }

            if (SECOND_IMU != IMU_NONE) {
                SendSensorInfo(1, static_cast<uint8_t>(CSensorManager::instance()->getLastSensorState1()), SECOND_IMU);
            }
        }
    }
}