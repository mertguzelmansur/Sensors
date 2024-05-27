// STM8S103F3 Code (Arduino IDE)

#include "stm8s.h"
#define SENSOR_PIN 2  // Replace with the actual pin connected to the seat belt sensor

bool lastSensorState = false;  // Variable to store the previous sensor state

void setup() {
  pinMode(SENSOR_PIN, INPUT_PULLUP);  // Configure sensor pin as input with pull-up
  Serial_begin(9600);  // Initialize serial communication
}

void loop() {
  bool currentSensorState = !digitalRead(SENSOR_PIN);  // Read the sensor state (assuming active low)

  if (currentSensorState != lastSensorState) {
    if (currentSensorState) {
      Serial_write('1');  // Send '1' to ESP32 to indicate seat belt is unbuckled
    } else {
      Serial_write('0');  // Send '0' to ESP32 to indicate seat belt is buckled
    }
    lastSensorState = currentSensorState;  // Update the last sensor state
  }

  delay(100);  // Add a delay to avoid flooding the ESP32 with data
}