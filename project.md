## 🎯 1. Project Overview
สร้างอุปกรณ์ควบคุมอเนกประสงค์สำหรับ Smart TV/PC โดยมีระบบ Input 2 โหมดหลัก:
1.  **Serial Mode**: รับคำสั่งตรงจาก PC (Direct Control)
2.  **Hardware Mode**: ใช้ Hardware Sensor (MPU6050) และปุ่มกด (ในอนาคต) มาทำลอจิกควบคุม

## 🏗️ 1.5 Architecture (Modular Design)
- **Data Providers**: `mpu_sensor` (คืนค่าดิบ/กรองแล้ว), `serial_input` (คำสั่งจาก PC)
- **Logic Controller**: `input_manager` (ตัวตัดสินใจว่าค่าจาก Sensor จะเอาไปสั่งเมาส์หรือคีย์บอร์ด)
- **Action Providers**: `mouse_bt` (Bluetooth Mouse), `keyboard_bt` (อนาคต)

## 📌 6. Current State
- [x] Initializing project structure
- [x] Bluetooth Mouse (BLE HID) Working 100% on PC
- [x] Serial Command Parser Working
- [ ] Refactoring `mpu_sensor` to Data Provider Pattern
- [ ] Implementing `input_manager` to bridge sensor and actions

## 🔌 2. Hardware Specs
- **MCU**: ESP32 Classic (Original Chip)
- **Sensor**: MPU6050 (GY-521) 3-Axis Gyro & Accelerometer
- **Connection**: Bluetooth HID (Target: Smart TV Android)
- **Input Channels**: I2C (MPU6050) & UART (Serial Port)

## 📍 3. Pin Mapping Table
| Device | Pin (ESP32) | Function | Type |
|---|---|---|---|
| **MPU6050** | GPIO 21 | SDA | I2C |
| **MPU6050** | GPIO 22 | SCL | I2C |
| **USB** | GPIO 1 (TX0) | TX | UART (Serial) |
| **USB** | GPIO 3 (RX0) | RX | UART (Serial) |

## 📚 4. Library Specs
- `ESP32-BLE-Mouse` (โดย T-vK) - *รอการยืนยันว่าจะใช้ BLE หรือ Classic*
- `MPU6050` (โดย ElectronicCats)
- `I2Cdev` (สำหรับจัดการ Sensor)

## ⚙️ 5. Config & Settings
- **Baud Rate**: 115200
- **Bluetooth Name**: `ESP32_Smart_Mouse`
- **I2C Address**: 0x68 (Default MPU6050)

## 📌 6. Current State
- [x] Initializing project structure
- [x] Creating `.gitignore`
- [ ] Configuring `platformio.ini`
- [ ] Developing Input Isolation Logic (Serial vs Gyro)
- [ ] Implementing BT Mouse HID Logic

## 📜 7. Issues & Solutions Log
- (ยังไม่มีปัญหาบันทึก)
