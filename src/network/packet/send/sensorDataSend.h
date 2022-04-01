#pragma once

#include <stdint.h>
#include "../../../sensors/sensor.h"

namespace Packet {
    namespace SensorData {
        void sendSensorRotation(uint8_t i_nId, uint32_t i_nAccuracy, const CSensor::SQuat &i_grQuat);
    }
}