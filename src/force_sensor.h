#pragma once
#include <Arduino.h>
#include "config.h"

// --- Force Sensor (FSR402) ---
// FSR_ANALOG_PIN (GPIO 36/VP) : อ่านค่าแรงกด (Analog 0-4095)
// FSR_DIGITAL_PIN (GPIO 39/VN) : อ่านค่าดิจิทัล (กด/ไม่กด)

void force_sensor_init();
int force_read_analog();       // ค่า ADC ดิบ 0-4095
bool force_read_digital();     // true = กด, false = ไม่กด
bool force_is_pressed();       // เช็คว่าค่า analog เกิน threshold หรือไม่
