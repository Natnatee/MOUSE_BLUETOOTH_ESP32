#include "profile_manager.h"

// Default Mock Profiles
ProfileData profiles[MAX_PROFILES] = {
    {
        "PROFILE 01",
        InputSource::MPU6050,      // Mouse controlled by MPU
        InputSource::JOYSTICK,     // Keyboard (Arrows) controlled by Joystick
        5                          // Sensitivity
    },
    {
        "PROFILE 02",
        InputSource::JOYSTICK,     // Mouse controlled by Joystick
        InputSource::BUTTON_SET_2, // Keyboard controlled by Buttons
        3
    },
    {
        "PROFILE 03",
        InputSource::NONE,
        InputSource::NONE,
        1
    }
};

uint8_t current_profile_idx = 0;

void profile_manager_init() {
    // Eventually load from Preferences here
    // load_profiles();
}

ProfileData* get_current_profile() {
    return &profiles[current_profile_idx];
}

void next_profile() {
    current_profile_idx = (current_profile_idx + 1) % MAX_PROFILES;
}

void prev_profile() {
    if (current_profile_idx == 0) {
        current_profile_idx = MAX_PROFILES - 1;
    } else {
        current_profile_idx--;
    }
}

void save_profiles() {
    // TODO: Implement Preferences save
}

void load_profiles() {
    // TODO: Implement Preferences load
}
