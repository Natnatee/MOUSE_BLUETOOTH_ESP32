#include "state_manager.h"

SystemState current_state = SystemState::STATE_PLAY;

void state_manager_init() {
    current_state = SystemState::STATE_PLAY;
    Serial.println("System State Initialized: PLAY");
}

SystemState get_current_state() {
    return current_state;
}

void switch_state(SystemState new_state) {
    current_state = new_state;
    Serial.printf("State Changed to: %d\n", (int)current_state);
}

void state_manager_run() {
    // FSM Logic will go here
    switch(current_state) {
        case SystemState::STATE_PLAY:
            // Handle regular inputs mapping to HID
            break;
        case SystemState::STATE_SETTING_MAIN:
            // Handle setting menu navigation
            break;
        default:
            break;
    }
}
