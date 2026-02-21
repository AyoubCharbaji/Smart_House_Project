#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// ======== CONFIG WIFI & FIREBASE ========
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

const String databaseURL = "https://smart-house-projet-default-rtdb.europe-west1.firebasedatabase.app";
const String apiKey = "";

// ======== PINS ========
#define DHTPIN  4
#define DHTTYPE DHT22
#define LED1 13
#define LED2 12
#define LED3 14
#define LED4 15
#define LDR_PIN 34
#define MQ_PIN  35

DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;

// ======== FONCTIONS FIREBASE ========
void sendObject(String path, String jsonData, bool usePost = false) {
  String url = databaseURL + path + ".json?auth=" + apiKey;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  int code;
  if (usePost) {
    code = http.POST(jsonData);           // push() = POST
  } else {
    code = http.PUT(jsonData);            // remplace = PUT
  }

  if (code > 0) {
    Serial.printf("Envoi %s [%d]\n", usePost ? "POST OK" : "PUT OK", code);
  } else {
    Serial.printf("Erreur envoi : %s\n", http.errorToString(code).c_str());
  }
  http.end();
}

int getInt(String path) {
  String url = databaseURL + path + ".json?auth=" + apiKey;
  http.begin(url);
  int code = http.GET();
  int value = 0;
  if (code == 200) {
    String payload = http.getString();
    payload.trim();
    if (payload != "null" && payload.length() > 0) {
      value = payload.toInt();
    }
  }
  http.end();
  return value;
}

// ======== SETUP & LOOP (inchangés jusqu’à l’envoi) ========
void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT); pinMode(LED2, OUTPUT); pinMode(LED3, OUTPUT); pinMode(LED4, OUTPUT);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.println("\nWiFi connecté !");
}

void loop() {
  int luxLevel = analogRead(LDR_PIN);
  int gasLevel = analogRead(MQ_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Erreur DHT22");
    delay(1000);
    return;
  }

  // === Logique LEDs et alarme (inchangée) ===
  int ldrThreshold = getInt("/settings/ldr_threshold");
  if (ldrThreshold <= 0 || ldrThreshold > 4095) ldrThreshold = 1800;

  bool led1State = true, led2State = true, led3State = true;
  if (luxLevel < ldrThreshold / 3) {
    led1State = true; led2State = false; led3State = false;
  } else if (luxLevel < 2 * ldrThreshold / 3) {
    led1State = true; led2State = true;  led3State = false;
  }

  int gasAlertThreshold = getInt("/settings/gas_threshold");
  if (gasAlertThreshold < 100) gasAlertThreshold = 1000;
  int tempAlertThreshold = getInt("/settings/temp_threshold");
  if (tempAlertThreshold < 20) tempAlertThreshold = 40;

  bool alarmActive = (t > tempAlertThreshold || gasLevel > gasAlertThreshold);
  digitalWrite(LED1, led1State); digitalWrite(LED2, led2State); digitalWrite(LED3, led3State);
  digitalWrite(LED4, alarmActive);
  if (alarmActive) tone(LED4, 2000, 200);

  // === ENVOI VERS FIREBASE ===
  unsigned long timestamp = millis();

  // 1. Dernière mesure (toujours écrasée → rapide)
  String jsonLast = "{"
    "\"temp\":"   + String(t, 1) + ","
    "\"hum\":"    + String(h, 1) + ","
    "\"light\":"  + String(luxLevel) + ","
    "\"gas\":"    + String(gasLevel) + ","
    "\"alarm\":"  + (alarmActive ? "true" : "false") + ","
    "\"timestamp\":" + String(timestamp) +
  "}";
  sendObject("/smart_house/sensor_data/last", jsonLast);  // PUT

  // 2. Historique : NOUVELLE entrée à chaque boucle (push)
  String jsonHistory = "{"
    "\"temp\":"   + String(t, 1) + ","
    "\"hum\":"    + String(h, 1) + ","
    "\"light\":"  + String(luxLevel) + ","
    "\"gas\":"    + String(gasLevel) + ","
    "\"room\":\"Maison\","
    "\"timestamp\":" + String(timestamp) +
  "}";
  sendObject("/smart_house/sensor_data/history", jsonHistory, true);  // POST = push

  Serial.printf("Envoyé → Temp: %.1f°C | Hum: %.1f%% | Gaz: %d | Lum: %d\n", t, h, gasLevel, luxLevel);

  delay(2000); // toutes les 2 secondes (ou 1000 si tu veux plus rapide)
}
