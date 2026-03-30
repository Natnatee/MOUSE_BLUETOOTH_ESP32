#include "mpu_sensor.h"
#include <MPU6050.h>
#include "config.h"

MPU6050 mpu;

bool mpu_init() {
    Wire.begin(SDA_PIN, SCL_PIN);
    mpu.initialize();
    
// ลองเช็ค Connection เบื้องต้น
    if (mpu.testConnection()) {
        Serial.println("MPU6050: System found sensor!");
        return true;
    }
    
    Serial.println("MPU6050: Sensor connection failed!");
    return false;
}

static sensor_data_t mpu_offset = {0, 0, 0, 0, 0, 0};

sensor_data_t mpu_get_data() {
    sensor_data_t data;
    mpu.getMotion6(&data.ax, &data.ay, &data.az, &data.gx, &data.gy, &data.gz);
    return data;
}

void mpu_calibrate() {
    mpu_offset = mpu_get_data();
    Serial.printf("MPU Calibrated! Offset: ax=%d, ay=%d, az=%d\n", mpu_offset.ax, mpu_offset.ay, mpu_offset.az);
}

sensor_data_t mpu_get_data_clean() {
    sensor_data_t raw = mpu_get_data();
    sensor_data_t clean;
    clean.ax = raw.ax - mpu_offset.ax;
    clean.ay = raw.ay - mpu_offset.ay;
    clean.az = raw.az - mpu_offset.az;
    clean.gx = raw.gx - mpu_offset.gx;
    clean.gy = raw.gy - mpu_offset.gy;
    clean.gz = raw.gz - mpu_offset.gz;
    return clean;
}
