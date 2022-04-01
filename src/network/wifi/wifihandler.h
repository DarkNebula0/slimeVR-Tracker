#pragma once

#include "../../Singleton.h"
#include "../../defines.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <AsyncUDP.h>

#define SendStruct(xStruct) (CWiFiNetwork::instance()->send(reinterpret_cast<const void*>(&xStruct), xStruct.length()))

class CWiFiNetwork : public CSingleton<CWiFiNetwork> {
    friend class CSingleton<CWiFiNetwork>;

private:
    CWiFiNetwork() = default;

    ~CWiFiNetwork() override = default;

public:
    static bool isConnected() {
        return WiFiClass::status() == WL_CONNECTED;
    };

    AsyncUDP *getUdpClient() {
        return &this->m_oUdp;
    }

    [[nodiscard]] __inline bool getIsSlimeServerConnected() const {
        return this->m_bIsSlimeServerConnected;
    };

    [[nodiscard]] __inline bool isTimedOut() const {
        return this->m_bIsSlimeServerConnected && (millis() - this->m_nLastHeartbeat) > SLIME_SERVER_TIMEOUT_MS;
    };

    [[nodiscard]] __inline int8_t lastRSSI() const {
        return this->m_nLastRSSI;
    };

public:
    void initialize();

    void send(const void *i_pBuffer, int16_t i_shSize);

    __inline void setIsSlimeServerConnected(bool i_bState) {
        this->m_bIsSlimeServerConnected = i_bState;
    }

    __inline void setRSSI(int8_t i_nRSSI) {
        this->m_nLastRSSI = i_nRSSI;
    }

    __inline void resetHeartbeat() {
        this->m_nLastHeartbeat = millis();
    }

private:
    AsyncUDP m_oUdp;
    IPAddress m_oHost;
    uint16_t m_shPort;
    uint64_t m_nLastHeartbeat;
    int8_t m_nLastRSSI;
    bool m_bIsSlimeServerConnected;
};