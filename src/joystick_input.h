#pragma once
#include <Arduino.h>

struct joystick_data_t {
    int x;       // ค่าจาก Analog -512 ถึง 512 (หรือสเกลที่ต้องการ)
    int y;       // ค่าจาก Analog -512 ถึง 512
    bool click;  // สถานะปุ่ม SW
};

void joystick_init();
void joystick_calibrate(); // เพิ่มฟังก์ชันหาค่ากลางอัตโนมัติ
joystick_data_t joystick_read();
