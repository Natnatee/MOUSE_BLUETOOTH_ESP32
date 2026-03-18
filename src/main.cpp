#include <Arduino.h>
#include "mouse_bt.h"
#include "input_controller.h"
#include "serial_input.h"

void setup() {
    // กฎ rule.md: สำหรับ /flash_monitor ต้องมี delay(6000)
    delay(6000);
    
    // 0. เริ่มระบบ Serial
    serial_input_init(115200);

    // 1. เริ่มระบบบลูทูธ (Action Provider)
    mouse_bt_init();
    
    // 2. เริ่มระบบคำนวณลอจิก (Logic Controller)
    controller_init();
    
    Serial.println(">>> System Ready! <<<");
}

void loop() {
    // แค่เรียก Controller ให้ทำงาน
    controller_update();
    
    // Non-blocking loop
}
