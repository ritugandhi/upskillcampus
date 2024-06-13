#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 2
#define DHTTYPE DHT22  // Change to DHT22 if using DHT22 sensor
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "PS";
const char* password = "PPPPP";
const char* server = "api.thingspeak.com";
String apiKey = "API-KEY"; // Replace with your ThingSpeak Write API Key

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");
  display.setCursor(0, 10);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");
  display.display();

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    if (client.connect(server, 80)) {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(t);
      postStr += "&field2=";
      postStr += String(h);
      postStr += "\r\n\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);

      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" C, Humidity: ");
      Serial.print(h);
      Serial.println("% sent to ThingSpeak");
    }
    client.stop();
  }
  delay(20000); // Update ThingSpeak every 20 seconds
}
