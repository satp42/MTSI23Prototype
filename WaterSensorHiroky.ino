#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

char ssid[] = "Detkin IOT North";
char pass[] = "n0rthC0untry";

// add your server details here
char serverAddress[] = "10.102.245.23";  // your server's url or IP address
int  serverPort = 3000;                       // your server's listening port

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, serverPort);

#define FLOW_SENSOR_PIN A1  // The pin the flow sensor is connected to
#define SIP_THRESHOLD 10  // Threshold to detect a sip
#define PULSE_VOLUME 2.25  // Volume per pulse for the flow sensor (mL), placeholder value
#define FLOW_CALC_INTERVAL 1000  // Calculation interval for the flow (ms)
#define ML_PER_CUP 237.0  // Milliliters in a cup

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

unsigned long lastFlowCalc = 0;
unsigned long lastAccelMeasure = 0;

volatile int pulseCount = 0; // Counts flow sensor pulses
float waterVolume = 0.0; // Total amount of water passed through sensor in mL
unsigned int sips = 0; // Counts number of sips

void flowPulseCounter()
{
  pulseCount++; // Increment pulse counter
}

void setup() {
  Serial.begin(9600);

  if (!accel.begin()) {
    Serial.println("Failed to start accelerometer. Check wiring, then reset.");
    while (1);
  }
  Serial.println("Accelerometer initialized!");

  
  // Setting the range of accelerometer
  accel.setRange(ADXL345_RANGE_16_G);

  // Set up flow sensor
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, RISING);

  // Add this WiFi connection code to 'setup()' function
  Serial.print("Connecting to WiFi... ");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("WiFi Connected!");
}

void loop() {
  
  sensors_event_t event;
  accel.getEvent(&event);

  if (millis() - lastFlowCalc >= FLOW_CALC_INTERVAL) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));

    waterVolume += pulseCount * PULSE_VOLUME;
    lastFlowCalc = millis();

    // Convert volume to cups
    float waterVolumeCups = waterVolume;

    Serial.print("Water Volume: ");
    Serial.print(waterVolumeCups);
    Serial.println(" cups");

    String postData = "Water Volume: " + String(waterVolumeCups) + " mL";

    client.beginRequest();
    client.post("/api/data"); 
    client.sendHeader("Content-Type", "application/x-www-form-urlencoded"); 
    client.sendHeader("Content-Length", postData.length());
    client.beginBody();
    client.print(postData);
    client.endRequest(); // no need to assign the return of this to any variable

    // get the status response of the HTTP request
    int statusCode = client.responseStatusCode();
    if (statusCode >= 0) {
      Serial.print("POST request was successful, status code: ");
      Serial.println(statusCode);
    } else {
      Serial.print("Failed to send POST request: ");
      Serial.println(statusCode);
    }
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, RISING);
  }

  if (fabs(event.acceleration.y) > SIP_THRESHOLD) {
    if (millis() - lastAccelMeasure >= FLOW_CALC_INTERVAL) {
      lastAccelMeasure = millis();
      sips++;
      Serial.print("Sips: ");
      Serial.println(sips);
    }
  }
}