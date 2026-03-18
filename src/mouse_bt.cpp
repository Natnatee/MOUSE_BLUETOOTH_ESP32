#include "mouse_bt.h"

BleMouse ble_mouse("ESP32_Smart_Mouse", "Espressif", 100);

void mouse_bt_init() {
    ble_mouse.begin();
}

bool mouse_bt_is_connected() {
    return ble_mouse.isConnected();
}

void mouse_move(int8_t x, int8_t y, int8_t wheel) {
    if (mouse_bt_is_connected()) {
        ble_mouse.move(x, y, wheel);
    } else {
        Serial.println("Warning: Mouse command ignored (Bluetooth NOT CONNECTED)");
    }
}

void mouse_click(uint8_t button) {
    if (mouse_bt_is_connected()) {
        ble_mouse.click(button);
    } else {
        Serial.println("Warning: Mouse click ignored (Bluetooth NOT CONNECTED)");
    }
}
