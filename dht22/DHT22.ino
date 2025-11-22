#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* ssid = "SIB BLOK E3 NO 14";
const char* password = "3783140504Okay";

#define DHTPIN D3
#define DHTTYPE DHT22
#define LED_PIN_1 D5
#define BUZZER_PIN D6

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

unsigned long lastBeep = 0;
unsigned long beepInterval = 0;

bool ledState = LOW;
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
      Serial.print("Temperatrure: "); 
      Serial.print(temperature); 
      Serial.println("°C"); 

      Serial.print("Humidity: "); 
      Serial.print(humidity); 
      Serial.println("%");
    }
  }

  if (temperature >= 30.0) {
    blinkInterval = 150;
    // beepInterval  = 150;
  } 
  else if (temperature <= 21.0) {
    blinkInterval = 300;
    // beepInterval  = 300;
  } 
  else {
    blinkInterval = 0;
    // beepInterval = 0;
    digitalWrite(LED_PIN_1, HIGH);
    // digitalWrite(BUZZER_PIN, LOW);
  }

  if ((temperature >= 30.0 && humidity >= 90) || (temperature <= 30.0 && humidity <= 60)) {
    beepInterval = 50;
  } else if ((temperature <= 30.0 && humidity >= 90) || (temperature >= 30.0 && humidity <= 60)) {
    beepInterval = 100;
  } else {
    beepInterval = 0;
    digitalWrite(BUZZER_PIN, LOW);
  }

  // if (humidity >= 60.0) {
  //   blinkInterval = 150;
  //   beepInterval  = 150;
  // } 
  // else if (humidity <= 45.0) {
  //   blinkInterval = 300;
  //   beepInterval  = 300;
  // } 
  // else {
  //   blinkInterval = 0;
  //   beepInterval = 0;
  //   digitalWrite(LED_PIN_1, LOW);
  //   digitalWrite(BUZZER_PIN, LOW);
  // }

  if (blinkInterval > 0 && (now - lastBlink >= blinkInterval)) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LED_PIN_1, ledState);
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
      lcd.print(temperature, 1);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0, 1);
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
      HTTPClient http;

      String url = "http://192.168.1.10/dhtiot/public/update-data/";
      url += String(temperature, 1) + "/" + String(humidity, 1);

      String url2 = "http://192.168.1.10:3002/sensor/update/";
      url2 += String(temperature, 1) + "/" + String(humidity, 1);
      
      http.begin(client, url);
      http.begin(client, url2);
      int httpCode = http.GET();

      Serial.print("Mengirim data ke "); 
      Serial.println(url);
      Serial.print("dan ke ");
      Serial.println(url2);

      if (httpCode > 0) {
        Serial.printf("HTTP Response Code: %d\n", httpCode); 
        // Print ini + code yang nandain sukses atau nggak (200 = sukses) 
        String payload = http.getString(); // Ngambil respons dari httpCode 
        Serial.println("Response: "); 
        Serial.println(payload);
      } else {
        Serial.printf("Gagal mengirim data. Error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
  }
}
