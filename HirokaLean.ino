#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "";
char ssid[] = "";
char pass[] = "";

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

bool isDrinking = false;
int drinkingEvents = 0;

float tiltThreshold = 1.3;

unsigned long drinkTime = 2000;
unsigned long drinkStart = 0;

int distance = 0;

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
  // Sensing the orientation of water bottle using ultrasonic sensor
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  distance = pulseIn(ECHO_PIN, HIGH) / 58;   //58 is used for converting the sensed signal to cm
  
  // Assuming that water is sensed only when distance is less than 15 cm
  // The constant 15 cm should be replaced by the actual height of your water bottle
  if (distance < 15) {
    // When the bottle is standing, start sensing the acceleration
    sensors_event_t event;
    accel.getEvent(&event);

    if(event.acceleration.x > tiltThreshold || event.accleration.y > tiltThreshold) {
      if (!isDrinking) {
        drinkStart = millis();
        isDrinking = true;
      }
      else if (isDrinking && (millis() - drinkStart >= drinkTime)) {
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