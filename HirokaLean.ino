#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

#define WATER_LEVEL_SENSOR_PIN A5
#define SENSOR_VALUE_AT_ONE_CUP 200
#define SENSOR_VALUE_AT_TWO_CUPS 750
#define ACCELERATION_THRESHOLD 0.8


char ssid[] = "";
char password[] = "";
char serverAddress = "http://localhost:3000/";
int serverPort = 3000;

WiFiClient wifiClient;
HttpClient httpClient = HttpClient(wifiClient, serverAddress, serverPort);

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

void setup() {
  Serial.begin(9600);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if (!accel.begin()){
    Serial.println("Failed to initialize accelerometer");
    while (1);
  }
}

void loop() {
  float accelZ = readAccelZ();
  if (accelZ < ACCELERATION_THRESHOLD) {
    float currentVolume = printWaterLevelInCups();
    httpClient.post("/your-endpoint", "application/json", String(currentVolume, 2));
  }
  delay(1000);
}

float printWaterLevelInCups() {
  // Read the value from the sensor
  int sensorValue = analogRead(WATER_LEVEL_SENSOR_PIN);
  // Convert the sensor reading to volume in cups
  float volumeInCups = mapFloat(sensorValue, SENSOR_VALUE_AT_ONE_CUP, SENSOR_VALUE_AT_TWO_CUPS, 1, 2);
  
  return volumeInCups;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float readAccelZ() {
  sensors_event_t event; 
  accel.getEvent(&event);
  float zAcceleration = event.acceleration.z;  // Get Z axis acceleration
  return zAcceleration;
}