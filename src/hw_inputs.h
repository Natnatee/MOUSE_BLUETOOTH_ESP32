#pragma once
#include <Arduino.h>

struct ButtonState {
    bool up;
    bool down;
    bool ok;
    bool back;
    bool conf1;
    bool conf2;
    bool conf3;
    bool conf4;
};

void hw_inputs_init();
ButtonState hw_read_buttons();
int hw_read_pot(); 
