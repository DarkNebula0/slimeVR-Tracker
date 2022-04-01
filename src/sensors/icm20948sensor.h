#pragma once
/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2021 Eiren Rain & SlimeVR contributors

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/
#include <ICM_20948.h>
#include <Preferences.h>
#include "sensor.h"

class CICM20948Sensor : public CSensor {
public:
    CICM20948Sensor(uint8_t i_bId, uint8_t i_bAddress) : m_bId(i_bId),
                                                         m_bAddress(i_bAddress),
                                                         m_nLastData(0),
                                                         m_bIsFirstRead(true) {}

    ~CICM20948Sensor() = default;

public:
    void motionSetup() final;

    void sendSensorData() final;

    ESensorStatus getSensorState() final {
        return this->m_eState;
    }

private:
    void loadBias();

public:
    CSensor::ESensorStatus m_eState = CSensor::ESensorStatus::SENSOR_OFFLINE;
    ICM_20948_I2C m_oImu;
    Preferences m_oPrefs;
    uint8_t m_bId;
    uint8_t m_bAddress;
    uint64_t m_nLastData;
    bool m_bIsFirstRead;

#define OVERRIDE_DMP_SETUP true
};