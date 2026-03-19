#include "joystick_input.h"

// กำหนดขาพิน (ESP32)
#define VRX_PIN 34
#define VRY_PIN 35
#define SW_PIN  32

// ค่ากลางของ Analog (จะถูกอัปเดตตอน Calibrate)
int centerX = 2048;
int centerY = 2048;
const int DEAD_ZONE = 400; // เพิ่มระยะกันหลุดให้กว้างขึ้นหน่อย

void joystick_init() {
    pinMode(VRX_PIN, INPUT);
    pinMode(VRY_PIN, INPUT);
    pinMode(SW_PIN, INPUT_PULLUP);
    
    joystick_calibrate(); // ทำการ Calibrate ตอนเริ่มต้น
    Serial.println("Joystick: HW-504 Initialized!");
}

void joystick_calibrate() {
    long sumX = 0;
    long sumY = 0;
    const int SAMPLES = 100;

    Serial.println("Joystick: Calibrating... (Keep it at center)");
    for(int i = 0; i < SAMPLES; i++) {
        sumX += analogRead(VRX_PIN);
        sumY += analogRead(VRY_PIN);
        delay(5);
    }
    centerX = sumX / SAMPLES;
    centerY = sumY / SAMPLES;
    
    Serial.printf("Joystick: Calibration Done! CenterX: %d, CenterY: %d\n", centerX, centerY);
}

joystick_data_t joystick_read() {
    joystick_data_t data;

    // อ่านค่า Analog
    int raw_x = analogRead(VRX_PIN);
    int raw_y = analogRead(VRY_PIN);
    
    // คำนวณความต่างจากจุดกึ่งกลางที่ Calibration ไว้
    data.x = raw_x - centerX;
    data.y = raw_y - centerY;

    // ระบบ Deadzone
    if (abs(data.x) < DEAD_ZONE) data.x = 0;
    if (abs(data.y) < DEAD_ZONE) data.y = 0;

    // อ่านค่าปุ่ม (SW_PIN เป็น LOW เมื่อกด เพราะใช้ INPUT_PULLUP)
    data.click = (digitalRead(SW_PIN) == LOW);

    return data;
}
