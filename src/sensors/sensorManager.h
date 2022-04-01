#pragma once

/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2022 TheDevMinerTV

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
#include "../Singleton.h"
#include "sensor.h"
#include <memory>
#include <FreeRTOS.h>


class CSensorManager : public CSingleton<CSensorManager> {
    friend class CSingleton<CSensorManager>;

private:
    CSensorManager();

    ~CSensorManager() override = default;

public:
    void initialize();

public:
    void setLastSensorState0(CSensor::ESensorStatus i_eState) {
        this->m_eLastSensorState0 = i_eState;
    }

    void setLastSensorState1(CSensor::ESensorStatus i_eState) {
        this->m_eLastSensorState1 = i_eState;
    }

public:
    [[nodiscard]] CSensor *getFirst() const { return m_pSensor1; };

    [[nodiscard]] CSensor *getSecond() const { return m_pSensor2; };

    [[nodiscard]] CSensor::ESensorStatus getLastSensorState0() const {
        return this->m_eLastSensorState0;
    }

    [[nodiscard]] CSensor::ESensorStatus getLastSensorState1() const {
        return this->m_eLastSensorState1;
    }

public:
    SemaphoreHandle_t x_SPI_Semaphore = nullptr;

private:
    CSensor *m_pSensor1;
    CSensor *m_pSensor2;

    CSensor::ESensorStatus m_eLastSensorState0;
    CSensor::ESensorStatus m_eLastSensorState1;
};