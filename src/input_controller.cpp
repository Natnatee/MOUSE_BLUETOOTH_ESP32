#include "input_controller.h"
#include "mpu_sensor.h"
#include "mouse_bt.h"
#include "serial_input.h"

// ค่าตัวแปรภายใน
control_mode_t current_mode = control_mode_t::SENSOR_MOUSE;
bool mpu_available = false;

void controller_init() {
    mpu_available = mpu_init();
    Serial.println("Controller: Core system initialized!");
}

void controller_set_mode(control_mode_t mode) {
    current_mode = mode;
    Serial.print("Controller: Mode changed to ");
    if (mode == control_mode_t::SERIAL_ONLY) Serial.println("Serial Only");
    if (mode == control_mode_t::SENSOR_MOUSE) Serial.println("Air Mouse");
    if (mode == control_mode_t::SENSOR_KEYS) Serial.println("Tilt Keys");
}

void controller_update() {
    // 0. แจ้งเตือนสถานะ Bluetooth ทันทีเมื่อมีการเชื่อมต่อ/ตัดการเชื่อมต่อ
    static bool last_connected = false;
    bool current_connected = mouse_bt_is_connected();
    if (current_connected != last_connected) {
        if (current_connected) Serial.println(">>> Bluetooth Mouse CONNECTED! <<<");
        else Serial.println(">>> Bluetooth Mouse DISCONNECTED! <<<");
        last_connected = current_connected;
    }

    // 1. รับคำสั่งจาก USB/Serial ตลอดเวลา (เพราะเป็น Direct Command)
    serial_input_process();

    // 2. ลอจิกสำหรับ Sensor (ถ้ามีเซนเซอร์)
    if (mpu_available && current_connected) {
        static unsigned long last_check = 0;
        if (millis() - last_check > 20) {
            sensor_data_t data = mpu_get_data();
            
            // --- แยก Logic ตามโหมด ---
            if (current_mode == control_mode_t::SENSOR_MOUSE) {
                // เอียง MPU = ขยับเมาส์
                int8_t mx = (int8_t)(data.gz / 512);
                int8_t my = (int8_t)(data.gx / 512);
                if (mx != 0 || my != 0) {
                    mouse_move(mx, my);
                }
            } 
            else if (current_mode == control_mode_t::SENSOR_KEYS) {
                // เอียง MPU = กดปุ่ม (Demo Logic)
                if (data.gz > 10000) {
                    Serial.println("Action: Tilt Left Detected!");
                    // keyboard_press(LEFT_ARROW); // ทำต่อในอนาคต
                }
            }
            
            last_check = millis();
        }
    }
}
