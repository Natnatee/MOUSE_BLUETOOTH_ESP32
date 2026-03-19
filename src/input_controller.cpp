#include "input_controller.h"
#include "mpu_sensor.h"
#include "mouse_bt.h"
#include "serial_input.h"
#include "joystick_input.h"
#include "display_module.h"
#include "config.h"

// ค่าตัวแปรภายใน
control_mode_t current_mode = control_mode_t::SENSOR_MOUSE;
bool mpu_available = false;

void controller_init() {
    mpu_available = mpu_init();
    joystick_init();
    Serial.println("Controller: Core system initialized!");
}

void controller_set_mode(control_mode_t mode) {
    current_mode = mode;
    Serial.print("Controller: Mode changed to ");
    const char* mode_str = "";
    if (mode == control_mode_t::SERIAL_ONLY) mode_str = "Serial Only";
    else if (mode == control_mode_t::SENSOR_MOUSE) mode_str = "Air Mouse";
    else if (mode == control_mode_t::SENSOR_KEYS) mode_str = "Tilt Keys";
    else if (mode == control_mode_t::JOYSTICK_MOUSE) mode_str = "Joystick Mouse";
    
    Serial.println(mode_str);
    display_update(mode_str, 0, 0, mouse_bt_is_connected());
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
                int8_t mx = (int8_t)(data.gz / MPU_DIVIDER);
                int8_t my = (int8_t)(data.gx / MPU_DIVIDER);
                if (mx != 0 || my != 0) {
                    mouse_move(mx, my);
                    display_update("Air Mouse", mx, my, true);
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

    // 3. อ่านค่าจาก Joystick (ถ้าเชื่อมต่อ Bluetooth)
    if (current_connected && (current_mode == control_mode_t::JOYSTICK_MOUSE)) {
        static unsigned long last_joy_check = 0;
        if (millis() - last_joy_check > 20) { // อ่าน Joystick
            joystick_data_t joy = joystick_read();
            
            // --- ลองเปิด Serial เพื่อดูค่าดีบัก ---
            static unsigned long last_print = 0;
            if (millis() - last_print > 500) { // พิมพ์ทุก 0.5 วินาที
                Serial.printf("DEBUG JOY: X=%d, Y=%d, Click=%s\n", joy.x, joy.y, joy.click ? "YES" : "NO");
                last_print = millis();
            }

            // ขยับเมาส์
            int8_t mx = (int8_t)(joy.x / JOY_DIVIDER);
            int8_t my = (int8_t)(joy.y / JOY_DIVIDER);
            
            if (mx != 0 || my != 0) {
                mouse_move(mx, my);
                display_update("Joystick", mx, my, true);
            }

            // จัดการคลิ๊ก (SW)
            static bool last_sw_state = false;
            if (joy.click != last_sw_state) {
                if (joy.click) mouse_press(MOUSE_LEFT);
                else mouse_release(MOUSE_LEFT);
                last_sw_state = joy.click;
            }

            last_joy_check = millis();
        }
    }
}
