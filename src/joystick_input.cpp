#include "joystick_input.h"
#include "config.h"

// ค่ากลางของ Analog (จะถูกอัปเดตตอน Calibrate)
int centerX = 2048;
int centerY = 2048;

void joystick_init() {
    pinMode(JOY_VRX_PIN, INPUT);
    pinMode(JOY_VRY_PIN, INPUT);
    pinMode(JOY_SW_PIN, INPUT_PULLUP);
    
    joystick_calibrate(); // ทำการ Calibrate ตอนเริ่มต้น
    Serial.println("Joystick: HW-504 Initialized!");
}

void joystick_calibrate() {
    long sumX = 0;
    long sumY = 0;
    const int SAMPLES = 100;

    Serial.println("Joystick: Calibrating... (Keep it at center)");
    for(int i = 0; i < SAMPLES; i++) {
        sumX += analogRead(JOY_VRX_PIN);
        sumY += analogRead(JOY_VRY_PIN);
        delay(5);
    }
    centerX = sumX / SAMPLES;
    centerY = sumY / SAMPLES;
    
    Serial.printf("Joystick: Calibration Done! CenterX: %d, CenterY: %d\n", centerX, centerY);
}

joystick_data_t joystick_read() {
    joystick_data_t data;

    // อ่านค่า Analog
    int raw_x = analogRead(JOY_VRX_PIN);
    int raw_y = analogRead(JOY_VRY_PIN);
    
    // คำนวณความต่างจากจุดกึ่งกลางที่ Calibration ไว้
    data.x = raw_x - centerX;
    data.y = raw_y - centerY;

    // ระบบ Deadzone
    if (abs(data.x) < JOY_DEAD_ZONE) data.x = 0;
    if (abs(data.y) < JOY_DEAD_ZONE) data.y = 0;

    // อ่านค่าปุ่ม (SW_PIN เป็น LOW เมื่อกด เพราะใช้ INPUT_PULLUP)
    data.click = (digitalRead(JOY_SW_PIN) == LOW);

    return data;
}
