#include "wifihandler.h"
#include <Arduino.h>
#include "../../logging/Logger.h"
#include <AutoConnect.h>
#include <AsyncUDP.h>
#include "../packet/packetHandler.h"
#include "../packet/send/globalPackageSend.h"

WebServer s_oPortalServer;
AutoConnect s_oPortal(s_oPortalServer);

[[noreturn]] void taskUpKeep(CWiFiNetwork *i_pClass) {
    bool bWasConnected = false;

    while (true) {
        if (!USE_WIFI_DATA) {
            s_oPortal.handleClient();
        }

        if (WiFiClass::status() == WL_CONNECTED) {
            if (i_pClass->isTimedOut()) {
                // Not sure if I should only reconnect to the SlimeVR server
                // But I think if a timeout occurs on the tracker itself it is better to restart it.
                CLogger::instance()->error("WiFi: Connection to the SlimeVR server was interrupted");
                ESP.restart();
            }

            if (i_pClass->getIsSlimeServerConnected()) {
                int8_t newRSSI = WiFi.RSSI();
                int8_t lastRSSI = i_pClass->lastRSSI();

                // Check rssi range +-5 to last report
                if (lastRSSI == 0 || !(lastRSSI - 5 <= newRSSI && newRSSI <= lastRSSI + 5)) {
                    // New rssi values update and tell slime vr server
                    i_pClass->setRSSI(newRSSI);
                    Packet::Global::SendRSSI(newRSSI);
                }
            }

            if (!bWasConnected) {
                bWasConnected = true;
                CLogger::instance()->info("WiFi: Connected successfully to SSID '%s', ip address %s\n",
                                          WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

                // Open udp connection
                i_pClass->getUdpClient()->listen(UDP_PORT);
                i_pClass->resetHeartbeat();
                i_pClass->setRSSI(0);
            }
        } else {
            if (bWasConnected) {
                bWasConnected = false;
                i_pClass->setIsSlimeServerConnected(false);
                CLogger::instance()->info("WiFi: Connection to WiFi lost, reconnecting...");
                // Close udp connection
                i_pClass->getUdpClient()->close();
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void CWiFiNetwork::initialize() {
    CLogger::instance()->info("Setting up WiFi");

    WiFi.persistent(true);
    // TODO: Power Saving
    WiFi.setSleep(WIFI_PS_NONE);
    WiFiClass::mode(WIFI_STA);
    WiFi.hostname("SlimeVR FBT Tracker");

    CLogger::instance()->info("Loaded credentials for SSID %s and pass length %d", WiFi.SSID().c_str(),
                              WiFi.psk().length());
    wl_status_t status = WiFi.begin(); // Should connect to last used access point, see https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/station-class.html#begin

    CLogger::instance()->debug("Status: %d", status);

    if (status != WL_CONNECTED) {
        // Remove after dev
        if (USE_WIFI_DATA) {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        } else {
            AutoConnectConfig oApConfig("SlimeVR FBT Tracker", HOTSPOT_PASSWORD);;
            oApConfig.autoReconnect = true;    // Attempt automatic reconnection.
            oApConfig.reconnectInterval = 6;   // Seek interval time is 180[s].
            s_oPortal.config(oApConfig);
            s_oPortal.begin();
        }
    }

    this->m_oUdp.onPacket([=](AsyncUDPPacket packet) {
        this->m_oHost = packet.remoteIP();
        this->m_shPort = packet.remotePort();
        Packet::CPacketHandler::instance()->handlePacket(packet.data(), packet.length());
    });

    // TODO: Check other task sizes
    // Create wifi upkeep task
    xTaskCreate(
            reinterpret_cast<TaskFunction_t>(taskUpKeep), "WifiUpKeep", 4000, reinterpret_cast<void *>(this), 5,
            nullptr
    );
}

void CWiFiNetwork::send(const void *i_pBuffer, const int16_t i_shSize) {
    if (CWiFiNetwork::isConnected()) {
        AsyncUDPMessage grMsg(i_shSize);
        grMsg.write(reinterpret_cast<const uint8_t *>(i_pBuffer), i_shSize);

        this->getUdpClient()->sendTo(grMsg, this->m_oHost, this->m_shPort);
    }
}


