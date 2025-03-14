#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "IIR_WiFi";
const char* password = "deeprobotics";

const char* slaveUrl = "http://192.168.1.101/setState"; // IP-адрес ведомого светофора

int redPin = D1;
int yellowPin = D2;
int greenPin = D3;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  // Зеленый сигнал
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, HIGH);
  sendState(slaveUrl, "red"); // Ведомый горит красным
  delay(5000); // Зеленый горит 5 секунд

  // Желтый сигнал
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(greenPin, LOW);
  sendState(slaveUrl, "yellow"); // Ведомый горит желтым
  delay(2000); // Желтый горит 2 секунды

  // Красный сигнал
  digitalWrite(redPin, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, LOW);
  sendState(slaveUrl, "green"); // Ведомый горит зеленым
  delay(5000); // Красный горит 5 секунд

  // Желтый сигнал
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(greenPin, LOW);
  sendState(slaveUrl, "yellow"); // Ведомый горит желтым
  delay(2000); // Желтый горит 2 секунды
}

void sendState(const char* url, const char* state) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client; // Создаем объект WiFiClient
    http.begin(client, url); // Используем новый API

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["state"] = state;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.printf("HTTP POST to %s with state %s. Response code: %d\n", url, state, httpResponseCode);
    } else {
      Serial.printf("Error sending HTTP POST to %s. Error code: %d\n", url, httpResponseCode);
    }

    http.end();
  }
}
