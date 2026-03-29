#pragma once
#include <Arduino.h>

enum class SystemState {
    STATE_PLAY,
    STATE_SETTING_MAIN,
    STATE_SETTING_PROFILE,
    STATE_SETTING_MOUSE,
    STATE_SETTING_KEYBOARD,
    STATE_SETTING_NAME,
    STATE_SETTING_DELETE_CONFIRM
};

void state_manager_init();
void state_manager_run();
SystemState get_current_state();
void switch_state(SystemState new_state);
