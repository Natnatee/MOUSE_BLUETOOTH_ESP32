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

sensor_data_t mpu_get_data() {
    sensor_data_t data;
    mpu.getMotion6(&data.ax, &data.ay, &data.az, &data.gx, &data.gy, &data.gz);
    return data;
}
