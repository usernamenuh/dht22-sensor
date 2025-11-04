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

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Koneksi ke WiFi
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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
  // Delay antar pengukuran
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

  // Kirim data ke server Laravel
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

  delay(3000);
}
