#pragma once
#include <Arduino.h>
#include <BleMouse.h>

extern BleMouse ble_mouse;

void mouse_bt_init();
bool mouse_bt_is_connected();
void mouse_move(int8_t x, int8_t y, int8_t wheel = 0);
void mouse_click(uint8_t button);
void mouse_press(uint8_t button);
void mouse_release(uint8_t button);
