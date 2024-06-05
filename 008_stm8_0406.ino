/*
// STM8S103F3 Board (Arduino IDE)

#include "stm8s.h"

#define SEAT_1_PIN 0  // Replace with the actual pin connected to seat sensor 1
#define BELT_1_PIN 1  // Replace with the actual pin connected to belt sensor 1
#define SEAT_2_PIN 2  // Replace with the actual pin connected to seat sensor 2
#define BELT_2_PIN 3  // Replace with the actual pin connected to belt sensor 2

#define MAX_BOARDS 4
#define END_BIT 0x23

uint8_t receivedData[MAX_BOARDS * 2]; // Array to store the received data from previous boards
uint8_t receivedDataIndex = 0; // Index to keep track of received data position
bool endBitReceived = false; // Flag to indicate if end bit is received

uint8_t con1, con2;

void setup() {
  pinMode(SEAT_1_PIN, INPUT_PULLUP);  // Configure seat sensor 1 pin as input with pull-up
  pinMode(BELT_1_PIN, INPUT_PULLUP);  // Configure belt sensor 1 pin as input with pull-up
  pinMode(SEAT_2_PIN, INPUT_PULLUP);  // Configure seat sensor 2 pin as input with pull-up
  pinMode(BELT_2_PIN, INPUT_PULLUP);  // Configure belt sensor 2 pin as input with pull-up
  Serial_begin(9600);
}

void loop() {
  bool seat_1 = !digitalRead(SEAT_1_PIN);    // Read seat sensor 1 state (assuming active low)
  bool belt_1 = !digitalRead(BELT_1_PIN);    // Read belt sensor 1 state (assuming active low)
  bool seat_2 = !digitalRead(SEAT_2_PIN);    // Read seat sensor 2 state (assuming active low)
  bool belt_2 = !digitalRead(BELT_2_PIN);    // Read belt sensor 2 state (assuming active low)

  if (seat_1 == 0 && belt_1 == 0) {
    con1 = 0x35;
  } else if (seat_1 == 1 && belt_1 == 0) {
    con1 = 0x37;
  } else if (seat_1 == 0 && belt_1 == 1) {
    con1 = 0x38;
  } else if (seat_1 == 1 && belt_1 == 1) {
    con1 = 0x36;
  }

  if (seat_2 == 0 && belt_2 == 0) {
    con2 = 0x35;
  } else if (seat_2 == 1 && belt_2 == 0) {
    con2 = 0x37;
  } else if (seat_2 == 0 && belt_2 == 1) {
    con2 = 0x38;
  } else if (seat_2 == 1 && belt_2 == 1) {
    con2 = 0x36;
  }

  // Check if at least (MAX_BOARDS * 2) bytes are available
  if (Serial_available() >= (MAX_BOARDS * 2)) {
    receivedDataIndex = 0; // Reset the index
    endBitReceived = false; // Reset the end bit flag
    for (int i = 0; i < (MAX_BOARDS * 2); i++) {
      uint8_t byte = Serial_read(); // Read the received data byte by byte
      receivedData[i] = byte;
      if (byte == END_BIT) {
        endBitReceived = true; // Set the end bit flag if end bit is received
        break; // Exit the loop after receiving the end bit
      }
    }
  }

  // Send the received data first, then the board's own data
  if (endBitReceived) {
    for (int i = 0; i < receivedDataIndex; i++) {
      Serial_write(receivedData[i]);
    }
    Serial_write(con1);
    Serial_write(con2);
    Serial_write(END_BIT); // Send the end bit after the board's data
  }

  receivedDataIndex += 2; // Update the index for the next board
  delay(1000); // Add a delay to avoid flooding the next board
}
*/
/////////////////////////////////////////////////////////////////////////

*

// STM8S103F3 Board (Arduino IDE)

#include "stm8s.h"

#define SEAT_1_PIN 0  
#define BELT_1_PIN 1  
#define SEAT_2_PIN 2 
#define BELT_2_PIN 3  

#define MAX_BOARDS 4 

uint8_t receivedData[MAX_BOARDS * 2]; 
uint8_t receivedDataIndex = 0; 

uint8_t con1, con2;

void setup() {
  pinMode(SEAT_1_PIN, INPUT_PULLUP);  
  pinMode(BELT_1_PIN, INPUT_PULLUP);  
  pinMode(SEAT_2_PIN, INPUT_PULLUP); 
  pinMode(BELT_2_PIN, INPUT_PULLUP);  
  Serial_begin(9600); 
}

void loop() {
  bool seat_1 = !digitalRead(SEAT_1_PIN);    
  bool belt_1 = !digitalRead(BELT_1_PIN);   
  bool seat_2 = !digitalRead(SEAT_2_PIN);    
  bool belt_2 = !digitalRead(BELT_2_PIN);    


  if (seat_1 == 0 && belt_1 == 0) {
    con1 = 0x35;
  } else if (seat_1 == 1 && belt_1 == 0) {
    con1 = 0x37;
  } else if (seat_1 == 0 && belt_1 == 1) {
    con1 = 0x38;
  } else if (seat_1 == 1 && belt_1 == 1) {
    con1 = 0x36;
  }

  if (seat_2 == 0 && belt_2 == 0) {
    con2 = 0x35;
  } else if (seat_2 == 1 && belt_2 == 0) {
    con2 = 0x37;
  } else if (seat_2 == 0 && belt_2 == 1) {
    con2 = 0x38;
  } else if (seat_2 == 1 && belt_2 == 1) {
    con2 = 0x36;
  }


  if (Serial_available() >= (MAX_BOARDS * 2)) {
    receivedDataIndex = 0; 
    for (int i = 0; i < (MAX_BOARDS * 2); i++) {
      receivedData[i] = Serial_read(); 
      //Serial_write(receivedData[i]);
    }
  }

 
  for (int i = 0; i < receivedDataIndex; i++) {
    Serial_write(receivedData[i]);}
    
  Serial_write(con1);
  Serial_write(con2);
  Serial_write(0x23);

  receivedDataIndex += 2; 
  delay(1000); 
}

///////////////////////////////////////////////////////////////////

/*
  uint16_t combinedData = (con1 << 8) | con2;  // Combine the two bytes into a single 16-bit value
  Serial_write(combinedData >> 8);             // Send the first byte (high byte)
  Serial_write(combinedData & 0xFF);           // Send the second byte (low byte)


  uint32_t receivedData = 0;
  if (Serial_available() >= 4) {
    receivedData = Serial_read() << 24;      // Read the first byte (high byte)
    receivedData |= Serial_read() << 16;     // Read the second byte
    receivedData |= Serial_read() << 8;      // Read the third byte
    receivedData |= Serial_read();           // Read the fourth byte (low byte)
  }

  uint32_t combinedData = (con1 << 24) | (con2 << 16) | receivedData; // Combine the received data with own data
  Serial_write(combinedData >> 24);                                   // Send the first byte (high byte)
  Serial_write((combinedData >> 16) & 0xFF);                          // Send the second byte
  Serial_write((combinedData >> 8) & 0xFF);                           // Send the third byte
  Serial_write(combinedData & 0xFF);                                  // Send the fourth byte (low byte)
  delay(1000);                                                         // Add a delay to avoid flooding the next board
}
*/


/*
uint32_t receivedData = 0;
 uint32_t combinedData = (receivedData << 16) | (con1 << 8) | con2; // Combine the received data with the board's own data

  if (Serial_available() >= 4) { // Check if at least 4 bytes are available
    receivedData = Serial_read() << 24; // Read the first byte (highest byte)
    receivedData |= Serial_read() << 16; // Read the second byte
    receivedData |= Serial_read() << 8; // Read the third byte
    receivedData |= Serial_read(); // Read the fourth byte (lowest byte)
  }

  Serial_write(combinedData >> 24); // Send the first byte (highest byte)
  Serial_write((combinedData >> 16) & 0xFF); // Send the second byte
  Serial_write((combinedData >> 8) & 0xFF); // Send the third byte
  Serial_write(combinedData & 0xFF); // Send the fourth byte (lowest byte)
  delay(1000); // Add a delay to avoid flooding the next board
}


*/