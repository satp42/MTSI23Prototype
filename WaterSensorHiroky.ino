#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define FLOW_SENSOR_PIN A1  // The pin the flow sensor is connected to
#define SIP_THRESHOLD 1.8  // Threshold to detect a sip
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

  // Setting the range of accelerometer
  accel.setRange(ADXL345_RANGE_16_G);

  // Set up flow sensor
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, RISING);
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  if (millis() - lastFlowCalc >= FLOW_CALC_INTERVAL) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));

    waterVolume += pulseCount * PULSE_VOLUME;
    lastFlowCalc = millis();

    // Convert volume to cups
    float waterVolumeCups = waterVolume / ML_PER_CUP;

    Serial.print("Water Volume: ");
    Serial.print(waterVolumeCups);
    Serial.println(" cups");

    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, RISING);
  }

  // if (fabs(event.acceleration.y) > SIP_THRESHOLD) {
  //   if (millis() - lastAccelMeasure >= FLOW_CALC_INTERVAL) {
  //     lastAccelMeasure = millis();
  //     sips++;
  //     Serial.print("Sips: ");
  //     Serial.println(sips);
  //   }
  // }
}