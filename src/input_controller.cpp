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

bool mpu_available = false;

// Global buttons
bool last_btn_up = false;
bool last_btn_down = false;
bool last_btn_ok = false;
bool last_btn_back = false;

// UI State trackers
int last_mouse_x = 0;
int last_mouse_y = 0;
char last_key_pressed = 0;

void request_ui_update() {
    SystemState state = get_current_state();
    bool conn = mouse_bt_is_connected();
    int current_vol = map(hw_read_pot(), 0, 4095, 0, 100);
    
    if (state == SystemState::STATE_PLAY) {
        ProfileData* p = get_current_profile();
        display_play_mode(p->name, conn, last_mouse_x, last_mouse_y, last_key_pressed, current_vol);
    } else {
        display_setting_mode("MAIN MENU", current_vol, conn);
    }
}

void controller_init() {
    mpu_available = mpu_init();
    joystick_init();
    hw_inputs_init();
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
            acted = true; 
        }
    } 
    else if (state == SystemState::STATE_SETTING_MAIN) {
        if (bs.back && !last_btn_back) { 
            switch_state(SystemState::STATE_PLAY); 
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
    bool current_connected = mouse_bt_is_connected();
    if (current_connected != last_connected) {
        if (current_connected) {
            Serial.println(">>> Bluetooth CONNECTED! <<<");
        } else {
            Serial.println(">>> Bluetooth DISCONNECTED! <<<");
        }
        request_ui_update();
        last_connected = current_connected;
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

    if (get_current_state() == SystemState::STATE_PLAY && current_connected) {
        ProfileData* p = get_current_profile();
        
        static unsigned long last_check = 0;
        if (millis() - last_check > 20) {
            
            int8_t mouse_x = 0;
            int8_t mouse_y = 0;
            bool trigger_mouse_click = false;
            
            // === MOUSE PROCESSING ===
            if (p->mouse_input == InputSource::MPU6050 && mpu_available) {
                sensor_data_t data = mpu_get_data();
                mouse_x = (int8_t)(data.gz / MPU_DIVIDER);
                mouse_y = (int8_t)(data.gx / MPU_DIVIDER);
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
            if (trigger_mouse_click != last_mouse_click) {
                if (trigger_mouse_click) { mouse_press(MOUSE_LEFT); last_key_pressed = 'M'; ui_needs_update = true; }
                else mouse_release(MOUSE_LEFT);
                last_mouse_click = trigger_mouse_click;
            }
            
            // === KEYBOARD PROCESSING ===
            char key_this_frame = 0;
            if (p->keyboard_input == InputSource::BUTTON_SET_2) {
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

    // --- Rate Limit UI Refresh (Max 10 FPS) ---
    // ป้องกันจอ I2C ทำให้อ่านค่าเมาส์ช้าลง (หน่วง)
    static unsigned long last_ui_refresh = 0;
    if (ui_needs_update && millis() - last_ui_refresh > 100) {
        request_ui_update();
        last_ui_refresh = millis();
    }
}
