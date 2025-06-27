#include <OneWire.h>
#include <DallasTemperature.h>

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

  Serial.println("Temperature and pH monitoring started!");
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

  // Print temperature + pH every printInterval
  if (currentMillis - printTime > printInterval) {
    DS18B20.requestTemperatures();
    temperature = DS18B20.getTempCByIndex(0);

    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" °C    ");

    Serial.print("Voltage: ");
    Serial.print(voltage, 2);
    Serial.print("    pH: ");
    Serial.println(pHValue, 2);

    digitalWrite(LED, digitalRead(LED) ^ 1); // Toggle LED
    printTime = currentMillis;
  }
}
