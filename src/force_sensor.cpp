#include "force_sensor.h"

void force_sensor_init() {
    // GPIO 36 (VP) และ 39 (VN) เป็น input-only, ไม่ต้อง pinMode
    // แต่ตั้งไว้เพื่อความชัดเจน
    pinMode(FSR_ANALOG_PIN, INPUT);
    pinMode(FSR_DIGITAL_PIN, INPUT);
    
    Serial.println("Force Sensor: Initialized (FSR402)");
    Serial.printf("  Analog Pin: GPIO %d | Digital Pin: GPIO %d\n", FSR_ANALOG_PIN, FSR_DIGITAL_PIN);
    Serial.printf("  Threshold: %d | Max: %d grams\n", FSR_THRESHOLD, FSR_MAX_GRAMS);
}

static float smoothed_force = 0;
static int baseline_offset = 0;
static bool needs_calibration = true;

int force_read_analog() {
    int raw = analogRead(FSR_ANALOG_PIN);
    
    // Auto-calibration: จับค่าครั้งแรกตอนเปิดเครื่อง (ถ้านิ่ง)
    if (needs_calibration) {
        baseline_offset = raw;
        smoothed_force = 0;
        needs_calibration = false;
        Serial.printf("Force Sensor: Calibrated baseline at %d\n", baseline_offset);
    }

    // คำนวณค่าจริง:
    // กรณีขาลอย/Pull-up: baseline จะสูง (เช่น 4095) -> raw - 4095 -> abs() -> เริ่มที่ 0
    // กรณี Pull-down: baseline จะต่ำ (เช่น 0) -> raw - 0 -> abs() -> เริ่มที่ 0
    int processed = abs(raw - baseline_offset);
    
    // ป้องกันค่าเกินขอบเขต
    if (processed > 4095) processed = 4095;
    if (processed < FSR_THRESHOLD) processed = 0;

    // Exponential Smoothing: NewValue = (Alpha * Current) + ((1 - Alpha) * Previous)
    // Alpha 0.2-0.3 ช่วยให้ค่านิ่งขึ้น ลดอาการ "ตัวเลขเปลี่ยนง่ายมาก"
    smoothed_force = (0.25f * processed) + (0.75f * smoothed_force);
    
    return (int)smoothed_force;
}

bool force_read_digital() {
    // Digital pin: HIGH = กด, LOW = ไม่กด (ขึ้นกับวงจร)
    return (digitalRead(FSR_DIGITAL_PIN) == HIGH);
}

bool force_is_pressed() {
    // ใช้ค่าที่ผ่านการกรองแล้วเทียบ threshold
    return (force_read_analog() > FSR_THRESHOLD);
}
