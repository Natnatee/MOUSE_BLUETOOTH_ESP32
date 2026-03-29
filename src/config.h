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

// --- Global Buttons (Setup Menu) ---
#define BTN_UP_PIN 13
#define BTN_DOWN_PIN 12
#define BTN_OK_PIN 14
#define BTN_BACK_PIN 27

// --- Configurable Buttons ---
#define BTN_CONF_1_PIN 4
#define BTN_CONF_2_PIN 5
#define BTN_CONF_3_PIN 18
#define BTN_CONF_4_PIN 19

// --- Potentiometer ---
#define POT_PIN 33

// --- Force Sensor (FSR402) ---
#define FSR_ANALOG_PIN 36   // VP 
#define FSR_DIGITAL_PIN 39  // VN
const int FSR_MAX_GRAMS = 5000;  // FSR402 max 5kg = 5000g
const int FSR_THRESHOLD = 50;    // ADC threshold ขั้นต่ำ (กรองสัญญาณรบกวน)
