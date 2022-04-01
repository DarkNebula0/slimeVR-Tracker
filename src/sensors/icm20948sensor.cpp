/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2021 S.J. Remington, SlimeVR contributors

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
#include "icm20948sensor.h"
#include <i2cscan.h>
#include <EEPROM.h>
#include "../defines.h"
#include "../logging/Logger.h"
#include "../network/packet/send/sensorDataSend.h"
#include "sensorManager.h"


void saveBiasTask(CICM20948Sensor *i_pClass) {

    uint32_t bias_save_periods[] = {120, 180, 300, 600,
                                    600}; // 2min + 3min + 5min + 10min + 10min (no more saves after 30min)

    uint8_t bias_save_counter = 0;

    // First sleep
    vTaskDelay((bias_save_periods[bias_save_counter] * 1000) / portTICK_PERIOD_MS);

    while (true) {
#if LOG_LEVEL == LOG_LEVEL_TRACE
        CLogger::instance()->trace("Saving Bias");
#endif

        int bias_a[3], bias_g[3], bias_m[3];
        xSemaphoreTake(CSensorManager::instance()->x_SPI_Semaphore, portMAX_DELAY);
        i_pClass->m_oImu.GetBiasGyroX(&bias_g[0]);
        i_pClass->m_oImu.GetBiasGyroY(&bias_g[1]);
        i_pClass->m_oImu.GetBiasGyroZ(&bias_g[2]);

        i_pClass->m_oImu.GetBiasAccelX(&bias_a[0]);
        i_pClass->m_oImu.GetBiasAccelY(&bias_a[1]);
        i_pClass->m_oImu.GetBiasAccelZ(&bias_a[2]);

        i_pClass->m_oImu.GetBiasCPassX(&bias_m[0]);
        i_pClass->m_oImu.GetBiasCPassY(&bias_m[1]);
        i_pClass->m_oImu.GetBiasCPassZ(&bias_m[2]);

        bool accel_set = bias_a[0] && bias_a[1] && bias_a[2];
        bool gyro_set = bias_g[0] && bias_g[1] && bias_g[2];
        bool mag_set = bias_m[0] && bias_m[1] && bias_m[2];

#if LOG_LEVEL == LOG_LEVEL_TRACE
        CLogger::instance()->trace("bias gyro on IMU: %d, %d, %d", bias_g[0], bias_g[1], bias_g[2]);
        CLogger::instance()->trace("bias accel on IMU: %d, %d, %d", bias_a[0], bias_a[1], bias_a[2]);
        CLogger::instance()->trace("bias mag on IMU: %d, %d, %d", bias_m[0], bias_m[1], bias_m[2]);
#endif

        bool auxiliary = i_pClass->m_bId == 1;
        if (accel_set) {
            // Save accel
            i_pClass->m_oPrefs.putInt(auxiliary ? "ba01" : "ba00", bias_a[0]);
            i_pClass->m_oPrefs.putInt(auxiliary ? "ba11" : "ba10", bias_a[1]);
            i_pClass->m_oPrefs.putInt(auxiliary ? "ba21" : "ba20", bias_a[2]);

#if LOG_LEVEL == LOG_LEVEL_TRACE
            CLogger::instance()->trace("Wrote Accel Bias");
#endif
        }

        if (gyro_set) {
            // Save gyro
            i_pClass->m_oPrefs.putInt(auxiliary ? "bg01" : "bg00", bias_g[0]);
            i_pClass->m_oPrefs.putInt(auxiliary ? "bg11" : "bg10", bias_g[1]);
            i_pClass->m_oPrefs.putInt(auxiliary ? "bg21" : "bg20", bias_g[2]);

#if LOG_LEVEL == LOG_LEVEL_TRACE
            CLogger::instance()->trace("Wrote Gyro Bias");
#endif
        }

        if (mag_set) {
            // Save mag
            i_pClass->m_oPrefs.putInt(auxiliary ? "bm01" : "bm00", bias_m[0]);
            i_pClass->m_oPrefs.putInt(auxiliary ? "bm11" : "bm10", bias_m[1]);
            i_pClass->m_oPrefs.putInt(auxiliary ? "bm21" : "bm20", bias_m[2]);

#if LOG_LEVEL == LOG_LEVEL_TRACE
            CLogger::instance()->trace("Wrote Mag Bias");
#endif
        }
        xSemaphoreGive(CSensorManager::instance()->x_SPI_Semaphore);
        bias_save_counter++;
        // Possible: Could make it repeat the final timer value if any of the biases are still 0. Save strategy could be improved.
        if (bias_save_counter < sizeof(bias_save_periods)) {
            vTaskDelay((bias_save_periods[bias_save_counter] * 1000) / portTICK_PERIOD_MS);
        } else {
            break;
        }

    }

    vTaskDelete(nullptr);
}

void CICM20948Sensor::motionSetup() {
    this->m_oPrefs.begin("ICM20948n", false);

    // SparkFun_ICM-20948_ArduinoLibrary only supports 0x68 or 0x69 via boolean, if something else throw a error
    boolean tracker = false;

    if (this->m_bAddress == 0x68) {
        tracker = false;
    } else if (this->m_bAddress == 0x69) {
        tracker = true;
    } else {
        CLogger::instance()->fatal("I2C Address not supported by ICM20948 library: 0x%02x", this->m_bAddress);
        return;
    }

    ICM_20948_Status_e imu_err = this->m_oImu.begin(Wire, tracker);
    if (imu_err != ICM_20948_Stat_Ok) {
        CLogger::instance()->fatal("Can't connect to ICM20948 at address 0x%02x, error code: 0x%02x", this->m_bAddress,
                                   imu_err);
        return;
    }

    // Configure imu setup and load any stored bias values
    if (this->m_oImu.initializeDMP() == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("DMP initialized");
    } else {
        CLogger::instance()->fatal("Failed to initialize DMP");
        return;
    }

    CLogger::instance()->debug("Using 9 axis configuration");
    if (this->m_oImu.enableDMPSensor(INV_ICM20948_SENSOR_ORIENTATION) == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("Enabled DMP sensor for sensor orientation");
    } else {
        CLogger::instance()->fatal("Failed to enable DMP sensor orientation");
        return;
    }

    // Might need to set up other DMP functions later, just Quad6/Quad9 for now
    if (this->m_oImu.setDMPODRrate(DMP_ODR_Reg_Quat9, 0) == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("Set Quat9 to 55Hz frequency");
    } else {
        CLogger::instance()->fatal("Failed to set Quat9 to 55Hz frequency");
        return;
    }

    // Enable the FIFO
    if (this->m_oImu.enableFIFO() == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("FIFO Enabled");
    } else {
        CLogger::instance()->fatal("Failed to enable FIFO");
        return;
    }

    // Enable the DMP
    if (this->m_oImu.enableDMP() == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("DMP Enabled");
    } else {
        CLogger::instance()->fatal("Failed to enable DMP");
        return;
    }

    // Reset DMP
    if (this->m_oImu.resetDMP() == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("Reset DMP");
    } else {
        CLogger::instance()->fatal("Failed to reset DMP");
        return;
    }

    // Reset FIFO
    if (this->m_oImu.resetFIFO() == ICM_20948_Stat_Ok) {
        CLogger::instance()->debug("Reset FIFO");
    } else {
        CLogger::instance()->fatal("Failed to reset FIFO");
        return;
    }

    // Load bias
    loadBias();

    this->m_eState = CSensor::SENSOR_OK;
    this->m_nLastData = millis();

    // TODO: Check other task sizes
    // Create bias saving task

    xTaskCreatePinnedToCore(
            reinterpret_cast<TaskFunction_t>(saveBiasTask), "SaveBiasTask", 2048, reinterpret_cast<void *>(this), 5,
            nullptr, 1
    );
}

void CICM20948Sensor::loadBias() {
    bool auxiliary = this->m_bAddress == 1;

    int32_t bias_a[3], bias_g[3], bias_m[3];

    bias_a[0] = this->m_oPrefs.getInt(auxiliary ? "ba01" : "ba00", 0);
    bias_a[1] = this->m_oPrefs.getInt(auxiliary ? "ba11" : "ba10", 0);
    bias_a[2] = this->m_oPrefs.getInt(auxiliary ? "ba21" : "ba20", 0);

    bias_g[0] = this->m_oPrefs.getInt(auxiliary ? "bg01" : "bg00", 0);
    bias_g[1] = this->m_oPrefs.getInt(auxiliary ? "bg11" : "bg10", 0);
    bias_g[2] = this->m_oPrefs.getInt(auxiliary ? "bg21" : "bg20", 0);

    bias_m[0] = this->m_oPrefs.getInt(auxiliary ? "bm01" : "bm00", 0);
    bias_m[1] = this->m_oPrefs.getInt(auxiliary ? "bm11" : "bm10", 0);
    bias_m[2] = this->m_oPrefs.getInt(auxiliary ? "bm21" : "bm20", 0);

    this->m_oImu.SetBiasGyroX(bias_g[0]);
    this->m_oImu.SetBiasGyroY(bias_g[1]);
    this->m_oImu.SetBiasGyroZ(bias_g[2]);

    this->m_oImu.SetBiasAccelX(bias_a[0]);
    this->m_oImu.SetBiasAccelY(bias_a[1]);
    this->m_oImu.SetBiasAccelZ(bias_a[2]);

    this->m_oImu.SetBiasCPassX(bias_m[0]);
    this->m_oImu.SetBiasCPassY(bias_m[1]);
    this->m_oImu.SetBiasCPassZ(bias_m[2]);

    // Display both values from ESP32 and read back from IMU
#if LOG_LEVEL == LOG_LEVEL_TRACE
    CLogger::instance()->trace("On IMU and on ESP32 should match");
    CLogger::instance()->trace("bias gyro on ESP32 : [%d, %d, %d]", bias_g[0], bias_g[1], bias_g[2]);
    CLogger::instance()->trace("bias accel on ESP32: [%d, %d, %d]", bias_a[0], bias_a[1], bias_a[2]);
    CLogger::instance()->trace("bias mag on ESP32  : [%d, %d, %d]", bias_m[0], bias_m[1], bias_m[2]);

    this->m_oImu.GetBiasGyroX(&bias_g[0]);
    this->m_oImu.GetBiasGyroY(&bias_g[1]);
    this->m_oImu.GetBiasGyroZ(&bias_g[2]);

    this->m_oImu.GetBiasAccelX(&bias_a[0]);
    this->m_oImu.GetBiasAccelY(&bias_a[1]);
    this->m_oImu.GetBiasAccelZ(&bias_a[2]);

    this->m_oImu.GetBiasCPassX(&bias_m[0]);
    this->m_oImu.GetBiasCPassY(&bias_m[1]);
    this->m_oImu.GetBiasCPassZ(&bias_m[2]);

    CLogger::instance()->trace("bias gyro on IMU : [%d, %d, %d]", bias_g[0], bias_g[1], bias_g[2]);
    CLogger::instance()->trace("bias accel on IMU: [%d, %d, %d]", bias_a[0], bias_a[1], bias_a[2]);
    CLogger::instance()->trace("bias mag on IMU  : [%d, %d, %d]", bias_m[0], bias_m[1], bias_m[2]);
#endif
}


void CICM20948Sensor::sendSensorData() {

    if (this->getSensorState() != ESensorStatus::SENSOR_OK) {
        this->m_nLastData = millis();
        return;
    }

    icm_20948_DMP_data_t data;
    this->m_oImu.readDMPdataFromFIFO(&data);

    if ((this->m_oImu.status == ICM_20948_Stat_Ok) ||
        (this->m_oImu.status == ICM_20948_Stat_FIFOMoreDataAvail)) // Was valid data available?
    {
        // SERIAL_PORT.print(F("Received data! Header: 0x")); // Print the header in HEX so we can see what data is arriving in the FIFO
        // if ( data.header < 0x1000) SERIAL_PORT.print( "0" ); // Pad the zeros
        // if ( data.header < 0x100) SERIAL_PORT.print( "0" );
        // if ( data.header < 0x10) SERIAL_PORT.print( "0" );
        // SERIAL_PORT.println( data.header, HEX );

        if ((data.header & DMP_header_bitmap_Quat9) >
            0) // We have asked for orientation data so we should receive Quat9
        {
            // Q0 value is computed from this equation: Q0^2 + Q1^2 + Q2^2 + Q3^2 = 1.
            // In case of drift, the sum will not add to 1, therefore, quaternion data need to be corrected with right bias values.
            // The quaternion data is scaled by 2^30.

            // Scale to +/- 1
            double q1 = ((double) data.Quat9.Data.Q1) / 1073741824.0; // Convert to double. Divide by 2^30
            double q2 = ((double) data.Quat9.Data.Q2) / 1073741824.0; // Convert to double. Divide by 2^30
            double q3 = ((double) data.Quat9.Data.Q3) / 1073741824.0; // Convert to double. Divide by 2^30
            double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));

            CSensor::SQuat grQuat;
            grQuat.dX = q1;
            grQuat.dY = q2;
            grQuat.dZ = q3;
            grQuat.dW = q0;

            if (m_bIsFirstRead) {
                this->m_bIsFirstRead = false;
            }

            Packet::SensorData::sendSensorRotation(this->m_bId, data.Quat9.Data.Accuracy, grQuat);
            this->m_nLastData = millis();
        }
    }

    if ((millis() - this->m_nLastData) > SENSOR_TIMEOUT && !m_bIsFirstRead) {
        CLogger::instance()->error("Sensor timeout I2C Address 0x%02x", this->m_bAddress);
        this->m_eState = ESensorStatus::SENSOR_ERROR;
    }
}