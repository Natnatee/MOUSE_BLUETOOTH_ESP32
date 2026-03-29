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

void display_play_mode(const char* profile_name, bool connected, int mouse_x, int mouse_y, char key_pressed, int volume_pct, int force_raw) {
    display.clearDisplay();
    
    // Header (Yellow 0-15px)
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.printf("PROFILE: %s", profile_name);
    
    display.setCursor(0, 8);
    display.printf("MODE: %s", connected ? "PLAY ON" : "PLAY OFF");
    
    display.drawLine(0, 16, 127, 16, SSD1306_WHITE);

    // Body (Blue 17-63px)
    display.setTextSize(1);
    
    // 1. Mouse Tracking
    display.setCursor(0, 20);
    display.printf("MOUSE : X:%d Y:%d", mouse_x, mouse_y);
    
    // 2. Keyboard Tracking
    display.setCursor(0, 30);
    if (key_pressed != 0) {
        display.printf("KEY   : '%c'", key_pressed);
    } else {
        display.printf("KEY   : NONE");
    }
    
    // 3. Volume (แสดง % พร้อมแถบ bar)
    display.setCursor(0, 40);
    int bar_width = map(volume_pct, 0, 100, 0, 56);
    display.printf("VOL:%3d%%[", volume_pct);
    display.setCursor(114, 40);
    display.print("]");
    // Draw solid bar for volume
    display.fillRect(56, 40, bar_width, 7, SSD1306_WHITE);
    
    // 4. Force Sensor (แสดงค่า analog ดิบ 0-4095)
    display.setCursor(0, 52);
    display.printf("FORCE : %d", force_raw);

    display.display();
}

void display_setting_mode(const char* menu_name, int volume_pct, bool connected) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.print("PROFILE: ---");
    display.setCursor(0, 8);
    display.printf("MODE: %s", connected ? "SETTING ON" : "SETTING OFF");
    display.drawLine(0, 16, 127, 16, SSD1306_WHITE);
    
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.printf("> %s", menu_name);
    
    display.display();
}

void display_clear() {
    display.clearDisplay();
    display.display();
}
