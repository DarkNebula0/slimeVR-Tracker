#pragma once

#include "../Singleton.h"
#include "./packet/trackerPacket.h"


class CNetwork : public CSingleton<CNetwork> {
    friend class CSingleton<CNetwork>;

private:
    CNetwork() = default;

    ~CNetwork() override = default;

public:
    bool initialize();

};
