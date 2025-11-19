#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const char* ssid = "smpqinthara";
const char* password = "100%qinthara";

#define DHTPIN D3
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  lcd.init(); 
  lcd.backlight();

  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  Serial.println(F("DHT22 Unified Sensor Example"));
  sensor_t sensor;

  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  delayMS = sensor.min_delay / 1000;
}

unsigned long lastUpdate = 0;
const unsigned long interval = 3000;

void loop() {
  delay(delayMS);
  sensors_event_t event;

  // Membaca temperature & humidity
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;

  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Gagal baca sensor DHT");
    return;
  }

  Serial.print("Temperatrure: ");
  Serial.print(temperature);
  Serial.println("°C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  if (millis() - lastUpdate >= interval) {
    lastUpdate = millis();

    lcd.clear();

    // ---- Suhu ----
    lcd.setCursor(0, 0);
    if (temperature >= 30.0) {
      lcd.print("Suhu Panas");
    } else if (temperature <= 21.0) {
      lcd.print("Suhu Dingin");
    } else {
      lcd.print("Suhu Normal");
    }

    // ---- Kelembapan ----
    lcd.setCursor(0, 1);
    if (humidity >= 60.0) {
        lcd.print("Terlalu Lembap");
    } else if (humidity <= 40.0) {
        lcd.print("Kurang Lembap");
    } else {
        lcd.print("Normal");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    
    // 172.27.54.11
    String url = "http://192.168.18.232/dhtiot/update-data/"; // URL Server
    url += String(temperature, 1) + "/" + String(humidity, 1); // URL = URL + temperatur + / + humidity 
    // http://172.27.54.11/dhtiot/public/update-data/{temperature}/{humidity}

    Serial.print("Mengirim data ke ");
    Serial.println(url);

    http.begin(client, url); // fungsi untuk memulai koneksi HTTP klien
    int httpCode = http.GET(); // metode HTTP untuk meminta data dari server, yang berfungsi dengan mengambil data tanpa mengubahnya

    if (httpCode > 0) { // Kalau ada httpCode nya
      Serial.printf("HTTP Response Code: %d\n", httpCode); // Print ini + code yang nandain sukses atau nggak (200 = sukses)
      String payload = http.getString(); // Ngambil respons dari httpCode 
      Serial.println("Response: ");
      Serial.println(payload);
    } else {
      Serial.printf("Gagal mengirim data. Error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi tidak terhubung");
    WiFi.reconnect();
  }

  delay(3000);
}
