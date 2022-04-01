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

#include "mpu6050sensor.h"
#include "../network/packet/send/sensorDataSend.h"
#include "sensorManager.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE

#include "Wire.h"

#endif

// MPU dmp control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

MPU6050 g_oImu;

void CMPU6050sensor::motionSetup() {

    g_oImu = MPU6050(this->m_bAddress);
    g_oImu.initialize();
    if (!g_oImu.testConnection()) {
        CLogger::instance()->fatal("Can't connect to MPU6050 (0x%02x) at address 0x%02x", g_oImu.getDeviceID(),
                                   this->m_bAddress);
        return;
    }

    CLogger::instance()->info("Connected to MPU6050 (0x%02x) at address 0x%02x", g_oImu.getDeviceID(),
                              this->m_bAddress);

    devStatus = g_oImu.dmpInitialize();

    if (devStatus == 0) {
#ifdef IMU_MPU6050_RUNTIME_CALIBRATION
        // We don't have to manually calibrate if we are using the dmp's automatic calibration
#else  // IMU_MPU6050_RUNTIME_CALIBRATION

        CLogger::instance()->debug("Performing startup calibration of accel and gyro...");
        // Do a quick and dirty calibration. As the imu warms up the offsets will change a bit, but this will be good-enough
        delay(1000); // A small sleep to give the users a chance to stop it from moving

        g_oImu.CalibrateGyro(6);
        g_oImu.CalibrateAccel(6);
        g_oImu.PrintActiveOffsets();
#endif // IMU_MPU6050_RUNTIME_CALIBRATION


        // turn on the DMP, now that it's ready
        CLogger::instance()->debug("Enabling DMP...");
        g_oImu.setDMPEnabled(true);

        // TODO: Add interrupt support
        // mpuIntStatus = g_oImu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        CLogger::instance()->debug("DMP ready! Waiting for first interrupt...");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = g_oImu.dmpGetFIFOPacketSize();

        this->m_eState = CSensor::SENSOR_OK;
        this->m_nLastData = millis();

    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        CLogger::instance()->error("DMP Initialization failed (code %d)", devStatus);
    }

}

void CMPU6050sensor::sendSensorData() {
    if (!dmpReady)
        return;

    if (g_oImu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        Quaternion rawQuat;
        CSensor::SQuat grQuaternion{};

        g_oImu.dmpGetQuaternion(&rawQuat, fifoBuffer);
        grQuaternion.dX = rawQuat.x;
        grQuaternion.dY = -rawQuat.y;
        grQuaternion.dZ = rawQuat.z;
        grQuaternion.dW = rawQuat.w;

        if (m_bIsFirstRead) {
            this->m_bIsFirstRead = false;
        }

        Packet::SensorData::sendSensorRotation(this->m_bId, 0, grQuaternion);
        this->m_nLastData = millis();
    } else if ((millis() - this->m_nLastData) > SENSOR_TIMEOUT && !m_bIsFirstRead) {
        CLogger::instance()->error("Sensor timeout I2C Address 0x%02x", this->m_bAddress);
        this->m_eState = ESensorStatus::SENSOR_ERROR;
    }

};
