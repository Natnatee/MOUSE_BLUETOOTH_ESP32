#pragma once
#include <Arduino.h>

// เลือกโหมดการทำงาน
enum class control_mode_t {
    SERIAL_ONLY,   // รับคำสั่งตรงจากคอม
    SENSOR_MOUSE,  // ใช้ Gyro คุมเมาส์
    SENSOR_KEYS    // ใช้ Gyro แทนปุ่ม (เช่น เอียงซ้าย = ปุ่มลูกศรซ้าย)
};

void controller_init();
void controller_update();
void controller_set_mode(control_mode_t mode);
