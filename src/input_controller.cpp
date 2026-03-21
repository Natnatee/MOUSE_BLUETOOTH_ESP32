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

// Helpers for debounce
bool last_btn_up = false;
bool last_btn_down = false;
bool last_btn_ok = false;
bool last_btn_back = false;

void controller_init() {
    mpu_available = mpu_init();
    joystick_init();
    hw_inputs_init();
    profile_manager_init();
    state_manager_init();
    
    Serial.println("Controller: Core system initialized!");
    // Initial display
    ProfileData* p = get_current_profile();
    display_update(p->name, 0, 0, mouse_bt_is_connected());
}

void process_global_buttons(ButtonState bs) {
    SystemState state = get_current_state();
    
    // PLAY Mode buttons
    if (state == SystemState::STATE_PLAY) {
        // Toggle Profiles
        if (bs.up && !last_btn_up) {
            prev_profile();
            ProfileData* p = get_current_profile();
            Serial.printf("Profile changed to: %s\n", p->name);
            display_update(p->name, 0, 0, mouse_bt_is_connected());
        }
        if (bs.down && !last_btn_down) {
            next_profile();
            ProfileData* p = get_current_profile();
            Serial.printf("Profile changed to: %s\n", p->name);
            display_update(p->name, 0, 0, mouse_bt_is_connected());
        }
        if (bs.back && !last_btn_back) {
            switch_state(SystemState::STATE_SETTING_MAIN);
            display_update("SETTING", 0, 0, mouse_bt_is_connected());
            Serial.println("Entered SETTING Mode");
        }
    } 
    // SETTING Mode buttons
    else if (state == SystemState::STATE_SETTING_MAIN) {
        if (bs.back && !last_btn_back) {
            switch_state(SystemState::STATE_PLAY);
            ProfileData* p = get_current_profile();
            display_update(p->name, 0, 0, mouse_bt_is_connected());
            Serial.println("Exited SETTING Mode");
        }
        // Menu navigation logic will go here
    }

    last_btn_up = bs.up;
    last_btn_down = bs.down;
    last_btn_ok = bs.ok;
    last_btn_back = bs.back;
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
        
        // Refresh display
        SystemState state = get_current_state();
        if (state == SystemState::STATE_PLAY) {
            display_update(get_current_profile()->name, 0, 0, current_connected);
        } else {
            display_update("SETTING", 0, 0, current_connected);
        }
        last_connected = current_connected;
    }

    serial_input_process();
    
    // Read HW 
    ButtonState bs = hw_read_buttons();
    process_global_buttons(bs);

    // Main Control Loop
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

            // Move Mouse
            if (mouse_x != 0 || mouse_y != 0) {
                mouse_move(mouse_x, mouse_y);
                display_update(p->name, mouse_x, mouse_y, true);
            }

            // Click Mouse
            static bool last_mouse_click = false;
            if (trigger_mouse_click != last_mouse_click) {
                if (trigger_mouse_click) mouse_press(MOUSE_LEFT);
                else mouse_release(MOUSE_LEFT);
                last_mouse_click = trigger_mouse_click;
            }
            
            // === KEYBOARD PROCESSING ===
            if (p->keyboard_input == InputSource::BUTTON_SET_2) {
                // Mock: Conf1 -> 'A', Conf2 -> 'B', Conf3 -> 'C', Conf4 -> 'D'
                static bool lc1 = false, lc2 = false, lc3 = false, lc4 = false;
                
                if (bs.conf1 != lc1) { if (bs.conf1) keyboard_press('a'); else keyboard_release('a'); lc1 = bs.conf1; }
                if (bs.conf2 != lc2) { if (bs.conf2) keyboard_press('b'); else keyboard_release('b'); lc2 = bs.conf2; }
                if (bs.conf3 != lc3) { if (bs.conf3) keyboard_press('c'); else keyboard_release('c'); lc3 = bs.conf3; }
                if (bs.conf4 != lc4) { if (bs.conf4) keyboard_press('d'); else keyboard_release('d'); lc4 = bs.conf4; }
            }
            else if (p->keyboard_input == InputSource::JOYSTICK) {
                // Mock: Map Joystick to WASD
                joystick_data_t joy = joystick_read();
                static bool last_w = false, last_s = false, last_a = false, last_d = false;
                bool w = joy.y < -JOY_DEAD_ZONE;
                bool s = joy.y > JOY_DEAD_ZONE;
                bool a = joy.x < -JOY_DEAD_ZONE;
                bool d = joy.x > JOY_DEAD_ZONE;
                
                if (w != last_w) { if (w) keyboard_press('w'); else keyboard_release('w'); last_w = w; }
                if (s != last_s) { if (s) keyboard_press('s'); else keyboard_release('s'); last_s = s; }
                if (a != last_a) { if (a) keyboard_press('a'); else keyboard_release('a'); last_a = a; }
                if (d != last_d) { if (d) keyboard_press('d'); else keyboard_release('d'); last_d = d; }
            }

            last_check = millis();
        }
    }
}
