#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

void display_init();
void display_play_mode(const char* profile_name, bool connected, int mouse_x, int mouse_y, char key_pressed, int volume_pct, int force_raw);
void display_setting_mode(const char* setting_name, int volume_pct, bool connected); // For later
void display_clear();
