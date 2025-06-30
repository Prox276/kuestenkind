/*
* Projekt: ESP8266 Datenlogger
* Beschreibung: Erfasst und sendet Sensordaten an einen Webserver. 
* Wir verwenden die Boardconfig ' LOLIN(WEMOS) D1 R2 & mini ' für unseren ESP8266, 
* d.h. vorher Extension für ESP8266 installieren!! Nicht vergessen die Platzhalter
* zu ersetzen :3 
*
* Autor: Prox
* CoAutor: Deiras
*
* Erstellt am: 25.06.2025 
* Zuletzt aktualisiert: 30.06.2025
* Update: das "wlan-update" überarbeitet und nochmals sinnvoll aufgeteilt, da
* gottverdammt sich mein Code fast von sich selbst erlösen wollte. Und API hin-
* zugefügt! 
*
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi-Einstellungen
const char* ssid = "hi team";
const char* password = "ohmygawdd";

// Server-Einstellungen
const char* server = "ip geb ich nicht >:("; // lokale ip des servers
const int port = 3000;
const char* apiKey = "meow"; // muss mit .env übereinstimmen

// DS18B20 Temperature Sensor
#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temperature;

// pH Sensor
#define SensorPin A0
#define Offset 0.00
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLength 40

int pHArray[ArrayLength];
int pHArrayIndex = 0;
float pHValue, voltage;

// Timing
unsigned long samplingTime = 0;
unsigned long printTime = 0;
unsigned long uploadTime = 0;
const unsigned long uploadInterval = 30000; // 30 Sekunden

// Function to average analog values with outlier removal
double averageArray(int* arr, int number) {
  long amount = 0;
  int min, max;

  if (number < 5) {
    for (int i = 0; i < number; i++) {
      amount += arr[i];
    }
    return (double)amount / number;
  }

  if (arr[0] < arr[1]) {
    min = arr[0]; max = arr[1];
  } else {
    min = arr[1]; max = arr[0];
  }

  for (int i = 2; i < number; i++) {
    if (arr[i] < min) {
      amount += min;
      min = arr[i];
    } else if (arr[i] > max) {
      amount += max;
      max = arr[i];
    } else {
      amount += arr[i];
    }
  }
  return (double)amount / (number - 2);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  DS18B20.begin();

  // WiFi-Verbindung herstellen
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED, !digitalRead(LED)); // LED blinken lassen während Verbindung
  }
  
  Serial.println("\nWiFi verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());
  Serial.print("Server-URL: http://");
  Serial.print(server);
  Serial.print(":");
  Serial.print(port);
  Serial.println("/api/add");
  
  digitalWrite(LED, HIGH); // LED dauerhaft an bei Verbindung
}

void sendDataToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    String url = "http://";
    url += server;
    url += ":";
    url += port;
    url += "/api/add";
    
    String jsonData = "{\"temperature\":";
    jsonData += String(temperature, 2);
    jsonData += ",\"ph\":";
    jsonData += String(pHValue, 2);
    jsonData += "}";

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);
    
    int httpCode = http.POST(jsonData);
    
    if (httpCode == HTTP_CODE_CREATED) {
      Serial.println("yippie! Daten erfolgreich gesendet.");
    } else {
      Serial.printf("Fehler %d: %s\n", httpCode, http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WIFI IST NICHT VERBUNDEN. DIE DATEN KÖNNEN DESHALB NICHT GESENDET WERDEN. FIX IT, ASAP.");
  }
}

void loop() {
  unsigned long currentMillis = millis(); 

  // pH sampling
  if (currentMillis - samplingTime > samplingInterval) {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;

    voltage = averageArray(pHArray, ArrayLength) * 5.0 / 1024.0;
    pHValue = 3.5 * voltage + Offset;

    samplingTime = currentMillis;
  }

  // Temperature reading
  if (currentMillis - printTime > printInterval) {
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);

    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" °C    ");

    Serial.print("Voltage: ");
    Serial.print(voltage, 2);
    Serial.print(" V    pH: ");
    Serial.print(pHValue, 2);
    Serial.print("    WiFi: ");
    Serial.println(WiFi.RSSI());

    digitalWrite(LED, digitalRead(LED) ^ 1); // Toggle LED
    printTime = currentMillis;
  }

  // Daten an Server senden
  if (currentMillis - uploadTime > uploadInterval) {
    sendDataToServer();
    uploadTime = currentMillis;
  }
}
