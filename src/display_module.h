#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

void display_init();
void display_play_mode(const char* profile_name, bool connected, int mouse_x, int mouse_y, char key_pressed, int volume_pct, int force_raw);
void display_setting_main(int cursor_idx, bool connected);
void display_setting_profile(const char* profile_name, int cursor_idx, bool connected);
void display_clear();
