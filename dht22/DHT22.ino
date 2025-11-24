#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* ssid = "SIB BLOK E3 NO 14";
const char* password = "3783140504Okay";

#define DHTPIN D3
#define DHTTYPE DHT22
#define LED_PIN_1 D5
#define LED_PIN_2 D6
#define BUZZER_PIN D7

DHT_Unified dht(DHTPIN, DHTTYPE);

float temperature = 0.0;
float humidity = 0.0;

// TIMER
unsigned long lastDHT = 0;
unsigned long dhtInterval = 3000;

unsigned long lastUpdate = 0;
unsigned long lcdInterval = 3000;

unsigned long lastSend = 0;
unsigned long sendInterval = 3000;

unsigned long lastBlink = 0;
unsigned long blinkInterval = 0;

unsigned long lastBlink2 = 0;
unsigned long blinkInterval2 = 0;

unsigned long lastBeep = 0;
unsigned long beepInterval = 0;

bool ledState = LOW;
bool ledState2 = LOW;
bool buzzerState = LOW;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();
}

void loop() {
  unsigned long now = millis();

  if (now - lastDHT >= dhtInterval) {
    lastDHT = now;

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    temperature = event.temperature;

    dht.humidity().getEvent(&event);
    humidity = event.relative_humidity;

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Undetected DHT");
    } else {
      Serial.print("Temperature: "); 
      Serial.print(temperature); 
      Serial.println("°C"); 

      Serial.print("Humidity: "); 
      Serial.print(humidity); 
      Serial.println("%");
    }
  }

  if (blinkInterval > 0 && (now - lastBlink >= blinkInterval)) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LED_PIN_1, ledState);
  }

  if (blinkInterval2 > 0 && (now - lastBlink2 >= blinkInterval2)) {
    lastBlink2 = now;
    ledState2 = !ledState2;
    digitalWrite(LED_PIN_2, ledState2);
  }

  if (beepInterval > 0 && (now - lastBeep >= beepInterval)) {
    lastBeep = now;
    buzzerState = !buzzerState;
    digitalWrite(BUZZER_PIN, buzzerState);
  }

  if (now - lastUpdate >= lcdInterval) {
    lastUpdate = now;

    lcd.clear();

    if (!isnan(temperature) && !isnan(humidity)) {
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temperature, 1);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Humi:");
      lcd.print(humidity, 1);
      lcd.print("%");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Undetected DHT");
    }
  }

  if (now - lastSend >= sendInterval) {
    lastSend = now;

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http1;
      HTTPClient http2;

      String url = "http://192.168.1.10/dhtiot/public/update-data/";
      url += String(temperature, 1) + "/" + String(humidity, 1);

      String url2 = "http://192.168.1.10:3002/sensor/update/";
      url2 += String(temperature, 1) + "/" + String(humidity, 1);\

      // -----------------------------
      // API 1
      // -----------------------------
      http1.begin(client, url);
      int httpCode1 = http1.GET();

      Serial.print("Mengirim data ke "); 
      Serial.println(url);

      if (httpCode1 > 0) {
        Serial.printf("HTTP Response Code: %d\n", httpCode1); 
        // Print ini + code yang nandain sukses atau nggak (200 = sukses) 
        String payload1 = http1.getString(); // Ngambil respons dari httpCode 
        // Serial.println(payload1);

        StaticJsonDocument<255> doc;
        DeserializationError error = deserializeJson(doc, payload1);
        
        String message = doc["message"];

        Serial.print("Response: "); 
        Serial.println(message);
        
        JsonObject data = doc["data"];

        if(error){
          Serial.print("deserialization json gagal: ");
          Serial.println(error.f_str());
          http1.end();
          return;
        }

        int id = data["id"];
        float tempVal = data["temperature"].as<float>();
        float humVal = data["humidity"].as<float>();
        const char* createdAt = data["created_at"];
        const char* updatedAt = data["updated_at"];
        float maxTempVal = data["max_temperature"].as<float>();
        float maxHumVal = data["max_humidity"].as<float>();
        float minTempVal = data["min_temperature"].as<float>();
        float minHumVal = data["min_humidity"].as<float>();

        if (tempVal > maxTempVal) {
          blinkInterval = 150;
          beepInterval = 150;
        } else if (tempVal < minTempVal) {
          blinkInterval = 300;
          beepInterval = 300;
        } else {
          blinkInterval = 0;
          beepInterval = 0;
          digitalWrite(LED_PIN_1, LOW);
          digitalWrite(BUZZER_PIN, LOW);
        }

        if (humVal > maxHumVal) {
          blinkInterval2 = 150;
          beepInterval = 150;
        } else if (humVal > minHumVal) {
          blinkInterval2 = 300;
          beepInterval = 300;
        } else {
          blinkInterval2 = 0;
          beepInterval = 0;
          digitalWrite(LED_PIN_2, LOW);
          digitalWrite(BUZZER_PIN, LOW);
        }
      } else {
        Serial.printf("Gagal mengirim data ke API 1. Error: %s\n", http1.errorToString(httpCode1).c_str());
      }
      http1.end();

      Serial.print("\n");
      // -----------------------------
      // API 2
      // -----------------------------
      // http2.begin(client, url2);
      // int httpCode2 = http2.GET();

      // Serial.print("Mengirim data ke "); 
      // Serial.println(url2);

      // if (httpCode2 > 0) {
      //   Serial.printf("HTTP Response Code: %d\n", httpCode2); 
      //   // Print ini + code yang nandain sukses atau nggak (200 = sukses) 
      //   String payload2 = http2.getString(); // Ngambil respons dari httpCode 

      //   StaticJsonDocument<255> doc;
      //   DeserializationError error = deserializeJson(doc, payload2);
        
      //   String message = doc["message"];

      //   Serial.print("Response: "); 
      //   Serial.println(message);
        
      //   if(error){
      //     Serial.print("deserialization json gagal: ");
      //     Serial.println(error.f_str());
      //     http2.end();
      //     return;
      //   }

      //   int id = doc["id"];
      //   float tempVal = doc["temperature"].as<float>();
      //   float humVal = doc["humidity"].as<float>();
      //   const char* createdAt = doc["created_at"];
      //   const char* updatedAt = doc["updated_at"];

      // } else {
      //   Serial.printf("Gagal mengirim data ke API 2. Error: %s\n", http2.errorToString(httpCode2).c_str());
      // }
      // http2.end();
    }
  }
}
