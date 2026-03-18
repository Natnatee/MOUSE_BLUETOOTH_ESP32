# 📟 MCU Specification: STM32F103C6 (Bluepill)

### 📋 General Specs
- **Core:** ARM Cortex-M3
- **Max Clock:** 72 MHz
- **Default Flash:** 32 KB (Verified)
- **SRAM:** 10 KB
- **Voltage:** 2.0V - 3.6V (Standard 3.3V)

### 🔌 Detailed Pinout & Peripheral Reference
| Peripheral | Pins | 5V Tolerant? | Description |
|---|---|---|---|
| **UART1** | PA9 (TX), PA10 (RX) | **Yes** | Main Serial (Used for Upload/Log) |
| **UART2** | PA2 (TX), PA3 (RX) | **Yes** | Secondary Serial |
| **I2C1** | PB6 (SCL), PB7 (SDA) | **Yes** | Standard I2C Port |
| **SPI1** | PA5 (SCK), PA6 (MISO), PA7 (MOSI) | No | High Speed Serial Peripheral |
| **ADC1/2** | PA0 - PA7, PB0, PB1 | No | 12-bit Analog Input (Max 3.3V) |
| **USB** | PA11 (D-), PA12 (D+) | **Yes** | USB Full Speed |
| **LED** | PC13 | No | Built-in LED (Active LOW) |

### 🛠️ External Hardware Components
1. **Button Matrix 4x4**: 16 tactile buttons for control and data mapping.
2. **LEDs (5 Colors)**:
   - Red (🔴), Yellow (🟡), Green (🟢), Blue (🔵), White (⚪)
   - Used for Profile binary display and Status signals.
3. **IR Send**: High-power IR LED (typically driven via transistor).
4. **IR Receive**: IR Receiver module (e.g., TSOP series) for learning signals.
5. **Power Module**: 
   - Battery Holder (1.5V)
   - Boost Converter (Step-up to 5V) for stable operation.

### 📚 Libraries Used
| Library Name | Version | Purpose |
|---|---|---|
| **IRremote** | ^4.3.1 | Core IR send/receive logic |
| **Keypad** | (TBD) | Handling 4x4 Matrix scanning |
| **EEPROM (Emulated)** | Built-in | Saving IR profiles to Flash |

### ⚙️ Recommended PlatformIO Config
```ini
platform = ststm32
board = bluepill_f103c6
framework = arduino
upload_protocol = stlink
monitor_speed = 115200
lib_deps =
    z3t0/IRremote@^4.3.1
```

### ⚠️ Common Hardware Issues / Tips
- **Bootloader Mode (Mandatory for UART):** 
    1. ปรับ Jumper: **Boot0 = 1, Boot1 = 0**
    2. **กดปุ่ม Reset** บนบอร์ด (เพื่อเข้าสู่ Bootloader Mode)
    3. จึงจะเริ่มสั่ง Flash โค้ดได้
- **Flash Protection:** ชิป C6 มี Flash น้อย (32KB) ควรระวังขนาดของ IR buffer หากใช้โหมด RAW
- **Wait for Serial:** เมื่อใช้ `pio run -t upload -t monitor` ควรใส่ `delay(6000)` ใน `setup()` เสมอ
- **ADC Warning:** ห้ามจ่ายไฟเกิน 3.3V เข้าขา ADC เด็ดขาด (ขาเหล่านี้ไม่ 5V-Tolerant)
