#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// LCD pins <--> Arduino pins
const int RS = 11, EN = 12, D4 = 2, D5 = 3, D6 = 4, D7 = 5;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define LEDPIN 7      // LED pin

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); // set up number of columns and rows
  dht.begin();
  pinMode(LEDPIN, OUTPUT); // Set LED pin as output

  lcd.setCursor(0, 0);
  lcd.print("  CHUNGU MUSAKA ");
  lcd.setCursor(0, 1);
  lcd.print("Temp Monitor!");
  delay(2000);
  lcd.clear();

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  delay(delayMS);

  sensors_event_t tempEvent, humEvent;
  dht.temperature().getEvent(&tempEvent);
  dht.humidity().getEvent(&humEvent);

  lcd.clear();

  lcd.setCursor(0, 0);
  if (isnan(tempEvent.temperature)) {
    lcd.print("Temp Error");
  } else {
    lcd.print("TMP: ");
    lcd.print((int)tempEvent.temperature);
    lcd.print((char)223);
    lcd.print("C");
  }

  lcd.setCursor(0, 1);
  if (isnan(humEvent.relative_humidity)) {
    lcd.print("Humidity Err");
  } else {
    lcd.print("HUM: ");
    lcd.print((int)humEvent.relative_humidity);
    lcd.print("%");
  }

  Serial.print(tempEvent.temperature);
  Serial.print(",");
  Serial.println(humEvent.relative_humidity);
}