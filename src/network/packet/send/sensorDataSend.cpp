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
            grRotation.dSensorRotation = (i_nId == 0) ? IMU_ROTATION : SECOND_IMU_ROTATION;
            grRotation.dX = i_grQuat.dX;
            grRotation.dY = i_grQuat.dY;
            grRotation.dZ = i_grQuat.dZ;
            grRotation.dW = i_grQuat.dW;

            SendStruct(grRotation);
        }

    }
}