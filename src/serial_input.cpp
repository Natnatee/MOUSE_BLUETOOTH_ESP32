#include "serial_input.h"
#include "mouse_bt.h"
#include "input_controller.h"
#include "profile_manager.h"

void serial_input_init(unsigned long baud) {
    Serial.begin(baud);
    // ไม่บล็อก setup() นานเกินไป (ลบ while(!Serial) ออกตามความนิยม ESP32)
    Serial.println("Serial Input Ready!");
}

bool serial_input_available() {
    return Serial.available() > 0;
}

void serial_input_process() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        // --- คำสั่งเปลี่ยนโหมด ---
        if (cmd.startsWith("MODE ")) {
            next_profile();
            Serial.println("Serial Command: Switched Profile");
            return;
        }

        // --- คำสั่งเมาส์จากคอมฯ (ทำงานข้ามโหมดได้ตลอด) ---
        if (cmd.startsWith("M ")) {
            int first_space = cmd.indexOf(' ');
            int second_space = cmd.indexOf(' ', first_space + 1);
            int x = cmd.substring(first_space + 1, second_space).toInt();
            int y = cmd.substring(second_space + 1).toInt();
            mouse_move(x, y);
            Serial.printf("Serial Command: Move %d, %d\n", x, y);
        }
        else if (cmd.startsWith("C ")) {
            char btn = cmd.charAt(2);
            if (btn == 'L') mouse_click(MOUSE_LEFT);
            if (btn == 'R') mouse_click(MOUSE_RIGHT);
            if (btn == 'M') mouse_click(MOUSE_MIDDLE);
            Serial.printf("Serial Command: Click %c\n", btn);
        }
    }
}
