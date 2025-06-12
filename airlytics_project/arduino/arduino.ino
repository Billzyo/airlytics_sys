/*This Arduino sketch reads temperature and humidity data from a DHT11 sensor and displays the values on a 16x2 LCD screen.
It also sends the readings via Serial1 to an ESP32 for remote monitoring or logging. 
Additionally, it listens for relay control commands (RELAY_ON or RELAY_OFF) from the ESP32 to toggle a connected relay module.

Hardware Connections
DHT11 Sensor: Data pin on pin 6.
LCD (16x2): Connected via pins 12, 11, 5, 4, 3, 2.
Relay Module: Controlled via pin 7.
Serial1 (ESP32 Communication): Baud rate 115200.*/

#include <LiquidCrystal.h>
#include <DHT.h>

// Initialize the LCD with the pin numbers
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define DHTPIN 6
#define DHTTYPE DHT11
#define RELAY_PIN 7  // Relay control pin

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);   // For debugging via USB
  Serial1.begin(115200);  // For communication with ESP32
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Initialize relay off

  lcd.begin(16, 2);
  lcd.print("Initializing...");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Sensor Error");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
  } else {
    String dataToSend = "Temp:" + String(temperature, 1) + "C Hum:" + String(humidity, 1) + "%";
    Serial1.print(dataToSend + "\n");
    Serial.println("Sent: " + dataToSend);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature, 1);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("H:");
    lcd.print(humidity, 1);
    lcd.print("%");
  }

  // ✅ Listen for relay commands from ESP32 via Serial1
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    command.trim();

    Serial.println("Received command: " + command);

    if (command.equalsIgnoreCase("RELAY_ON")) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Relay turned ON");
    } else if (command.equalsIgnoreCase("RELAY_OFF")) {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay turned OFF");
    }
  }

  delay(2000);
}
