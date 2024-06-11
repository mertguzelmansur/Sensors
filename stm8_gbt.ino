#include "STM8S.h"

#define START_BYTE 0x55   // Start byte for data flow
#define END_BYTE 0x23     // End byte for data flow
#define TEST_BYTE 0x01    // Byte for testing Rx and Tx connections

// Define input pins with descriptive names
#define SEAT_1 0
#define BELT_1 1
#define SEAT_2 2
#define BELT_2 3

uint8_t isFirstCard = 0; // Flag to determine if this board is the first card

// Evaluate digital input group and return appropriate code
uint8_t Evaluate_Input_Group(uint8_t pin1, uint8_t pin2) {
    uint8_t value = 0;
    if (digitalRead(pin1)) value |= (1 << 0);
    if (digitalRead(pin2)) value |= (1 << 1);

    switch (value) {
        case 0x00: return '6';
        case 0x01: return '7';
        case 0x02: return '4';
        case 0x03: return '5';
        default: return 0;
    }
}

// Read and evaluate digital inputs, return a single data value
uint16_t Read_And_Evaluate_Digital_Inputs(void) {
    uint8_t group1 = Evaluate_Input_Group(SEAT_1, BELT_1);
    uint8_t group2 = Evaluate_Input_Group(SEAT_2, BELT_2);
    return (group1 << 8) | group2; // Return 16-bit value
}

// Send data over UART
void UART_Send_Data(uint8_t data) {
    Serial_write(data);
}

// Receive data over UART
uint8_t UART_Receive_Data(void) {
    while (!Serial_available()); // Wait until data is received
    return Serial_read();
}

// Test Rx and Tx connections
int Test_Communication(void) {
    UART_Send_Data(TEST_BYTE);
    delay(10000); // Short delay
    if (Serial_available()) {
        uint8_t received_data = Serial_read();
        if (received_data == TEST_BYTE) {
            return 1; // Communication successful
        }
    }
    return 0; // Communication failed
}

void setup() {
    Serial_begin(9600);

    pinMode(SEAT_1, INPUT_PULLUP);
    pinMode(BELT_1, INPUT_PULLUP);
    pinMode(SEAT_2, INPUT_PULLUP);
    pinMode(BELT_2, INPUT_PULLUP);

    // Test communication on startup
    if (!Test_Communication()) {
        isFirstCard = 1; // If communication fails, set this board as the first card
    }
}

void loop() {
    uint8_t received_data;
    uint16_t input_data;
    char combined_data[10]; // Adjust size as needed to fit combined data

    // Read the board's own input data
    input_data = Read_And_Evaluate_Digital_Inputs();
    combined_data[0] = (input_data >> 8) & 0xFF; // Store own high byte
    combined_data[1] = input_data & 0xFF; // Store own low byte
    int index = 2;

    if (!isFirstCard) {
        // Other cards wait for incoming data
        do {
            received_data = UART_Receive_Data();
        } while (received_data != START_BYTE);

        // Read incoming data until END_BYTE
        while (true) {
            received_data = UART_Receive_Data();
            if (received_data == END_BYTE) break;
            combined_data[index++] = received_data;
        }
    }

    // Send combined data
    UART_Send_Data(START_BYTE); // Send start byte
    for (int i = 0; i < index; i++) {
        UART_Send_Data(combined_data[i]);
    }
    UART_Send_Data(END_BYTE); // Send end byte

    UART_Send_Data('\n'); // New line character

    delay(1000);
}
