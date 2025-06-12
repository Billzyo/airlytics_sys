/*
This ESP32 sketch connects multiple systems—RFID authentication, 
Wi-Fi-enabled sensor monitoring, WebSocket communication, 
and Blynk IoT platform integration—to create a versatile IoT control system.

Features
RFID Access Control: Reads UID using an MFRC522 RFID module and triggers LEDs and buzzer based on card authentication.
Sensor Data Handling: Receives temperature and humidity data from Arduino via Serial2 and updates the values.
Web Dashboard: Displays live temperature and humidity. Charts historical data using Chart.js.
Includes relay control buttons.*/


#define BLYNK_TEMPLATE_ID "TMPL2lCwVo2Hv"
#define BLYNK_TEMPLATE_NAME "iot"
#define BLYNK_AUTH_TOKEN "P3XdHD4rrQs3hKtUJSTpiRthT-LIRW1G"


#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 22
#define RED_LED 14
#define GREEN_LED 12
#define BUZZER 27

MFRC522 rfid(SS_PIN, RST_PIN);

// Example allowed UID (4 bytes)
byte allowedUID[4] = { 0xE3, 0x36, 0x74, 0x31 };  // ✅ Card 2 UID



WebSocketsServer webSocket = WebSocketsServer(81);
WebServer server(80);

String temperature = "--";
String humidity = "--";

const char* ssid = ".0";
const char* password = "123456789cde";

// Web relay states
bool relayState = false;

void setup() {
  SPI.begin();
  rfid.PCD_Init();
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(115200);                       // Debugging
  Serial2.begin(115200, SERIAL_8N1, 32, 25);  // RX=32, TX=25

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  while (!Blynk.connected()) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Blynk");

  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/data", handleJson);
  server.on("/relay_on", handleRelayOn);
  server.on("/relay_off", handleRelayOff);
  server.begin();

  // Setup WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  Blynk.run();

  // Handle WebSocket clients
  webSocket.loop();

  // RFID tag detection
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("Card UID:");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    if (isAllowed(rfid.uid.uidByte)) {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      tone(BUZZER, 1000, 200);  // short beep
      delay(500);
      digitalWrite(GREEN_LED, LOW);
    } else {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      tone(BUZZER, 1000, 1000);  // long beep
      delay(1000);
      digitalWrite(RED_LED, LOW);
    }
    rfid.PICC_HaltA();  // Stop reading
  }


  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    Serial.println("Received: " + data);
    parseSensorData(data);
    // After updating sensor data, send to WebSocket clients
    sendSensorDataWebSocket();
  }
}

// Function to send sensor data via WebSocket
// Function to send sensor data via WebSocket
void sendSensorDataWebSocket() {
  String jsonData = "{";
  jsonData += "\"temperature\":\"" + temperature + "\",";
  jsonData += "\"humidity\":\"" + humidity + "\"";
  jsonData += "}";
  webSocket.broadcastTXT(jsonData);  // Send to all connected clients
}


// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("WebSocket client %u connected\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client %u disconnected\n", num);
      break;
    case WStype_TEXT:
      {
        String msg = String((char*)payload).substring(0, length);
        Serial.printf("WebSocket message from %u: %s\n", num, msg.c_str());

        if (msg == "lights_on" || msg == "turn_on_lights") {
          Serial2.println("RELAY_ON");
          relayState = true;
          Serial.println("Relay ON command sent (WebSocket)");
        } else if (msg == "lights_out" || msg == "turn_off_lights") {
          Serial2.println("RELAY_OFF");
          relayState = false;
          Serial.println("Relay OFF command sent (WebSocket)");
        }
      }
      break;
    default:
      break;
  }
}



void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html><html><head><title>ESP32 Sensor Data & Relay Control</title><script src="https://cdn.jsdelivr.net/npm/chart.js"></script><meta http-equiv="refresh" content="2"><style>body{font-family:Arial,sans-serif;text-align:center;padding:20px;}canvas{max-width:400px;margin:20px auto;}</style></head><body><h2>Sensor Data</h2><p>Temperature: )rawliteral"
                + temperature + R"rawliteral(&deg;C</p><p>Humidity: )rawliteral" + humidity + R"rawliteral( %</p>
<canvas id="tempChart"></canvas><canvas id="humChart"></canvas>
<h2>Relay Control</h2>
<form action='/relay_on' method='get' style='display:inline;'><button type='submit'>Turn ON</button></form>
<form action='/relay_off' method='get' style='display:inline; margin-left:10px;'><button type='submit'>Turn OFF</button></form>
<script>
const tempData = JSON.parse(localStorage.getItem('tempData') || '[]');
const humData = JSON.parse(localStorage.getItem('humData') || '[]');
const labels = JSON.parse(localStorage.getItem('labels') || '[]');
const now = new Date().toLocaleTimeString();
const temp = parseFloat(")rawliteral"
                + temperature + R"rawliteral(");
const hum = parseFloat(")rawliteral"
                + humidity + R"rawliteral(");
if (!isNaN(temp) && !isNaN(hum)) {
  labels.push(now); tempData.push(temp); humData.push(hum);
  if (labels.length > 10) { labels.shift(); tempData.shift(); humData.shift(); }
  localStorage.setItem('labels', JSON.stringify(labels));
  localStorage.setItem('tempData', JSON.stringify(tempData));
  localStorage.setItem('humData', JSON.stringify(humData));
}
new Chart(document.getElementById("tempChart"), {
  type: 'line',
  data: {
    labels: labels,
    datasets: [{ label: 'Temperature (°C)', data: tempData, borderColor: 'red', fill: false }]
  }
});
new Chart(document.getElementById("humChart"), {
  type: 'line',
  data: {
    labels: labels,
    datasets: [{ label: 'Humidity (%)', data: humData, borderColor: 'blue', fill: false }]
  }
});
</script></body></html>
)rawliteral";
  server.send(200, "text/html", html);
}


void handleJson() {
  String json = "{";
  json += "\"temperature\":\"" + temperature + "\",";
  json += "\"humidity\":\"" + humidity + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleRelayOn() {
  Serial2.println("RELAY_ON");
  Serial.println("Relay ON command sent (Web)");
  relayState = true;
  server.sendHeader("Location", "/");
  server.send(302);
}

void handleRelayOff() {
  Serial2.println("RELAY_OFF");
  Serial.println("Relay OFF command sent (Web)");
  relayState = false;
  server.sendHeader("Location", "/");
  server.send(302);
}

void parseSensorData(String data) {
  int tStart = data.indexOf("Temp:");
  int tEnd = data.indexOf("C");
  int hStart = data.indexOf("Hum:");
  int hEnd = data.indexOf("%");

  if (tStart != -1 && tEnd != -1 && hStart != -1 && hEnd != -1) {
    temperature = data.substring(tStart + 5, tEnd);
    humidity = data.substring(hStart + 4, hEnd);
    temperature.trim();
    humidity.trim();

    // Send to Blynk
    Blynk.virtualWrite(V1, temperature);  // V0 = temperature
    Blynk.virtualWrite(V2, humidity);     // V1 = humidity

    // Broadcast new data over WebSocket
    sendSensorDataWebSocket();
  }


}
bool isAllowed(byte * uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != allowedUID[i]) return false;
  }
  return true;
}


// Blynk virtual pin to control relay
BLYNK_WRITE(V0) {
  int state = param.asInt();
  if (state == 1) {
    Serial2.println("RELAY_ON");
    Serial.println("Relay ON command sent (Blynk)");
    relayState = true;
  } else {
    Serial2.println("RELAY_OFF");
    Serial.println("Relay OFF command sent (Blynk)");
    relayState = false;
  }
}
