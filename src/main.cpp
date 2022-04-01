#include "defines.h"

#include "logging/Logger.h"
#include "network/network.h"
#include "sensors/sensorManager.h"
#include "network/wifi/wifihandler.h"
#include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(serialBaudRate);
    Serial.println();
    Serial.println();
    Serial.println();

    CLogger::instance()->info("SlimeVR v" FIRMWARE_VERSION " starting up...");

    // I2C settings
    Wire.begin(PIN_IMU_SDA, PIN_IMU_SCL);
    Wire.setClock(I2C_SPEED);

    // Wait for IMU to boot
    delay(500);

    // Initialize sensors
    CSensorManager::instance()->initialize();

    // Initialize wifi
    CNetwork::instance()->initialize();
}

void loop() {

//    if (CSensorManager::instance()->getFirst()->getSensorState() == CSensor::SENSOR_OK &&
//        CWiFiNetwork::instance()->getIsSlimeServerConnected()) {
//        CSensorManager::instance()->getFirst()->sendSensorData();
//        sleep(1);
//    }
}