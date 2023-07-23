#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

char auth[] = "";
char ssid[] = "";
char pass[] = "";

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

bool isDrinking = false;
int drinkCount = 0;

float tiltThreshold = 1.3;

unsigned long drinkTime = 2000;
unsigned long drinkStart = 0;

// Define the trigger and echo pin of ultrasonic sensor
#define TRIGGER_PIN 2   // You can change it to any other suitable Digital IO pin
#define ECHO_PIN 3      // You can change it to any other suitable Digital IO pin

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);

  if(!accel.begin()) {
    Serial.println("no ADXL detected... Check wiring");
    while(1);
  }

  // Initialize ultrasonic sensor pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Sensing the orientation of water bottle using ultrasonic sensor
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  int distance = pulseIn(ECHO_PIN, HIGH, 20000) / 58; // Timeout after 20ms to prevent long waits if no echo received

  // Assuming that water is sensed only when distance is less than 15 cm
  if (distance < 15) {
    // When the bottle is standing, start sensing the acceleration
    sensors_event_t event;
    accel.getEvent(&event);

    if(event.acceleration.x > tiltThreshold || event.acceleration.y > tiltThreshold) {
      if (!isDrinking) {
        drinkStart = currentMillis;
        isDrinking = true;
      }
      else if (isDrinking && (currentMillis - drinkStart >= drinkTime)) {
        drinkCount++;
        Blynk.virtualWrite(V0, drinkCount);
        isDrinking = false;
      }
    } else {
      isDrinking = false;
    }
  }

  Blynk.run();
}