// Include WiFi library
#include <WiFiNINA.h>
// Define the pin for the water level sensor
#define WATER_LEVEL_SENSOR_PIN A5
// Define sensor readings for known volumes
// THESE VALUES WILL NEED TO BE UPDATED BASED ON YOUR NEW SENSOR
#define SENSOR_VALUE_AT_ONE_CUP 200   
#define SENSOR_VALUE_AT_TWO_CUPS 750

// Define your WiFi credentials.
#define WIFI_SSID "Your_SSID"          // Replace with your WiFi SSID
#define WIFI_PASSWORD "Your_Password"  // Replace with your WiFi password

// This function maps a float value from one range to another range.
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void printWaterLevelInCups() {
  // Read the value from the eTape sensor
  int sensorValue = analogRead(WATER_LEVEL_SENSOR_PIN);
  
  // Convert the sensor reading to volume in cups
  float volumeInCups = mapFloat(sensorValue, SENSOR_VALUE_AT_ONE_CUP, SENSOR_VALUE_AT_TWO_CUPS, 1, 2);

  // Print the volume to the serial monitor
  Serial.print("Volume: ");
  Serial.print(volumeInCups);
  Serial.println(" cups");
}

void setup() {
  // Begin serial communication
  Serial.begin(9600);
  
  // connect to WiFi
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("WiFi connected");
}

void loop() {
  // Call the printWaterLevelInCups function every second
  printWaterLevelInCups();
  delay(1000);
}