---
description: Build, upload and open serial monitor
---
⚠️ **คำเตือน**: ต้องมีการใส่ `delay(6000)` ใน `setup()` เพื่อให้ Serial Monitor พร้อมดักจับข้อมูลทันทีหลังอัปโหลดเสร็จ

รันคำสั่งต่อไปนี้:

```bash
pio run -t upload -t monitor
```
