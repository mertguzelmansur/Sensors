#define RX_PIN 16 // GPIO 16 for RX2
#define TX_PIN 17 // GPIO 17 for TX2

void setup() {
  Serial.begin(9600); // Initialize serial communication for UART1 (for PC communication)
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize UART2 with RX and TX pins for STM8S103F3 communication
}

void loop() {
  if (Serial2.available()) {
    uint8_t data = Serial2.read(); // Read data from the first STM8S103F3 board
    Serial.print("Seat belt status: ");
    if (data & 0x08) Serial.print("1 ");
    else Serial.print("0 ");
    if (data & 0x04) Serial.print("1 ");
    else Serial.print("0 ");
    if (data & 0x02) Serial.print("1 ");
    else Serial.print("0 ");
    if (data & 0x01) Serial.print("1");
    else Serial.print("0");
    Serial.println();
  }

  // Add code here to handle communication with PC via Serial (UART1)

  delay(100); // Add a delay to avoid flooding the serial monitor
}