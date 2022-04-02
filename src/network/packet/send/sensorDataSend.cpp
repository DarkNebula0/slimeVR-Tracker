#include "globalPackageSend.h"
#include "../../wifi/wifihandler.h"
#include "../trackerPacket.h"
#include "defines.h"
#include "../../../sensors/sensorManager.h"

using namespace Packet::TrackerPacket::Send;

namespace Packet {
    namespace SensorData {

        void sendSensorRotation(uint8_t i_nId, uint32_t i_nAccuracy, const CSensor::SQuat &i_grQuat) {
            SRotationData grRotation;
            grRotation.nId = i_nId;
            grRotation.nAccuracy = i_nAccuracy;
            grRotation.nDataType = 1;
            grRotation.rX = static_cast<float>(i_grQuat.dX);
            grRotation.rY = static_cast<float>(i_grQuat.dY);
            grRotation.rZ = static_cast<float>(i_grQuat.dZ);
            grRotation.rW = static_cast<float>(i_grQuat.dW);

            SendStruct(grRotation);
        }

    }
}