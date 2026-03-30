#pragma once
#include <Arduino.h>
#include <Wire.h>

struct sensor_data_t {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
};

bool mpu_init();
sensor_data_t mpu_get_data();
void mpu_calibrate();
sensor_data_t mpu_get_data_clean();
