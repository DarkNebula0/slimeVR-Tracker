//
// Created by app12 on 22.03.2022.
//

#include "globalPackage.h"
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
    }
}