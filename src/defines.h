#pragma once
/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2021 Eiren Rain

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
// Constants
#define IMU_NONE 0
#define IMU_MPU9250 1
#define IMU_MPU6500 2
#define IMU_BNO080 3
#define IMU_BNO085 4
#define IMU_BNO055 5
#define IMU_MPU6050 6
#define IMU_BNO086 7
#define IMU_BMI160 8
#define IMU_ICM20948 9

#define DEG_0 0.f
#define DEG_90 -PI / 2
#define DEG_180 PI
#define DEG_270 PI / 2

// IMU
#define IMU IMU_ICM20948
#define SECOND_IMU IMU_NONE
#define IMU_ROTATION DEG_90
#define SECOND_IMU_ROTATION DEG_90
#define I2C_SPEED 400000
#define USE_6_AXIS true // uses 9 (with mag) if false (only for ICM-20948 currently)
#define SENSOR_TIMEOUT 2000

// Something more leisurely for the development
#define SAMPLING_RATE_IN_MS 10

// Pins
#define PIN_IMU_SDA 21
#define PIN_IMU_SCL 22
#define PIN_IMU_INT 23
#define PIN_IMU_INT_2 25
#define PIN_BATTERY_LEVEL 36

// Debug
#define serialBaudRate 115200
#define FIRMWARE_VERSION "LC-0.0.1"

// Logger
#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5

#define LOG_LEVEL LOG_LEVEL_TRACE

// Network
#define HOTSPOT_PASSWORD "SlimeVR-Hotspot"
#define UDP_PORT 6969
#define SLIME_SERVER_TIMEOUT_MS 10000
#define USE_WIFI_DATA true
#define WIFI_SSID ""
#define WIFI_PASSWORD ""