//
// Created by app12 on 22.03.2022.
//

#include "network.h"
#include "wifi/wifihandler.h"
#include "packet/packetHandler.h"


bool CNetwork::initialize() {
    // Init Wi-Fi connection
    Packet::CPacketHandler::instance()->initialize();
    CWiFiNetwork::instance()->initialize();
    return false;
}