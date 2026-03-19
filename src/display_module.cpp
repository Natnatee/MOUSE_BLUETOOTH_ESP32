#include "display_module.h"

// ใช้พาสโปรเซสเซอร์เพื่อเช็คแอดเดรส
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void display_init() {
    // กำหนดขา I2C หากแตกต่างจากบอร์ดมาตรฐาน
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // ตั้งค่า OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // วนลูปถ้าต่อไม่ได้
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("ESP32 Mouse Ready!");
    display.display();
}

void display_update(const char* mode, int x, int y, bool connected) {
    display.clearDisplay();
    
    // ส่วนหัว (สีเหลือง - แถว 0-15)
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.printf("MODE: %s", mode);
    
    // ขยับมาบรรทัดที่ 8 เพื่อให้อยู่ในเขตสีเหลือง (พิกเซลที่ 8-15)
    display.setCursor(0, 8);
    display.printf("BT: %s", connected ? "CONNECTED" : "DISCONNECT");
    
    // เส้นแบ่ง (พิกเซลที่ 16 พอดี)
    display.drawLine(0, 16, 127, 16, SSD1306_WHITE);

    // ส่วนล่าง (สีฟ้า - แถว 16-63)
    display.setTextSize(2);
    display.setCursor(0, 22);
    display.printf("X: %4d", x);
    display.setCursor(0, 44);
    display.printf("Y: %4d", y);
    
    display.display();
}

void display_clear() {
    display.clearDisplay();
    display.display();
}
