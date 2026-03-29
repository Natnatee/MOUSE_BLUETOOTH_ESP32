#include "display_module.h"
#include "profile_manager.h"

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

void display_setting_main(int cursor_idx, bool connected) {
    display.clearDisplay();
    display.setTextSize(1);
    
    // Header (Yellow Zone 0-15px)
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("PROFILE: ---");
    display.setCursor(0, 8);
    display.printf("MODE: %s", connected ? "SETTING ON" : "SETTING OFF");
    display.drawLine(0, 16, 127, 16, SSD1306_WHITE);

    // Body (Blue Zone 17-63px)
    // We can only show 3 items at a time safely
    int max_display_items = 3;
    int total_items = MAX_PROFILES + 1; // 4 items: Add + 3 Profiles
    
    // Calculate display window (pagination)
    int start_idx = 0;
    if (cursor_idx >= max_display_items) {
        start_idx = cursor_idx - max_display_items + 1;
    }
    
    // Safety clamp (in case cursor wrapped from bottom to top)
    if (start_idx < 0) start_idx = 0;
    if (start_idx + max_display_items > total_items) start_idx = total_items - max_display_items;

    // Draw up to max_display_items
    for (int i = 0; i < max_display_items; i++) {
        int item_idx = start_idx + i;
        if (item_idx >= total_items) break;
        
        // Start Y at 18, each takes 15px
        int y_pos = 18 + (i * 15);
        
        if (item_idx == cursor_idx) {
            // Highlighted: solid white box, black text
            display.fillRect(0, y_pos, 128, 13, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            // Normal: white outline box, white text
            display.drawRect(0, y_pos, 128, 13, SSD1306_WHITE);
            display.setTextColor(SSD1306_WHITE);
        }
        
        // Print text inside box, slightly indented
        display.setCursor(4, y_pos + 3);
        
        if (item_idx == 0) {
            display.print("[+] ADD NEW PROFILE");
        } else {
            // Index 1,2,3 mapped to profile 0,1,2
            ProfileData* p = get_profile(item_idx - 1);
            if (p != nullptr) {
                display.print(p->name);
            }
        }
    }
    
    display.display();
}

void display_setting_profile(const char* profile_name, int cursor_idx, bool connected) {
    display.clearDisplay();
    display.setTextSize(1);
    
    // Header (Yellow Zone 0-15px)
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.printf("PROFILE: %s", profile_name);
    display.setCursor(0, 8);
    display.printf("MODE: %s", connected ? "SETTING ON" : "SETTING OFF");
    display.drawLine(0, 16, 127, 16, SSD1306_WHITE);

    // Body (Blue Zone 17-63px)
    const char* menu_items[] = {"MOUSE", "KEYBOARD", "DELETE"};
    int max_items = 3;
    
    // Draw 3 boxes
    for (int i = 0; i < max_items; i++) {
        // Start Y at 18, each takes 15px
        int y_pos = 18 + (i * 15);
        
        if (i == cursor_idx) {
            // Highlighted: solid white box, black text
            display.fillRect(0, y_pos, 128, 13, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            // Normal: white outline box, white text
            display.drawRect(0, y_pos, 128, 13, SSD1306_WHITE);
            display.setTextColor(SSD1306_WHITE);
        }
        
        // Print text inside box, slightly indented
        display.setCursor(4, y_pos + 3);
        display.print("> ");
        display.print(menu_items[i]);
    }
    
    display.display();
}

void display_clear() {
    display.clearDisplay();
    display.display();
}
