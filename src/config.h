#pragma once
#include <Arduino.h>

// --- I2C Pins (Common for MPU and OLED) ---
#define SDA_PIN 21
#define SCL_PIN 22

// --- Joystick Pins (HW-504) ---
#define JOY_VRX_PIN 34
#define JOY_VRY_PIN 35
#define JOY_SW_PIN  32

// --- Display Config ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C // ปกติคือ 0x3C หรือ 0x3D

// --- Motion Config ---
const int JOY_DEAD_ZONE = 400; // ค่าระยะตายของจอย
const int MPU_DIVIDER = 512;  // ความไวของ MPU
const int JOY_DIVIDER = 140;  // ความไวของ Joystick
