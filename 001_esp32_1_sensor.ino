//#define SEAT_BELT_SENSOR_PIN 16 // Replace with the appropriate GPIO pin connected to the STM8S103F3 TX
#include <HardwareSerial.h>

#define SERIAL_BAUD 9600

bool lastSeatBeltStatus = false; // Variable to store the previous seat belt status

HardwareSerial serialPort(2); // Use UART2 for communication

void setup() {
  Serial.begin(SERIAL_BAUD); // Initialize serial communication
  serialPort.begin(SERIAL_BAUD, SERIAL_8N1, 16, 17); // Set up UART2 with RX=16, TX=17

  //pinMode(SEAT_BELT_SENSOR_PIN, INPUT); // Set the sensor pin as input
}

void loop() {
  if (serialPort.available()) {
    char data = serialPort.read(); // Read data from STM8S103F3
    bool currentSeatBeltStatus = (data == '0'); // Convert received data to a boolean (true for '1', false for '0')

    if (currentSeatBeltStatus != lastSeatBeltStatus) {
      if (currentSeatBeltStatus) {
        Serial.println("Emniyet Kemeri Durumu: Kemer takili degil");
      } else {
        Serial.println("Emniyet Kemeri Durumu: Kemer takili");
      }
      lastSeatBeltStatus = currentSeatBeltStatus; // Update the last seat belt status
    }
  }
/*
  bool sensorStatus = digitalRead(SEAT_BELT_SENSOR_PIN); // Read the sensor status directly
  if (sensorStatus != lastSeatBeltStatus) {
    if (sensorStatus) {
      Serial.println("Seat belt status: Unbuckled");
    } else {
      Serial.println("Seat belt status: Buckled");
    }
    lastSeatBeltStatus = sensorStatus; // Update the last seat belt status
  }
*/
  delay(100); // Add a delay to avoid flooding the serial monitor
}