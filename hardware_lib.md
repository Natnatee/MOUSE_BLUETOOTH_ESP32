# 📟 MCU Specification: ESP32 (Classic)

### 📋 General Specs
- **Core:** Xtensa® Dual-Core 32-bit LX6
- **Max Clock:** 240 MHz
- **Flash:** 4 MB (Used with `huge_app.csv` partition)
- **SRAM:** 520 KB
- **Voltage:** 3.0V - 3.6V (Standard 3.3V)
- **Built-in:** Bluetooth 4.2 (Legacy & BLE), Wi-Fi

### 🔌 Detailed Pinout & Peripheral Reference (For this Project)
| Peripheral | Pins | Function | Description |
|---|---|---|---|
| **I2C (Wire)** | GPIO 21 (SDA), GPIO 22 (SCL) | Bus | MPU6050 & OLED 0.96" |
| **ADC1** | GPIO 34 (VRX), 35 (VRY) | Analog In | Joystick Axis |
| **ADC1** | GPIO 33 | Analog In | Potentiometer (Volume Bar) |
| **ADC1** | GPIO 36 (VP) | Analog In | FSR402 Force Sensor Data |
| **Digital In** | GPIO 13, 12, 14, 27 | UI Setup | Navigation Buttons (Up, Down, OK, Back) |
| **Digital In** | GPIO 17, 5, 18, 19 | Config | Custom Profile Buttons (1-4) |
| **Digital In** | GPIO 32 | Switch | Joystick Click |
| **Digital In** | GPIO 39 (VN) | Switch | Force Sensor Digital Signal |

### 🛠️ External Hardware Components
1. **OLED 0.96" (SSD1306)**: 128x64 pixels for UI Display.
2. **MPU6050**: 6-Axis motion sensor for mouse movement.
3. **FSR402**: Force Sensitive Resistor for pressure detection.
4. **Joystick**: Standard 2-axis for navigation/mouse control.
5. **Potentiometer**: 10k Ohm for volume control.

### 📚 Libraries Used (ESP32 Project)
| Library Name | Purpose |
|---|---|
| **NimBLE-Arduino** | Efficient BLE stack for ESP32 |
| **ESP32-NimBLE-Combo** | Bluetooth HID Keyboard + Mouse combo |
| **MPU6050** | Motion sensor reading |
| **Adafruit SSD1306** | OLED display driver |
| **Adafruit GFX** | Graphics core for UI |

### ⚙️ Recommended PlatformIO Config
```ini
platform = espressif32
board = esp32dev
framework = arduino
board_build.partitions = huge_app.csv
monitor_speed = 115200
lib_deps =
    h2zero/NimBLE-Arduino @ ^1.4.1
    https://github.com/TheNitek/ESP32-NimBLE-Combo.git
    electroniccats/MPU6050 @ ^1.3.1
    adafruit/Adafruit SSD1306 @ ^2.5.7
```

### ⚠️ Common Hardware Issues / Tips
- **Partition Table**: ESP32 BLE/NimBLE libraries are large. Use `huge_app.csv` to ensure enough space for code.
- **Power Consumption**: Bluetooth + OLED can draw significant current. Use a stable 5V source (via Vin) or high-quality Battery.
- **Boot Mode**: Hold **BOOT** button while uploading if the auto-reset circuit is not working.
- **Bluetooth Remove**: if connection fails after flashing, **Unpair/Remove** the device from Windows Bluetooth settings and re-pair.
- **ADC Noise**: Use averaging or filtering for Joystick and FSR readings for smoother UI display.
