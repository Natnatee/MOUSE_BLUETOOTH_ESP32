#pragma once
#include <Arduino.h>
#include <BleCombo.h>

// Handle cases where constants might be missing in some combo forks
#ifndef MOUSE_LEFT
#define MOUSE_LEFT 1
#endif
#ifndef MOUSE_RIGHT
#define MOUSE_RIGHT 2
#endif
#ifndef MOUSE_MIDDLE
#define MOUSE_MIDDLE 4
#endif

void mouse_bt_init();
bool mouse_bt_is_connected();
void mouse_move(int8_t x, int8_t y, int8_t wheel = 0);
void mouse_click(uint8_t button);
void mouse_press(uint8_t button);
void mouse_release(uint8_t button);

// New Keyboard Functions
void keyboard_press(uint8_t key);
void keyboard_release(uint8_t key);
void keyboard_release_all();

// Media controls
void keyboard_volume_up();
void keyboard_volume_down();
