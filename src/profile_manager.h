#pragma once
#include <Arduino.h>

#define MAX_PROFILES 3
#define PROFILE_NAME_LEN 12

// Enum defining the different input sources that can handle actions
enum class InputSource {
    NONE = 0,
    JOYSTICK,
    MPU6050,
    BUTTON_SET_2
};

struct ProfileData {
    char name[PROFILE_NAME_LEN];
    InputSource mouse_input;
    InputSource keyboard_input;
    uint8_t mpu_sensitivity;
};

void profile_manager_init();
ProfileData* get_current_profile();
ProfileData* get_profile(uint8_t index);
void next_profile();
void prev_profile();
void save_profiles(); 
void load_profiles(); 
