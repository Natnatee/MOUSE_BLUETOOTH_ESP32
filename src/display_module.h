#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

void display_init();
void display_update(const char* mode, int x, int y, bool connected);
void display_clear();
