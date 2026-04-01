#include "input_controller.h"
#include "mpu_sensor.h"
#include "mouse_bt.h"
#include "serial_input.h"
#include "joystick_input.h"
#include "display_module.h"
#include "config.h"
#include "profile_manager.h"
#include "state_manager.h"
#include "hw_inputs.h"
#include "force_sensor.h"

bool mpu_available = false;
bool volume_synced = false;
int last_vol_sent = 0;

// Global buttons
bool last_btn_up = false;
bool last_btn_down = false;
bool last_btn_ok = false;
bool last_btn_back = false;

// UI State trackers
int last_mouse_x = 0;
int last_mouse_y = 0;
char last_key_pressed = 0;
int setting_menu_idx = 0;
int setting_profile_menu_idx = 0;
int selected_profile_idx = 0;
bool setting_delete_cancel_selected = true;

// Name Edit State trackers
bool is_adding_new_profile = false;
char name_edit_buf[PROFILE_NAME_LEN];
int name_edit_cursor = 0;
int name_edit_char_idx = 1; // 0 = space, 1-26 = A-Z, Default to 1 (A)

char get_alphabet_char(int idx) {
    if (idx == 0) return ' ';
    if (idx >= 1 && idx <= 26) return (char)('A' + idx - 1);
    return ' ';
}

int get_char_idx(char c) {
    if (c >= 'A' && c <= 'Z') return (c - 'A' + 1);
    if (c >= 'a' && c <= 'z') return (c - 'a' + 1);
    return 0; // Space
}

void request_ui_update() {
    SystemState state = get_current_state();
    bool conn = mouse_bt_is_connected();
    int current_vol = map(hw_read_pot(), 0, 4095, 0, 100);
    int current_force = force_read_analog();
    
    if (state == SystemState::STATE_PLAY) {
        ProfileData* p = get_current_profile();
        display_play_mode(p->name, conn, last_mouse_x, last_mouse_y, last_key_pressed, current_vol, current_force);
    } else if (state == SystemState::STATE_SETTING_MAIN) {
        display_setting_main(setting_menu_idx, conn);
    } else if (state == SystemState::STATE_SETTING_PROFILE) {
        ProfileData* p = get_profile(selected_profile_idx);
        if (p) {
            display_setting_profile(p->name, setting_profile_menu_idx, conn);
        }
    } else if (state == SystemState::STATE_SETTING_DELETE_CONFIRM) {
        ProfileData* p = get_profile(selected_profile_idx);
        if (p) {
            display_setting_delete_confirm(p->name, setting_delete_cancel_selected);
        }
    } else if (state == SystemState::STATE_SETTING_NAME) {
        bool blink = (millis() / 500) % 2 == 0;
        char current_char = get_alphabet_char(name_edit_char_idx);
        display_setting_name(name_edit_buf, name_edit_cursor, current_char, blink);
    }
}

void controller_init() {
    mpu_available = mpu_init();
    joystick_init();
    hw_inputs_init();
    force_sensor_init();
    profile_manager_init();
    state_manager_init();
    
    Serial.println("Controller: Core system initialized!");
    request_ui_update();
}

void process_global_buttons(ButtonState bs) {
    SystemState state = get_current_state();
    static unsigned long last_action = 0;
    
    // Debounce 250ms
    if (millis() - last_action < 250) {
        last_btn_up = bs.up;
        last_btn_down = bs.down;
        last_btn_back = bs.back;
        last_btn_ok = bs.ok;
        return;
    }

    bool acted = false;

    if (state == SystemState::STATE_PLAY) {
        if (bs.up && !last_btn_up) { prev_profile(); acted = true; }
        else if (bs.down && !last_btn_down) { next_profile(); acted = true; }
        else if (bs.back && !last_btn_back) { 
            switch_state(SystemState::STATE_SETTING_MAIN); 
            setting_menu_idx = 0; // Reset cursor
            acted = true; 
        }
        
        // Long Press OK to Calibrate MPU
        static unsigned long ok_press_start = 0;
        static bool ok_long_handled = false;
        if (bs.ok && !last_btn_ok) { ok_press_start = millis(); ok_long_handled = false; }
        if (bs.ok && last_btn_ok && !ok_long_handled && (millis() - ok_press_start > 1000)) {
            mpu_calibrate();
            ok_long_handled = true;
            last_key_pressed = 'C'; // UI Feedback: 'C' for Calibrate
            acted = true;
        }
    } 
    else if (state == SystemState::STATE_SETTING_MAIN) {
        if (bs.up && !last_btn_up) {
            if (setting_menu_idx > 0) setting_menu_idx--;
            else setting_menu_idx = get_active_profiles_count(); // Wrap around to bottom
            acted = true;
        }
        else if (bs.down && !last_btn_down) {
            if (setting_menu_idx < get_active_profiles_count()) setting_menu_idx++;
            else setting_menu_idx = 0; // Wrap around to top
            acted = true;
        }
        else if (bs.ok && !last_btn_ok) {
            if (setting_menu_idx > 0) { // If it's an existing profile (1, 2, 3)
                selected_profile_idx = setting_menu_idx - 1;
                setting_profile_menu_idx = 0; // Reset sub-menu cursor
                switch_state(SystemState::STATE_SETTING_PROFILE);
                acted = true;
            } else { // setting_menu_idx == 0 (Add New Profile)
                if (get_active_profiles_count() < MAX_PROFILES) {
                    is_adding_new_profile = true;
                    memset(name_edit_buf, 0, PROFILE_NAME_LEN);
                    name_edit_cursor = 0;
                    name_edit_char_idx = 1; // Start with 'A'
                    switch_state(SystemState::STATE_SETTING_NAME);
                    acted = true;
                }
            }
        }
        else if (bs.back && !last_btn_back) { 
            switch_state(SystemState::STATE_PLAY); 
            acted = true; 
        }
    }
    else if (state == SystemState::STATE_SETTING_PROFILE) {
        if (bs.up && !last_btn_up) {
            if (setting_profile_menu_idx > 0) setting_profile_menu_idx--;
            else setting_profile_menu_idx = 3; // Wrap around MOUSE, KEYBOARD, NAME, DELETE
            acted = true;
        }
        else if (bs.down && !last_btn_down) {
            if (setting_profile_menu_idx < 3) setting_profile_menu_idx++;
            else setting_profile_menu_idx = 0;
            acted = true;
        }
        else if (bs.ok && !last_btn_ok) {
            if (setting_profile_menu_idx == 3) { // DELETE
                setting_delete_cancel_selected = true; // Default to cancel
                switch_state(SystemState::STATE_SETTING_DELETE_CONFIRM);
            }
            else if (setting_profile_menu_idx == 2) { // NAME
                ProfileData* p = get_profile(selected_profile_idx);
                if (p) {
                    memset(name_edit_buf, 0, PROFILE_NAME_LEN);
                    strncpy(name_edit_buf, p->name, PROFILE_NAME_LEN - 1);
                    name_edit_cursor = strlen(name_edit_buf);
                    if (name_edit_cursor >= PROFILE_NAME_LEN - 1) name_edit_cursor = PROFILE_NAME_LEN - 2;
                    name_edit_char_idx = get_char_idx(name_edit_buf[name_edit_cursor]);
                    if (name_edit_char_idx == 0) name_edit_char_idx = 1; // Default to 'A'
                }
                switch_state(SystemState::STATE_SETTING_NAME);
            }
            acted = true;
        }
        else if (bs.back && !last_btn_back) {
            switch_state(SystemState::STATE_SETTING_MAIN);
            acted = true;
        }
    }
    else if (state == SystemState::STATE_SETTING_DELETE_CONFIRM) {
        // Use Up/Down logic to mimic Left/Right layout behavior
        if ((bs.up && !last_btn_up) || (bs.down && !last_btn_down)) {
            setting_delete_cancel_selected = !setting_delete_cancel_selected;
            acted = true;
        }
        else if (bs.ok && !last_btn_ok) {
            if (!setting_delete_cancel_selected) {
                delete_profile(selected_profile_idx);
                switch_state(SystemState::STATE_SETTING_MAIN);
            } else {
                switch_state(SystemState::STATE_SETTING_PROFILE);
            }
            acted = true;
        }
        else if (bs.back && !last_btn_back) {
            switch_state(SystemState::STATE_SETTING_PROFILE);
            acted = true;
        }
    }
    else if (state == SystemState::STATE_SETTING_NAME) {
        if (bs.up && !last_btn_up) {
            name_edit_char_idx++;
            if (name_edit_char_idx > 26) name_edit_char_idx = 0;
            acted = true;
        }
        else if (bs.down && !last_btn_down) {
            if (name_edit_char_idx > 0) name_edit_char_idx--;
            else name_edit_char_idx = 26;
            acted = true;
        }
        
        static unsigned long name_ok_press_time = 0;
        static bool name_ok_long_handled = false;
        
        if (bs.ok && !last_btn_ok) {
            name_ok_press_time = millis();
            name_ok_long_handled = false;
        }
        
        if (bs.ok && last_btn_ok) {
            if (!name_ok_long_handled && (millis() - name_ok_press_time > 800)) {
                // Long Press -> Backspace
                name_ok_long_handled = true;
                if (name_edit_cursor > 0) {
                    name_edit_cursor--;
                    name_edit_char_idx = get_char_idx(name_edit_buf[name_edit_cursor]);
                    if (name_edit_char_idx == 0) name_edit_char_idx = 1; // Default to 'A'
                }
                acted = true;
            }
        }
        
        if (!bs.ok && last_btn_ok) {
            if (!name_ok_long_handled && (millis() - name_ok_press_time > 20)) {
                // Short Press -> Next
                name_edit_buf[name_edit_cursor] = get_alphabet_char(name_edit_char_idx);
                if (name_edit_cursor < PROFILE_NAME_LEN - 2) {
                    name_edit_cursor++;
                    name_edit_char_idx = get_char_idx(name_edit_buf[name_edit_cursor]); 
                    if (name_edit_char_idx == 0) name_edit_char_idx = 1; // Default to 'A'
                }
                acted = true;
            }
        }
        
        if (bs.back && !last_btn_back) {
            // Guarantee null termination at the end of buffer length
            name_edit_buf[PROFILE_NAME_LEN - 1] = '\0'; 
            
            // If the user presses back, the current un-confirmed char is IGNORED, 
            // but we ensure the string cuts off cleanly at the cursor
            name_edit_buf[name_edit_cursor] = '\0';
            
            if (is_adding_new_profile) {
                if (add_new_profile(name_edit_buf)) {
                    selected_profile_idx = get_active_profiles_count() - 1;
                    setting_profile_menu_idx = 0; // Start focus on MOUSE
                    switch_state(SystemState::STATE_SETTING_PROFILE);
                } else {
                    switch_state(SystemState::STATE_SETTING_MAIN); // Failed
                }
                is_adding_new_profile = false;
            } else {
                ProfileData* p = get_profile(selected_profile_idx);
                if (p) {
                    strncpy(p->name, name_edit_buf, PROFILE_NAME_LEN);
                }
                switch_state(SystemState::STATE_SETTING_PROFILE);
            }
            acted = true;
        }
    }

    if (acted) {
        last_action = millis();
        request_ui_update();
    }

    last_btn_up = bs.up;
    last_btn_down = bs.down;
    last_btn_back = bs.back;
    last_btn_ok = bs.ok;
}

void controller_update() {
    static bool last_connected = false;
    static bool pending_sync = false;
    static unsigned long connect_time = 0;
    
    bool current_connected = mouse_bt_is_connected();
    if (current_connected != last_connected) {
        if (current_connected) {
            Serial.println(">>> Bluetooth CONNECTED! <<<");
            pending_sync = true;
            connect_time = millis();
        } else {
            Serial.println(">>> Bluetooth DISCONNECTED! <<<");
            volume_synced = false;
            pending_sync = false;
        }
        request_ui_update();
        last_connected = current_connected;
    }

    // Process Force Sync after 2000ms
    if (pending_sync && current_connected && (millis() - connect_time > 2000)) {
        Serial.println("Force Syncing Volume to Host...");
        for (int i = 0; i < 50; i++) { 
            keyboard_volume_down();
            delay(5);
        }
        int initial_vol = map(hw_read_pot(), 0, 4095, 0, 100);
        int steps_up = initial_vol / 2; 
        for (int i = 0; i < steps_up; i++) {
            keyboard_volume_up();
            delay(5);
        }
        Serial.println("Volume Synced!");
        last_vol_sent = initial_vol;
        volume_synced = true;
        pending_sync = false;
    }

    serial_input_process();
    
    ButtonState bs = hw_read_buttons();
    process_global_buttons(bs);

    bool ui_needs_update = false;

    // Track Potentiometer for UI Volume Changes 
    static int last_vol_disp = map(hw_read_pot(), 0, 4095, 0, 100);
    int curr_vol = map(hw_read_pot(), 0, 4095, 0, 100);
    if (abs(curr_vol - last_vol_disp) >= 2) {
        last_vol_disp = curr_vol;
        ui_needs_update = true;
    }

    // Host Volume Adjust Logic
    if (current_connected && volume_synced) {
        // อัปเดตเมื่อหมุนต่างกัน >= 4% (2 แกร๊ก) หรือเมื่อหมุนสุดขอบที่ 0% หรือ 100%
        if (abs(curr_vol - last_vol_sent) >= 4 || (curr_vol == 0 && last_vol_sent > 0) || (curr_vol == 100 && last_vol_sent < 100)) {
            
            int diff = curr_vol - last_vol_sent;
            int steps = abs(diff) / 2; // Windows ขยับทีละ 2% ต่อการกด 1 ครั้ง
            
            // กรณีหมุนสุดขอบ 0 หรือ 100 แต่ diff หาร 2 แล้วไม่ได้สัก step ให้อย่างน้อยส่ง 1 ครั้ง
            if (steps == 0 && abs(diff) > 0 && (curr_vol == 0 || curr_vol == 100)) {
                steps = 1;
            }

            if (steps > 0) {
                if (diff > 0) {
                    for (int i = 0; i < steps; i++) {
                        keyboard_volume_up();
                        delay(2);
                    }
                    last_vol_sent += (steps * 2); // บวกเฉพาะจำนวน% ที่สอดคล้องกับครั้งที่กดจริง
                } else {
                    for (int i = 0; i < steps; i++) {
                        keyboard_volume_down();
                        delay(2);
                    }
                    last_vol_sent -= (steps * 2); // ลบเฉพาะจำนวน% ที่สอดคล้องกับครั้งที่กดจริง
                }
            }
        }
    }

    // Track Force Sensor for UI Changes
    static int last_force_disp = 0;
    int curr_force = force_read_analog();
    if (abs(curr_force - last_force_disp) >= 10) {
        last_force_disp = curr_force;
        ui_needs_update = true;
    }

    if (get_current_state() == SystemState::STATE_PLAY && current_connected) {
        ProfileData* p = get_current_profile();
        
        static unsigned long last_check = 0;
        if (millis() - last_check > 20) {
            
            int8_t mouse_x = 0;
            int8_t mouse_y = 0;
            bool trigger_mouse_click = false;
            
            // === MOUSE PROCESSING ===
            if (p->mouse_input == InputSource::MPU6050 && mpu_available) {
                sensor_data_t data = mpu_get_data_clean(); // ใช้ค่าที่ผ่านการเซตศูนย์แล้ว
                
                // เพิ่มความไว: X +30%, Y +10%
                mouse_x = (int8_t)(data.gz * 1.3f / MPU_DIVIDER);
                mouse_y = (int8_t)-(data.gy * 1.1f / MPU_DIVIDER);

                // MPU Mouse Clicks (Button A -> Left, Button B -> Right)
                static bool last_l_click = false;
                static bool last_r_click = false;
                
                if (bs.conf1 != last_l_click) {
                    if (bs.conf1) { mouse_press(MOUSE_LEFT); last_key_pressed = 'M'; ui_needs_update = true; }
                    else mouse_release(MOUSE_LEFT);
                    last_l_click = bs.conf1;
                }
                
                if (bs.conf2 != last_r_click) {
                    if (bs.conf2) { mouse_press(MOUSE_RIGHT); last_key_pressed = 'M'; ui_needs_update = true; }
                    else mouse_release(MOUSE_RIGHT);
                    last_r_click = bs.conf2;
                }
            } 
            else if (p->mouse_input == InputSource::JOYSTICK) {
                joystick_data_t joy = joystick_read();
                mouse_x = (int8_t)(joy.x / JOY_DIVIDER);
                mouse_y = (int8_t)(joy.y / JOY_DIVIDER);
                trigger_mouse_click = joy.click;
            }

            if (mouse_x != 0 || mouse_y != 0) {
                mouse_move(mouse_x, mouse_y);
                last_mouse_x = mouse_x;
                last_mouse_y = mouse_y;
                ui_needs_update = true;
            }

            static bool last_mouse_click = false;
            // Existing logic for Joystick Click
            if (p->mouse_input == InputSource::JOYSTICK && trigger_mouse_click != last_mouse_click) {
                if (trigger_mouse_click) { mouse_press(MOUSE_LEFT); last_key_pressed = 'M'; ui_needs_update = true; }
                else mouse_release(MOUSE_LEFT);
                last_mouse_click = trigger_mouse_click;
            }
            
            // === KEYBOARD PROCESSING ===
            char key_this_frame = 0;
            if (p->keyboard_input == InputSource::MPU6050) {
                // มุมเอียงเลี้ยว (20 องศา)
                const int TILT_THRESHOLD = 5600; 
                sensor_data_t data = mpu_get_data_clean();
                int current_force = force_read_analog();
                joystick_data_t joy = joystick_read();

                static bool last_a = false, last_d = false, last_w = false;
                static bool last_t = false, last_g = false, last_c = false, last_p = false;
                
                // เลี้ยวซ้าย-ขวาจากการเอียง
                bool a = data.ay > TILT_THRESHOLD;  
                bool d = data.ay < -TILT_THRESHOLD; 
                
                // เดินหน้า (W) จาก FSR
                bool w = current_force > 3500;

                // Joystick Up/Down -> T/G
                bool t = joy.y < -JOY_DEAD_ZONE;
                bool g = joy.y > JOY_DEAD_ZONE;

                // Buttons -> C/P (Config 1 & 2)
                bool c = bs.conf1;
                bool p_btn = bs.conf2; // ใช้ชื่อ p_btn เพราะ p ถูกใช้ใน pointer

                if (a != last_a) { if (a) { keyboard_press('a'); key_this_frame = 'A'; } else keyboard_release('a'); last_a = a; }
                if (d != last_d) { if (d) { keyboard_press('d'); key_this_frame = 'D'; } else keyboard_release('d'); last_d = d; }
                if (w != last_w) { if (w) { keyboard_press('w'); key_this_frame = 'W'; } else keyboard_release('w'); last_w = w; }
                if (t != last_t) { if (t) { keyboard_press('t'); key_this_frame = 'T'; } else keyboard_release('t'); last_t = t; }
                if (g != last_g) { if (g) { keyboard_press('g'); key_this_frame = 'G'; } else keyboard_release('g'); last_g = g; }
                if (c != last_c) { if (c) { keyboard_press('c'); key_this_frame = 'C'; } else keyboard_release('c'); last_c = c; }
                if (p_btn != last_p) { if (p_btn) { keyboard_press('p'); key_this_frame = 'P'; } else keyboard_release('p'); last_p = p_btn; }
            }
            else if (p->keyboard_input == InputSource::BUTTON_SET_2) {
                static bool lc1 = false, lc2 = false, lc3 = false, lc4 = false;
                if (bs.conf1 != lc1) { if (bs.conf1) { keyboard_press('a'); key_this_frame = 'A'; } else keyboard_release('a'); lc1 = bs.conf1; }
                if (bs.conf2 != lc2) { if (bs.conf2) { keyboard_press('b'); key_this_frame = 'B'; } else keyboard_release('b'); lc2 = bs.conf2; }
                if (bs.conf3 != lc3) { if (bs.conf3) { keyboard_press('c'); key_this_frame = 'C'; } else keyboard_release('c'); lc3 = bs.conf3; }
                if (bs.conf4 != lc4) { if (bs.conf4) { keyboard_press('d'); key_this_frame = 'D'; } else keyboard_release('d'); lc4 = bs.conf4; }
            }
            else if (p->keyboard_input == InputSource::JOYSTICK) {
                joystick_data_t joy = joystick_read();
                static bool last_w = false, last_s = false, last_a = false, last_d = false;
                bool w = joy.y < -JOY_DEAD_ZONE;
                bool s = joy.y > JOY_DEAD_ZONE;
                bool a = joy.x < -JOY_DEAD_ZONE;
                bool d = joy.x > JOY_DEAD_ZONE;
                
                if (w != last_w) { if (w) { keyboard_press('w'); key_this_frame='W'; } else keyboard_release('w'); last_w = w; }
                if (s != last_s) { if (s) { keyboard_press('s'); key_this_frame='S'; } else keyboard_release('s'); last_s = s; }
                if (a != last_a) { if (a) { keyboard_press('a'); key_this_frame='A'; } else keyboard_release('a'); last_a = a; }
                if (d != last_d) { if (d) { keyboard_press('d'); key_this_frame='D'; } else keyboard_release('d'); last_d = d; }
            }

            // Keyboard UI display logic
            static unsigned long last_key_time = 0;
            if (key_this_frame != 0) {
                last_key_pressed = key_this_frame;
                last_key_time = millis();
                ui_needs_update = true;
            } else if (last_key_pressed != 0 && millis() - last_key_time > 500 && last_key_pressed != 'M') {
                last_key_pressed = 0;
                ui_needs_update = true;
            }

            last_check = millis();
        }
    }

    // --- Constant UI Refresh (10 FPS) ---
    // รีเฟรชจอทุกๆ 0.1 วิ เพื่อให้ค่าบนจอดู Smooth ตลอดเวลา
    static unsigned long last_ui_refresh = 0;
    if (millis() - last_ui_refresh > 100) {
        request_ui_update();
        last_ui_refresh = millis();
    }
}
