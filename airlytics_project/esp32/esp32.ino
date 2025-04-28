#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "BillZ 3.0";
const char* password = "123456789";
const char* serverURL = "http://192.168.160.212/php-server/submit.php";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  float temp = 26.4;    // Replace with actual sensor values
  float hum = 60.1;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "temperature=" + String(temp) + "&humidity=" + String(hum);
    Serial.println("Sending: " + postData);

    int httpResponseCode = http.POST(postData);
    String payload = http.getString();

    Serial.println("Response code: " + String(httpResponseCode));
    Serial.println("Server reply: " + payload);

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  delay(5000);
}