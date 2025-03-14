#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// Настройки Wi-Fi
const char* ssid = "IIR_WiFi";       // Замените на имя вашей Wi-Fi сети
const char* password = "deeprobotics"; // Замените на пароль от вашей Wi-Fi сети

// IP-адрес другой платы
const char* otherBoardIP = "192.168.68.54"; // Замените на IP-адрес другой платы

// Пины
const int buttonPin = 0;  // GPIO4 для кнопки
const int ledPin = 2;     // GPIO2 для светодиода

// Переменные
int lastButtonState = HIGH; // Для отслеживания состояния кнопки
bool ledState = false;      // Текущее состояние светодиода

// Веб-сервер
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Настройка пинов
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Настройка маршрутов сервера
  server.on("/led", HTTP_GET, handleLedRequest);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); // Обработка входящих запросов

  // Обработка нажатия кнопки
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(20); // Антидребезг
    if (digitalRead(buttonPin) == LOW) {
      toggleOtherBoardLED(); // Отправка запроса на другую плату
    }
  }
  lastButtonState = buttonState;
}

// Обработка GET-запроса на /led
void handleLedRequest() {
  ledState = !ledState; // Инвертируем состояние светодиода
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  server.send(200, "text/plain", ledState ? "LED is ON" : "LED is OFF");
  Serial.println(ledState ? "LED turned ON" : "LED turned OFF");
}

// Отправка GET-запроса на другую плату
void toggleOtherBoardLED() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://" + String(otherBoardIP) + "/led";
    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("GET request sent to %s, response code: %d\n", url.c_str(), httpCode);
    } else {
      Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Wi-Fi not connected");
  }
}
