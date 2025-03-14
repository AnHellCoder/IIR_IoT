#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

int DHTpin = 2; // Пин, к которому подключен DHT11
SimpleDHT11 dht11(DHTpin); // Создаём объект датчика

const char* ssid = "IIR_WiFi";    //  Your Wi-Fi Name
const char* password = "deeprobotics";   // Wi-Fi Password

WiFiServer server(80);

void setup() {
  Serial.begin(115200); // Инициализация Serial

  Serial.print("Connecting to the Newtork");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)

  {

    delay(500);

    Serial.print(".");

  }

  Serial.println("WiFi connected"); 

  server.begin();  // Starts the Server

  Serial.println("Server started");

 

  Serial.print("IP Address of network: "); // will IP address on Serial Monitor

  Serial.println(WiFi.localIP());

  Serial.print("Copy and paste the following URL: http://"); // Will print IP address in URL format

  Serial.print(WiFi.localIP());

  Serial.println("/");
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  int err = dht11.read(&temperature, &humidity, NULL); // Чтение данных

  if (err == SimpleDHTErrSuccess) {
    Serial.print("Temperature: "); Serial.print((int)temperature); Serial.println(" ºC");
    Serial.print("Humidity: "); Serial.print((int)humidity); Serial.println(" %");
  } else {
    Serial.print("Failed to read from DHT11, error code: "); Serial.println(err);
  }

  delay(2000);

  WiFiClient client = server.available();

  if (!client)

  {

    return;

  }

  Serial.println("Waiting for new client");

  while(!client.available())

  {

    delay(1);

  }

//*------------------HTML Page Code---------------------*//

  client.println("HTTP/1.1 200 OK"); //

  client.println("Content-Type: text/html");

  client.println("");

  client.println("<!DOCTYPE HTML>");

  client.println("<html>");

 

  client.print("Temperature "); client.print((int)temperature); client.println(" ºC");
  client.println("<br><br>");
  client.print("Humidity: "); client.print((int)humidity); client.println("%");
 

  delay(1);

  Serial.println("Client disonnected");

  Serial.println("");
}
