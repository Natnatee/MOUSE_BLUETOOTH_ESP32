#include "mouse_bt.h"
#include <BleCombo.h>

BleComboKeyboard Keyboard("ESP32 BT Mouse", "Espressif", 100);
BleComboMouse Mouse(&Keyboard);

void mouse_bt_init() {
    Keyboard.begin();
    Mouse.begin();
}

bool mouse_bt_is_connected() {
    return Keyboard.isConnected();
}

void mouse_move(int8_t x, int8_t y, int8_t wheel) {
    if (mouse_bt_is_connected()) {
        Mouse.move(x, y, wheel);
    } else {
        Serial.println("Warning: HID command ignored (NOT CONNECTED)");
    }
}

void mouse_click(uint8_t button) {
    if (mouse_bt_is_connected()) {
        Mouse.click(button);
    }
}

void mouse_press(uint8_t button) {
    if (mouse_bt_is_connected()) {
        Mouse.press(button);
    }
}

void mouse_release(uint8_t button) {
    if (mouse_bt_is_connected()) {
        Mouse.release(button);
    }
}

void keyboard_press(uint8_t key) {
    if (mouse_bt_is_connected()) {
        Keyboard.press(key);
    }
}

void keyboard_release(uint8_t key) {
    if (mouse_bt_is_connected()) {
        Keyboard.release(key);
    }
}

void keyboard_release_all() {
    if (mouse_bt_is_connected()) {
        Keyboard.releaseAll();
    }
}

void keyboard_volume_up() {
    if (mouse_bt_is_connected()) {
        Keyboard.write(KEY_MEDIA_VOLUME_UP);
    }
}

void keyboard_volume_down() {
    if (mouse_bt_is_connected()) {
        Keyboard.write(KEY_MEDIA_VOLUME_DOWN);
    }
}
