// Define the pin for the water level sensor
#define WATER_LEVEL_SENSOR_PIN A5
// Define sensor readings for known volumes
#define SENSOR_VALUE_AT_ONE_CUP 200
#define SENSOR_VALUE_AT_TWO_CUPS 750

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void printWaterLevelInCups() {
  // Read the value from the sensor
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
}

void loop() {
  // Call the printWaterLevelInCups function
  printWaterLevelInCups();
  // Wait for some time before the next reading
  delay(1000);
}