#include "sensorManager.h"
#include "../logging/Logger.h"
#include "./icm20948sensor.h"
#include "./mpu6050sensor.h"
#include "./erroneousSensor.h"
#include "../network/packet/send/globalPackageSend.h"
#include "../network/wifi/wifihandler.h"
#include <memory>
#include <i2cscan.h>

CSensorManager::CSensorManager() {
    this->m_pSensor1 = new CSensor();
    this->m_pSensor2 = new CSensor();
}


[[noreturn]] void sensorTasks(CSensorManager *i_pClass) {
    while (true) {
        if (CWiFiNetwork::instance()->getIsSlimeServerConnected()) {

            auto pFistSensor = i_pClass->getFirst();
            auto pSecondSensor = i_pClass->getSecond();
            auto fistSensorState = pFistSensor->getSensorState();
            auto secondSensorState = pFistSensor->getSensorState();
            bool bSendNewSensorState = false;

            if (i_pClass->getLastSensorState0() != fistSensorState) {
                i_pClass->setLastSensorState0(fistSensorState);
                bSendNewSensorState = true;
            }

            if (i_pClass->getLastSensorState1() != secondSensorState) {
                i_pClass->setLastSensorState1(secondSensorState);
                bSendNewSensorState = true;
            }

            // Send new sensor state if something has changed
            if (bSendNewSensorState) {
                Packet::Global::SendSensorState(fistSensorState, secondSensorState);
            }

            xSemaphoreTake(i_pClass->x_SPI_Semaphore, portMAX_DELAY);
            // Send sensor data
            if (fistSensorState == CSensor::SENSOR_OK) {
                pFistSensor->sendSensorData();
            }

            if (secondSensorState == CSensor::SENSOR_OK) {
                pSecondSensor->sendSensorData();
            }
            xSemaphoreGive(i_pClass->x_SPI_Semaphore);
        }

        vTaskDelay(SAMPLING_RATE_IN_MS / portTICK_PERIOD_MS);
    }
}

void CSensorManager::initialize() {
    CLogger::instance()->info("CSensorManager init");
    uint8_t firstIMUAddress = I2CSCAN::pickDevice(0x68, 0x69, true);
    uint8_t secondIMUAddress = I2CSCAN::pickDevice(0x69, 0x68, false);

    // First IMU
#if IMU != IMU_NONE
    delete this->m_pSensor1;
    if (firstIMUAddress == 0) {
        this->m_pSensor1 = (CSensor *) new CErroneousSensor();
    } else {
        CLogger::instance()->trace("Primary IMU found at address 0x%02X", firstIMUAddress);

        switch (IMU) {
            case IMU_ICM20948:
                this->m_pSensor1 = (CSensor *) new CICM20948Sensor(0, firstIMUAddress);
                break;
            case IMU_MPU6050:
                this->m_pSensor1 = (CSensor *) new CMPU6050sensor(0, firstIMUAddress);
                break;
        }

        this->m_pSensor1->motionSetup();
    }
#endif


    // Second IMU
#if SECOND_IMU != IMU_NONE
    // Second sensor
      delete this->m_pSensor2;
    if (secondIMUAddress == firstIMUAddress) {
        CLogger::instance()->debug("No secondary IMU connected");
    } else if (secondIMUAddress == 0) {
        this->m_pSensor2 = new CErroneousSensor();
    } else {
        CLogger::instance()->trace("Secondary IMU found at address 0x%02X", secondIMUAddress);
        switch (IMU) {
            case IMU_ICM20948:
                this->m_pSensor2 =  new CICM20948Sensor(1, secondIMUAddress);
                break;
            case IMU_MPU6050:
                this->m_pSensor2 = new CMPU6050sensor(1, secondIMUAddress);
                break;
        }

        this->m_pSensor2->motionSetup();
    }

#endif

    // Create mutex
    this->x_SPI_Semaphore = xSemaphoreCreateMutex();
    // TODO: Check other task sizes
    // Create sensor tasks task
    xTaskCreatePinnedToCore(
            reinterpret_cast<TaskFunction_t>(sensorTasks), "SensorTasks", 4000, reinterpret_cast<void *>(this), 2,
            nullptr, 1
    );

    CLogger::instance()->info("CSensorManager init complete");
}

