## 🎯 1. Project Overview
สร้างอุปกรณ์ควบคุมอเนกประสงค์สำหรับ Smart TV/PC โดยทำงานได้ 2 สถานะหลัก (State Machine):
1. **PLAY Mode**: อ่านค่าแอนะล็อก/ดิจิทัล นำไปสั่งเมาส์และคีย์บอร์ดแบบ Composite 
2. **SETTING Mode**: หน้าต่างปรับแต่งสำหรับเปลี่ยน Profile และจับคู่ Input เข้าทิศทางเมาส์/ปุ่มกดคีย์บอร์ดแบบ Dynamic

## 🏗️ 1.5 Architecture (Modular Design)
- **Data Providers**: `mpu_sensor`, `joystick_input`, `hw_inputs` (ปุ่ม x8, Potentiometer x1), `force_sensor` (FSR402)
- **Logic Controllers**: 
   - `input_controller` (ตัวตัดสินใจหลักและคุม UI Refresh Rate 10FPS)
   - `profile_manager` (จัดการข้อมูล Profile การผูกปุ่ม)
   - `state_manager` (คุม FSM สลับโหมด)
- **Action Providers**: `mouse_bt` (อัปเกรดเป็น `BleCombo` ส่งค่าทั้งเมาส์และคีย์บอร์ดสื่อมีเดีย)
- **Display Provider**: `display_module` (OLED 3 โซน แสดง Profile, เมาส์, คีย์, แถบเสียง% และ Force)

## 🔌 2. Hardware Specs
- **MCU**: ESP32 Classic (Original Chip)
- **Sensor**: MPU6050 (GY-521) 3-Axis Gyro & Accelerometer
- **Force Sensor**: FSR402 (0-5kg)
- **Connection**: Bluetooth Composite HID (Keyboard + Mouse)
- **Display**: OLED 0.96" I2C

## 📍 3. Pin Mapping Table
| Device | Pin (ESP32) | Function | Type |
|---|---|---|---|
| **MPU6050 / OLED** | GPIO 21 | SDA | I2C |
| **MPU6050 / OLED** | GPIO 22 | SCL | I2C |
| **Joystick** | GPIO 34 | VRX | ADC1 |
| **Joystick** | GPIO 35 | VRY | ADC1 |
| **Joystick** | GPIO 32 | SW / Click | Digital In |
| **Potentiometer** | GPIO 33 | Volume | ADC1 |
| **Global Btn (Up)** | GPIO 13 | UI Navigation | Digital In (Pull-up) |
| **Global Btn (Down)** | GPIO 12 | UI Navigation | Digital In (Pull-up) |
| **Global Btn (OK)** | GPIO 14 | UI Select | Digital In (Pull-up) |
| **Global Btn (Back)** | GPIO 27 | UI Switch State | Digital In (Pull-up) |
| **Config Btn 1** | GPIO 17 | Custom Key | Digital In (Pull-up) |
| **Config Btn 2** | GPIO 5 | Custom Key | Digital In (Pull-up) |
| **Config Btn 3** | GPIO 18 | Custom Key | Digital In (Pull-up) |
| **Config Btn 4** | GPIO 19 | Custom Key | Digital In (Pull-up) |
| **Force Sensor (Analog)** | GPIO 36 (VP) | Force ADC | ADC1 |
| **Force Sensor (Digital)** | GPIO 39 (VN) | Force Digital | Digital In |

## 📚 4. Library Specs
- `blackketter/ESP32-BLE-Combo` (คีย์บอร์ด+เมาส์ในตัวเดียว)
- `electroniccats/MPU6050`
- `Adafruit SSD1306` + `Adafruit GFX Library`

## ⚙️ 5. Config & Settings
- **Partition Scheme**: `huge_app.csv` (ขยาย Flash เป็น 3MB)
- **Baud Rate**: 115200
- **I2C Address**: 0x3C (OLED), 0x68 (MPU6050)

## 📌 6. Current State
- [x] Initializing project structure & GitHub
- [x] Bluetooth HID Combo Working (Mouse + Keyboard Working)
- [x] FSM & Dynamic Profile Manager 
- [x] Display Module with Volume Bar UI (%) + Force Sensor Display and 10 FPS Rate Limiter
- [x] Force Sensor (FSR402) Module Integrated
- [x] HW Inputs (Debounce 250ms & Multiple Buttons/Potentiometer) 
- [x] Connect Potentiometer to actual `KEY_MEDIA_VOLUME` (Synced with Host)
- [x] Implement `SETTING Mode` UI (Main Menu, Profile Sub-menu, Name Edit, Delete)
- [ ] Save/Load Profiles with `Preferences` (Persistent Storage)

## 📜 7. Issues & Solutions Log
- **[Fix] Flash Memory น้อยไป**: เปลี่ยน `board_build.partitions = huge_app.csv` 
- **[Fix] เมาส์ไม่ขยับ/ปุ่มไม่ขึ้นเมื่อเปลี่ยนไลบรารี**: OS จำค่า Device Descriptor เก่า ต้องสั่ง Remove Device ออกาก Bluetooth Windows ก่อนเชื่อมต่อใหม่
- **[Fix] ปุ่มสลับ Profile เบิ้ลข้ามหน้า**: ใส่หน่วงเวลา Debounce 250ms 
- **[Fix] หน้าจอ OLED กระตุก/เมาส์หน่วง**: จับแยก `ui_needs_update` ไว้และครอบด้วย `last_ui_refresh > 100ms` (10 FPS Limit)
- **[Fix] แก้ไขชื่อ Profile**: ปรับปรุงให้ตัวอักษรที่ขึ้นมาใหม่ (ตำแหน่งว่าง) เริ่มต้นที่ 'A' เสมอ เพื่อความสะดวกในการพิมพ์
