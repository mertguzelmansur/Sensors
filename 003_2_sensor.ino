// Second STM8S103F3 Board (Arduino IDE)

#include "stm8s.h"
#define SENSOR_PIN 1  // Replace with the actual pin connected to the seat belt sensor

void setup() {
  pinMode(SENSOR_PIN, INPUT_PULLUP);  // Configure sensor pin as input with pull-up
  Serial_begin(9600);  // Initialize serMial communication
}

void loop() {
  bool sensorState = !digitalRead(SENSOR_PIN); // Read the sensor state (assuming active low)
  uint8_t receivedData = Serial_read();        // Read data from the third STM8S103F3 board
  uint8_t combinedData = (sensorState ? 0x04 : 0x00) | receivedData; // Combine sensor data with received data
  Serial_write(combinedData);                  // Send combined data to the first STM8S103F3 board
  delay(2000);                                  // Add a delay to avoid flooding the next board
}