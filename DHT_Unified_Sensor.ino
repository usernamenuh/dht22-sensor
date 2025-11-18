#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Konfigurasi Wifi
const char* ssid = "BAYU-MADESA";
const char* password = "050511ko";

#define DHTPIN D4
#define BUZZER D7
#define LEDYELLOW D6
#define LEDRED D1
#define LEDGREEN D2
#define LEDWHITE D3
#define LEDYELLOW2 D8
#define LEDRED2 D5
#define DHTTYPE DHT22

<<<<<<< HEAD
float thresholdTemp = 30.0;
String BASE_URL = "http://192.168.100.13:8000";
int pinLampu[6] = {D6, D1, D2, D3, D8, D5};
=======
// Pin LED dan buzzer
#define LED_HIJAU D3
#define LED_MERAH D2
#define BUZZER D5

float thresholdTemp = 30.0; // nilai default
>>>>>>> 2c57767603ccef5007a9bce5ced53d8c2dddda1c

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setup pin output
<<<<<<< HEAD
  pinMode(LEDYELLOW, OUTPUT);
  pinMode(LEDRED, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDWHITE, OUTPUT);
  pinMode(LEDYELLOW2, OUTPUT);
  pinMode(LEDRED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LEDYELLOW, LOW);
  digitalWrite(LEDRED, LOW);
  digitalWrite(LEDGREEN, LOW);
  digitalWrite(LEDWHITE, LOW);
  digitalWrite(LEDYELLOW2, LOW);
  digitalWrite(LEDRED2, LOW);
=======
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Matikan semua di awal
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, LOW);
>>>>>>> 2c57767603ccef5007a9bce5ced53d8c2dddda1c
  digitalWrite(BUZZER, LOW);

  // Koneksi ke WiFi
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");

  Serial.println();
  Serial.println("WiFi terkoneksi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Inisialisasi sensor DHT
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));

  // Ambil detail sensor
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  }
  
}

void loop() {
  getThresholdFromServer();
<<<<<<< HEAD
  lampu();
=======
>>>>>>> 2c57767603ccef5007a9bce5ced53d8c2dddda1c
  delay(delayMS);

  sensors_event_t event;

  // Baca suhu
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;

  // Baca kelembapan
  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;

  // Cek apakah sensor valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Gagal membaca sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // === Kontrol LED & Buzzer ===
  if (temperature < thresholdTemp) {
<<<<<<< HEAD
    Serial.println("Suhu di bawah threshold.");
    digitalWrite(BUZZER, LOW);
    // Suhu di bawah 30°C → LED hijau berkedip, LED merah & buzzer mati
    /* digitalWrite(LED_MERAH, LOW);
=======
    // Suhu di bawah 30°C → LED hijau berkedip, LED merah & buzzer mati
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(BUZZER, LOW);
>>>>>>> 2c57767603ccef5007a9bce5ced53d8c2dddda1c

    digitalWrite(LED_HIJAU, HIGH);
    delay(1000); // nyala 1 detik
    digitalWrite(LED_HIJAU, LOW);
<<<<<<< HEAD
    delay(1000); // mati 1 detik */

  } else {
    Serial.println("Suhu di atas atau sama dengan threshold.");
    digitalWrite(BUZZER, HIGH); 
    // Suhu 30°C atau lebih → LED merah & buzzer nyala, LED hijau mati
    /* digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_MERAH, HIGH); */
=======
    delay(1000); // mati 1 detik

  } else {
    // Suhu 30°C atau lebih → LED merah & buzzer nyala, LED hijau mati
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(BUZZER, HIGH);
>>>>>>> 2c57767603ccef5007a9bce5ced53d8c2dddda1c
  }

  // === Kirim data ke server Laravel ===
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = BASE_URL + "/update-data/";

    url += String(temperature, 1) + "/" + String(humidity, 1);

    Serial.print("Mengirim data ke: ");
    Serial.println(url);

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println("Response:");
      Serial.println(payload);
    } else {
      Serial.printf("Gagal mengirim data. Error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi tidak terkoneksi, mencoba ulang...");
    WiFi.reconnect();
  }

  delay(2000); // jeda kecil sebelum loop berikutnya
<<<<<<< HEAD
}



void getThresholdFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, BASE_URL + "/get-setting");
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Data setting dari server:");
      Serial.println(payload);

      int pos = payload.indexOf("threshold_temp");
      if (pos != -1) {
        int colon = payload.indexOf(":", pos);
        int end = payload.indexOf("}", colon);
        String valueStr = payload.substring(colon + 1, end);
        thresholdTemp = valueStr.toFloat();
      }
    } else {
      Serial.printf("Gagal ambil setting: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}
}

void lampu() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, BASE_URL + "/get-lampu");
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Data LAMPUU dari server:");
      Serial.println(payload);
      StaticJsonDocument<700> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error){
        for (int i = 0; i < doc.size(); i++) {
          int id = doc[i]["id"];
          String name = doc[i]["name"].as<String>();
          int status = doc[i]["status"];
          
          // ubah lampu 1->index 0
          int index = id - 1;

          int pinLampu[6] = {D6, D1, D2, D3, D8, D5};

          // kontrol LED 
          if (index >= 0 && index < 6) {
            digitalWrite(pinLampu[index], status);

            Serial.print("name : ");
            Serial.print(name);
            Serial.print(" = ");
            
            Serial.println(status);
          }
        }
      }
      
    } else {
      Serial.printf("Gagal ambil LAMPU: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
=======
>>>>>>> 2c57767603ccef5007a9bce5ced53d8c2dddda1c
}



void getThresholdFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://192.168.100.13:8000/get-setting");
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Data setting dari server:");
      Serial.println(payload);

      int pos = payload.indexOf("threshold_temp");
      if (pos != -1) {
        int colon = payload.indexOf(":", pos);
        int end = payload.indexOf("}", colon);
        String valueStr = payload.substring(colon + 1, end);
        thresholdTemp = valueStr.toFloat();
      }
    } else {
      Serial.printf("Gagal ambil setting: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

