# Airlytics
#ESP32 & PHP/MySQL temperature-humidity monitor

ESP32-based temperature & humidity monitor  
• ESP32 sketch sends DHT11 readings to PHP/MySQL  
• Arduino sketch displays on 16×2 LCD  
• PHP scripts store & retrieve data from MySQL  
• Realtime dashboard with Chart.js

## Folder structure
- `esp32/esp32.ino`  
- `arduino/arduino.ino`  
- `php-server/`  
  - `submit.php`, `get_data.php`, `get-latest.php`, `realtime-dashboard.php`  
- `db_setup.sql`

## Getting started
1. Run `db_setup.sql` in MySQL/XAMPP  
2. Place `php-server/` in XAMPP’s `htdocs/`  
3. Upload Arduino & ESP32 sketches  
4. Visit `http://<your-ip>/php-server/realtime-dashboard.php`

## License
MIT
