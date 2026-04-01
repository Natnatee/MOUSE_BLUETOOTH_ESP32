#include "profile_manager.h"

ProfileData profiles[MAX_PROFILES];
uint8_t current_profile_idx = 0;
uint8_t active_profiles_count = 0;

void profile_manager_init() {
    active_profiles_count = 3;
    
    strncpy(profiles[0].name, "MPU MOUSE", PROFILE_NAME_LEN);
    profiles[0].mouse_input = InputSource::MPU6050;
    profiles[0].keyboard_input = InputSource::JOYSTICK;
    profiles[0].mpu_sensitivity = 5;

    strncpy(profiles[1].name, "Ball", PROFILE_NAME_LEN);
    profiles[1].mouse_input = InputSource::NONE; // ปิดเมาส์ตามสั่ง
    profiles[1].keyboard_input = InputSource::MPU6050; // ยังใช้ MPU คุม A/D/W
    profiles[1].mpu_sensitivity = 3;

    strncpy(profiles[2].name, "JOY MOUSE", PROFILE_NAME_LEN);
    profiles[2].mouse_input = InputSource::JOYSTICK; // ใช้ Joy แทนเมาส์
    profiles[2].keyboard_input = InputSource::NONE;
    profiles[2].mpu_sensitivity = 1;
}

uint8_t get_active_profiles_count() {
    return active_profiles_count;
}

bool add_new_profile(const char* name) {
    if (active_profiles_count >= MAX_PROFILES) return false;
    
    int idx = active_profiles_count;
    strncpy(profiles[idx].name, name, PROFILE_NAME_LEN);
    profiles[idx].name[PROFILE_NAME_LEN - 1] = '\0';
    
    profiles[idx].mouse_input = InputSource::NONE;
    profiles[idx].keyboard_input = InputSource::NONE;
    profiles[idx].mpu_sensitivity = 5;
    
    active_profiles_count++;
    return true;
}

void delete_profile(uint8_t index) {
    if (index >= active_profiles_count || active_profiles_count <= 1) return; // Prevent deleting the last profile
    
    for (int i = index; i < active_profiles_count - 1; i++) {
        profiles[i] = profiles[i+1];
    }
    
    active_profiles_count--;
    
    if (current_profile_idx >= active_profiles_count) {
        current_profile_idx = active_profiles_count - 1;
    }
}

ProfileData* get_current_profile() {
    if (active_profiles_count == 0) return nullptr;
    return &profiles[current_profile_idx];
}

ProfileData* get_profile(uint8_t index) {
    if (index >= active_profiles_count) return nullptr;
    return &profiles[index];
}

void next_profile() {
    if (active_profiles_count == 0) return;
    current_profile_idx = (current_profile_idx + 1) % active_profiles_count;
}

void prev_profile() {
    if (active_profiles_count == 0) return;
    if (current_profile_idx == 0) {
        current_profile_idx = active_profiles_count - 1;
    } else {
        current_profile_idx--;
    }
}

void save_profiles() {}
void load_profiles() {}
