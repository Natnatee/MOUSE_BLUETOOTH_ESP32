#include "hw_inputs.h"
#include "config.h"

void hw_inputs_init() {
    // Assuming buttons are wired to GND and use internal Pull-UP resistors
    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
    pinMode(BTN_OK_PIN, INPUT_PULLUP);
    pinMode(BTN_BACK_PIN, INPUT_PULLUP);
    
    pinMode(BTN_CONF_1_PIN, INPUT_PULLUP);
    pinMode(BTN_CONF_2_PIN, INPUT_PULLUP);
    pinMode(BTN_CONF_3_PIN, INPUT_PULLUP);
    pinMode(BTN_CONF_4_PIN, INPUT_PULLUP);
}

ButtonState hw_read_buttons() {
    ButtonState bs;
    // Active LOW check
    bs.up = (digitalRead(BTN_UP_PIN) == LOW);
    bs.down = (digitalRead(BTN_DOWN_PIN) == LOW);
    bs.ok = (digitalRead(BTN_OK_PIN) == LOW);
    bs.back = (digitalRead(BTN_BACK_PIN) == LOW);
    
    bs.conf1 = (digitalRead(BTN_CONF_1_PIN) == LOW);
    bs.conf2 = (digitalRead(BTN_CONF_2_PIN) == LOW);
    bs.conf3 = (digitalRead(BTN_CONF_3_PIN) == LOW);
    bs.conf4 = (digitalRead(BTN_CONF_4_PIN) == LOW);
    return bs;
}

int hw_read_pot() {
    // ESP32 default 12-bit ADC (0 - 4095)
    return analogRead(POT_PIN);
}
