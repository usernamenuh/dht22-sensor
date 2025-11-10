#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Konfigurasi Wifi
const char* ssid = "BAYU-MADESA";
const char* password = "050511ko";

#define DHTPIN D4
#define DHTTYPE DHT22

// Pin LED dan buzzer
#define LED_HIJAU D3
#define LED_MERAH D2
#define BUZZER D5

float thresholdTemp = 30.0; // nilai default

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setup pin output
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Matikan semua di awal
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, LOW);
  digitalWrite(BUZZER, LOW);

  // Koneksi ke WiFi
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

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

void loop() {
  getThresholdFromServer();
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
    // Suhu di bawah 30°C → LED hijau berkedip, LED merah & buzzer mati
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(BUZZER, LOW);

    digitalWrite(LED_HIJAU, HIGH);
    delay(1000); // nyala 1 detik
    digitalWrite(LED_HIJAU, LOW);
    delay(1000); // mati 1 detik

  } else {
    // Suhu 30°C atau lebih → LED merah & buzzer nyala, LED hijau mati
    digitalWrite(LED_HIJAU, LOW);
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(BUZZER, HIGH);
  }

  // === Kirim data ke server Laravel ===
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://192.168.100.13:8000/update-data/";
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

