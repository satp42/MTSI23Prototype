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

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);

  if(!accel.begin()) {
    Serial.println("no ADXL345 detected... Check wiring");
    while(1);
  }
}

void loop() {
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

  Blynk.run();
}
