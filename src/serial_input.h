#pragma once
#include <Arduino.h>

void serial_input_init(unsigned long baud = 115200);
bool serial_input_available();
void serial_input_process();
