#include <HardwareSerial.h>
#include <ESP_8_BIT_GFX.h>
#include <Button.h>



#include "logo.c"
#include "seatbelt.c"
#include "bus.c"


#define START_BYTE 0x4D
#define END_BYTE 0x23
#define TIMEOUT_MS 5000 // Timeout for receiving data from each STM8 board

#define seatemptycolor 0x49
#define seatfullcolor 0x03
#define beltcolor 0xE0
#define warningcolor 0xFC


int seatNumber;
char seatStatuses[16]; // Store statuses for each seat

String busname;

int bustype = 1;

int select_id = 1;

Button button1(35);

int Servicemode = LOW;       // Current state of the LED
int servicebutton = LOW; 
int upbutton = LOW; // Current reading from the input pin
int lastservicebutton = LOW; // Previous reading from the input pin

unsigned long pressStartTime = 0;  // The last time the button was pressed
unsigned long longPressDuration = 2000;  // Duration for a long press (2 seconds)

static const unsigned char PROGMEM warning[] = {0x00,0x00,0x01,0x80,0x02,0x40,0x02,0x40,0x04,0x20,0x09,0x90,0x09,0x90,0x11,0x88,0x11,0x88,0x21,0x84,0x40,0x02,0x41,0x82,0x81,0x81,0x80,0x01,0x7f,0xfe,0x00,0x00};
static const unsigned char PROGMEM belt[] = {0x00,0x00,0xc0,0x00,0x01,0xe0,0x00,0x07,0xf0,0x00,0x0f,0xf0,0x00,0x1f,0xe0,0x00,0x7b,0xc0,0x00,0x71,0xc0,0x00,0x7b,0xc0,0x00,0x3f,0x80,0x01,0x9f,0x00,0x07,0xcc,0x00,0x1e,0xe0,0x00,0x3e,0xe0,0x00,0x3f,0xc0,0x00,0x7f,0x80,0x00,0xff,0x80,0x00,0xff,0x00,0x00,0xfe,0x00,0x00,0x70,0x00,0x00,0x00,0x00,0x00};
static const unsigned char PROGMEM seat1[] = {0x00,0xff,0x80,0x01,0xff,0xc0,0x01,0xff,0xc0,0x00,0xff,0x80,0x00,0x00,0x00,0x3f,0xff,0x0c,0x7f,0xff,0x9e,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0xff,0xff,0xde,0x7f,0xff,0x9e,0x3f,0xff,0x0c,0x00,0x00,0x00,0x00,0xff,0x80,0x01,0xff,0xc0,0x01,0xff,0xc0,0x00,0xff,0x80};
static const unsigned char PROGMEM wheel[] = {0x01,0xff,0x80,0x07,0xff,0xe0,0x0f,0xff,0xf0,0x1f,0xff,0xf8,0x3f,0x3c,0xfc,0x7c,0x3c,0x3e,0x78,0x3c,0x1e,0xf8,0x3c,0x1f,0xf0,0x7e,0x0f,0xf0,0xff,0x0f,0xf0,0xff,0xff,0xe0,0xff,0xff,0xe0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0x0f,0xf0,0x7e,0x0f,0xf8,0x3c,0x1f,0x78,0x3c,0x1e,0x7c,0x3c,0x3e,0x3f,0x3c,0xfc,0x1f,0xff,0xf8,0x0f,0xff,0xf0,0x07,0xff,0xe0,0x01,0xff,0x80};

static const unsigned char PROGMEM bisa[] = {0x7f,0xe0,0x07,0x80,0x00,0x00,0x00,0x00,0xff,0xfc,0x0f,0x80,0x00,0x00,0x00,0x00,0xff,0xfe,0x0f,0x80,0x00,0x00,0x00,0x00,0xff,0xff,0x07,0x00,0x00,0x00,0x00,0x00,0xf0,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x07,0x87,0x00,0xfc,0x00,0xff,0x00,0xf0,0x07,0x87,0x03,0xff,0x01,0xff,0x80,0xf0,0x07,0x07,0x07,0xff,0x83,0xff,0xc0,0xf0,0x0f,0x07,0x07,0x87,0x87,0xc3,0xe0,0xf0,0x7e,0x07,0x0f,0x03,0xc7,0x81,0xe0,0xff,0xfc,0x07,0x0f,0x00,0x01,0x01,0xe0,0xff,0xfe,0x07,0x0f,0x00,0x00,0x00,0xe0,0xff,0xff,0x07,0x07,0xe0,0x00,0x1f,0xe0,0xf0,0x0f,0x87,0x07,0xfe,0x01,0xff,0xe0,0xf0,0x07,0x87,0x03,0xff,0x03,0xff,0xe0,0xf0,0x03,0x87,0x00,0x7f,0x87,0xf8,0xe0,0xf0,0x03,0x87,0x00,0x07,0xcf,0x80,0xe0,0xf0,0x03,0x87,0x00,0x03,0xcf,0x01,0xe0,0xf0,0x07,0x87,0x0e,0x03,0xcf,0x01,0xe0,0xf0,0x0f,0x87,0x0f,0x03,0xcf,0x03,0xe0,0xff,0xff,0x07,0x0f,0xdf,0x87,0xcf,0xe0,0xff,0xff,0x07,0x07,0xff,0x87,0xff,0xe0,0xff,0xfc,0x07,0x03,0xff,0x03,0xfe,0xe0,0x00,0x00,0x00,0x00,0xfc,0x00,0xf8,0x00};
static const unsigned char PROGMEM otomotiv[] = {0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x07,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x0f,0xff,0xc0,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x3e,0x00,0x00,0x00,0x1f,0xef,0xe0,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x1c,0x00,0x00,0x00,0x1f,0x01,0xf0,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x3c,0x00,0xf0,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x7c,0x00,0x78,0xff,0xc0,0x7f,0x00,0xe7,0xe1,0xf8,0x00,0xfe,0x07,0xfe,0x3c,0x78,0x07,0x80,0x78,0x00,0x78,0xff,0xc1,0xff,0x80,0xef,0xf7,0xfc,0x03,0xff,0x07,0xfe,0x3c,0x78,0x07,0x80,0x78,0x00,0x3c,0xff,0xc3,0xff,0xe0,0xff,0xff,0xfe,0x07,0xff,0x87,0xfe,0x3c,0x78,0x07,0x80,0x70,0x00,0x3c,0x1c,0x07,0xe3,0xe0,0xfc,0xff,0x1e,0x0f,0xc7,0xc0,0xf0,0x3c,0x3c,0x0f,0x00,0x70,0x00,0x3c,0x1c,0x07,0x81,0xf0,0xf0,0x3c,0x0f,0x0f,0x03,0xe0,0xf0,0x3c,0x3c,0x0f,0x00,0xf0,0x00,0x3c,0x1c,0x07,0x00,0xf0,0xf0,0x3c,0x0f,0x1e,0x01,0xe0,0xf0,0x3c,0x3c,0x0e,0x00,0xf0,0x00,0x3c,0x1c,0x0f,0x00,0x70,0xf0,0x3c,0x0f,0x1e,0x00,0xe0,0xf0,0x3c,0x1e,0x1e,0x00,0xf0,0x00,0x3c,0x1c,0x0f,0x00,0x78,0xe0,0x3c,0x0f,0x1e,0x00,0xf0,0xf0,0x3c,0x1e,0x1e,0x00,0x70,0x00,0x3c,0x1c,0x0f,0x00,0x78,0xe0,0x3c,0x0f,0x1e,0x00,0xf0,0xf0,0x3c,0x0e,0x1c,0x00,0x70,0x00,0x3c,0x1c,0x0f,0x00,0x78,0xe0,0x3c,0x0f,0x1c,0x00,0xf0,0xf0,0x3c,0x0f,0x3c,0x00,0x78,0x00,0x38,0x1c,0x0f,0x00,0x78,0xe0,0x3c,0x0f,0x1e,0x00,0xf0,0xf0,0x3c,0x0f,0x3c,0x00,0x78,0x00,0x78,0x1c,0x0f,0x00,0x78,0xe0,0x3c,0x0f,0x1e,0x00,0xe0,0xf0,0x3c,0x07,0x38,0x00,0x3c,0x00,0x78,0x1c,0x0f,0x00,0x70,0xe0,0x3c,0x0f,0x1e,0x01,0xe0,0xf0,0x3c,0x07,0xf8,0x00,0x3e,0x00,0xf0,0x1c,0x07,0x80,0xf0,0xe0,0x3c,0x0f,0x0f,0x01,0xe0,0xf0,0x3c,0x07,0xf0,0x00,0x1f,0x03,0xf0,0x1e,0x07,0xc1,0xf0,0xe0,0x3c,0x0f,0x0f,0x03,0xe0,0xf0,0x3c,0x03,0xf0,0x00,0x1f,0xff,0xe0,0x1f,0xc3,0xf7,0xe0,0xe0,0x3c,0x0f,0x07,0xff,0xc0,0xfe,0x3c,0x03,0xf0,0x00,0x07,0xff,0xc0,0x1f,0xc3,0xff,0xc0,0xe0,0x3c,0x0f,0x07,0xff,0x80,0x7e,0x3c,0x03,0xe0,0x00,0x03,0xff,0x00,0x0f,0xc0,0xff,0x80,0xe0,0x3c,0x0f,0x01,0xff,0x00,0x3f,0x3c,0x01,0xe0,0x00,0x00,0x7c,0x00,0x03,0xc0,0x3e,0x00,0x00,0x00,0x00,0x00,0x7c,0x00,0x0e,0x00,0x00,0x00,0x00};

extern  const unsigned short logo[13640];
extern  const unsigned short seatbelt[504];
extern  const unsigned short bus[52480];


ESP_8_BIT_GFX videoOut(true /* = NTSC */, 8 /* = RGB332 color */);

HardwareSerial MySerial(2); // Use UART2 (GPIO 16, 17)

void selector() {
  if(select_id == -1){
    select_id = 3;  
  }
   if(select_id == 4){
    select_id = 1;
  }
  switch (select_id) {
  case 1:
    bustype = 1;
    busname = "MINIBUS 15 + 1 KOLTUK";
    busmap1();
    break;
  case 2:
    bustype = 2;
    busname = "MINIBUS 17 KOLTUK";
    busmap2();
    break;
  case 3:
    bustype = 3;
    busname = "MINIBUS 22 KOLTUK";
    busmap3();  
    break;
  default:
    // if nothing else matches, do the default
    // default is optional
    break;
 }}


void setup() {
    Serial.begin(115200);
    MySerial.begin(9600, SERIAL_8N1, 16, 17); // Initialize UART2

    button1.begin();

    pinMode(34, INPUT);

    videoOut.begin();
    // Show the logo for 5 seconds
    videoOut.fillScreen(0); // Clear the screen
    

    videoOut.drawBitmap(25, 110, bisa, 59, 25, 0x03);
    videoOut.drawBitmap(91, 110, otomotiv, 137, 25, 0xE0);

    //videoOut.drawRGBBitmap(5, 80, logo, 310, 44); // Adjust x, y, width, height as needed
    //videoOut.drawRGBBitmap(30, 80, seatbelt, 28, 28); // Adjust x, y, width, height as needed
    //videoOut.drawRGBBitmap(5, 5, bus, 320, 164); // Adjust x, y, width, height as needed

    videoOut.waitForFrame(); // Ensure logo is videoOuted
    delay(2000); // Keep the logo on the screen for 5 seconds

    // Clear the screen after 5 seconds
    videoOut.fillScreen(0);
}

void loop() {
    videoOut.waitForFrame();
    videoOut.fillScreen(0xFF);

    static bool dataStarted = false;
    static String receivedData = "";
    static unsigned long lastDataReceivedTime = 0;

    if (MySerial.available()) {
        char receivedChar = MySerial.read();

        if (receivedChar == START_BYTE) {
            dataStarted = true;
            receivedData = ""; // Reset the received data buffer
            lastDataReceivedTime = millis();
        }

        if (dataStarted) {
            receivedData += receivedChar;

            if (receivedChar == END_BYTE) {
                dataStarted = false;
                Serial.println("Received data: " + receivedData);
                parseData(receivedData);
            }
        }
    }

    // Check for timeout
    if (dataStarted && millis() - lastDataReceivedTime > TIMEOUT_MS) {
        dataStarted = false;
        Serial.println("Timeout occurred, assuming end of data from STM8 boards");
        parseData(receivedData); // Parse data received so far
    }

    // Count empty seats
    int emptySeatCount = 0;
    for (int i = 0; i < 8; ++i) {
        if (seatStatuses[i] == '5') {
            emptySeatCount++;
        }
    }

servicebutton = digitalRead(34);

  if (servicebutton != lastservicebutton) {
    if (servicebutton == HIGH) {
      // Button is pressed
      pressStartTime = millis();
    } else {
      // Button is released
      if (millis() - pressStartTime >= longPressDuration) {
        // Long press detected, toggle the LED state
        Servicemode = !Servicemode;
        Serial.println(Servicemode);
      }
    }
    delay(50);  // Debounce delay
  }
  lastservicebutton = servicebutton;

     if (button1.released() && Servicemode == HIGH ){
    Serial.println("Button 2 released");
    select_id++;
    Serial.println(select_id);
    selector(); 
  }

  if (Servicemode == HIGH){

    videoOut.setCursor(60, 224);
    videoOut.setTextColor(0);
    videoOut.print(busname);
    }


    // videoOut the count of empty seats
    videoOut.setCursor(130, 20);
    videoOut.setTextSize(1);
    videoOut.setTextColor(0);
    videoOut.print("YOLCU SAYISI: ");
    videoOut.print(emptySeatCount);

    videoOut.fillRoundRect(82, 113, 80, 22, 8, 0xE0);
    videoOut.setTextSize(1);
    videoOut.setTextColor(0xFF);
    videoOut.setCursor(94, 121);
    videoOut.print("Yolcu Var!");

  switch (bustype) {
  case 0:
    break;
  case 1:
    busmap1();
    break;
  case 2:
    busmap2();
    break;
  case 3:
  busmap3();
  break;
  default:
    break;
 }
}

void busmap1(){

        videoOut.drawBitmap(8, 188, wheel, 24, 24, seatemptycolor);
//  seat 1
    if (seatStatuses[0] == '5'|| seatStatuses[0] == '7') {
        videoOut.drawBitmap(45, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[0] == '6' || seatStatuses[0] == '4') {
        videoOut.drawBitmap(45, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[0] == '6' || seatStatuses[0] == '7') {
        videoOut.drawBitmap(45, 191, belt, 20, 20, beltcolor); // belt 1
      } 
    else {
        videoOut.drawBitmap(45, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(46, 192, warning, 16, 16, warningcolor);
    } 
//  seat 2
    if (seatStatuses[1] == '5' || seatStatuses[1] == '7') {
        videoOut.drawBitmap(45, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[1] == '6' || seatStatuses[1] == '4') {
        videoOut.drawBitmap(45, 143, seat1, 23, 34, 0x08); 
    }
    if (seatStatuses[1] == '6' || seatStatuses[1] == '7') {
        videoOut.drawBitmap(45, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(45, 143, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(46, 152, warning, 16, 16, warningcolor);
    } 
//  seat 3 
    if (seatStatuses[2] == '5' || seatStatuses[2] == '7') {
        videoOut.drawBitmap(84, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[2] == '6' || seatStatuses[2] == '4') {
        videoOut.drawBitmap(84, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[2] == '6' || seatStatuses[2] == '7') {
        videoOut.drawBitmap(84, 191, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(84, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(86, 192, warning, 16, 16, warningcolor);
    } 

//  seat 4
    if (seatStatuses[3] == '5' || seatStatuses[3] == '7') {
        videoOut.drawBitmap(84, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[3] == '6' || seatStatuses[3] == '4') {
        videoOut.drawBitmap(84, 143, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[3] == '6' || seatStatuses[3] == '7') {
        videoOut.drawBitmap(84, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(84, 143, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(86, 152, warning, 16, 16, warningcolor);
    } 

//  seat 5
    if (seatStatuses[4] == '5' || seatStatuses[4] == '7') {
        videoOut.drawBitmap(123, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[4] == '6' || seatStatuses[4] == '4') {
        videoOut.drawBitmap(123, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[4] == '6' || seatStatuses[4] == '7') {
        videoOut.drawBitmap(123, 191, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(123, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(124, 192, warning, 16, 16, warningcolor);
    } 

//  seat 6
    if (seatStatuses[5] == '5' || seatStatuses[5] == '7') {
        videoOut.drawBitmap(123, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[5] == '6' || seatStatuses[5] == '4') {
        videoOut.drawBitmap(123, 143, seat1, 23, 34, 0x08); 
    }
    if (seatStatuses[5] == '6' || seatStatuses[5] == '7') {
        videoOut.drawBitmap(123, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(123, 143, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(124, 151, warning, 16, 16, warningcolor);
    } 

//  seat 7
    if (seatStatuses[6] == '5' || seatStatuses[6] == '7') {
        videoOut.drawBitmap(162, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[6] == '6' || seatStatuses[6] == '4') {
        videoOut.drawBitmap(162, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[6] == '6' || seatStatuses[6] == '7') {
        videoOut.drawBitmap(167, 123, belt, 24, 24, beltcolor); 
    }
        else {
        videoOut.drawBitmap(162, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(162, 191, warning, 16, 16, warningcolor);
    } 

//  seat 8
    if (seatStatuses[7] == '5' || seatStatuses[7] == '7') {
        videoOut.drawBitmap(162, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[7] == '6' || seatStatuses[7] == '4') {
        videoOut.drawBitmap(162, 143, seat1, 23, 34, seatfullcolor); 
        
    }
    if (seatStatuses[7] == '6' || seatStatuses[7] == '7') {
        videoOut.drawBitmap(162, 151, belt, 20, 20, beltcolor);
    }
        else {
        videoOut.drawBitmap(162, 143, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(163, 152, warning, 16, 16, warningcolor);
    } 

//  seat 9
    if (seatStatuses[8] == '5' || seatStatuses[8] == '7') {
        videoOut.drawBitmap(201, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[8] == '6' || seatStatuses[8] == '4') {
        videoOut.drawBitmap(201, 183, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[8] == '6' || seatStatuses[8] == '7') {
        videoOut.drawBitmap(201, 191, belt, 20, 20, beltcolor);
    }
        else {
        videoOut.drawBitmap(201, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(202, 192, warning, 16, 16, warningcolor);
    } 

//  seat 10
    if (seatStatuses[9] == '5' || seatStatuses[9] == '7') {
        videoOut.drawBitmap(201, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(201, 143, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[9] == '6' || seatStatuses[9] == '7') {
        videoOut.drawBitmap(201, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(201, 143, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(202, 152, warning, 16, 16, warningcolor);
    } 

//  seat 11
    if (seatStatuses[10] == '5' || seatStatuses[10] == '7') {
        videoOut.drawBitmap(201, 72, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(201, 72, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[10] == '6' || seatStatuses[10] == '7') {
        videoOut.drawBitmap(201, 79, belt, 20, 20, beltcolor);
    }
        else {
       videoOut.drawBitmap(201, 72, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(202, 81, warning, 16, 16, warningcolor);
    } 

//  seat 12
    if (seatStatuses[11] == '5' || seatStatuses[11] == '7') {
        videoOut.drawBitmap(201, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(201, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[11] == '6' || seatStatuses[11] == '7') {
        videoOut.drawBitmap(201, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(201, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(202, 40, warning, 16, 16, warningcolor);
    } 

//  seat 13
    if (seatStatuses[12] == '5' || seatStatuses[12] == '7') {
        videoOut.drawBitmap(162, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(162, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[12] == '6' || seatStatuses[12] == '7') {
        videoOut.drawBitmap(162, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(162, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(164, 40, warning, 16, 16, warningcolor);
    } 

//  seat 14
    if (seatStatuses[13] == '5' || seatStatuses[13] == '7') {
        videoOut.drawBitmap(123, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(123, 32, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[13] == '6' || seatStatuses[13] == '7') {
        videoOut.drawBitmap(123, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(123, 32, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(124, 40, warning, 16, 16, warningcolor);
    } 

//  seat 15
    if (seatStatuses[14] == '5' || seatStatuses[14] == '7') {
        videoOut.drawBitmap(84, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(84, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[14] == '6' || seatStatuses[14] == '7') {
        videoOut.drawBitmap(84, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(84, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(86, 40, warning, 16, 16, warningcolor);
    } 

//  seat 16
    if (seatStatuses[15] == '5' || seatStatuses[15] == '7') {
       videoOut.drawBitmap(8, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(8, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[15] == '6' || seatStatuses[15] == '7') {
        videoOut.drawBitmap(8, 40, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(8, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(9, 40, warning, 16, 16, warningcolor);
    } 

}

// BUSMAP 2

void busmap2(){

        videoOut.drawBitmap(8, 188, wheel, 24, 24, seatemptycolor);
//  seat 1
    if (seatStatuses[0] == '5'|| seatStatuses[0] == '7') {
        videoOut.drawBitmap(78, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[0] == '6' || seatStatuses[0] == '4') {
        videoOut.drawBitmap(78, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[0] == '6' || seatStatuses[0] == '7') {
        videoOut.drawBitmap(78, 191, belt, 20, 20, beltcolor); // belt 1
      } 
    else {
        videoOut.drawBitmap(78, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(79, 192, warning, 16, 16, warningcolor);
    } 
//  seat 2
    if (seatStatuses[1] == '5' || seatStatuses[1] == '7') {
        videoOut.drawBitmap(78, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[1] == '6' || seatStatuses[1] == '4') {
        videoOut.drawBitmap(78, 143, seat1, 23, 34, 0x08); 
    }
    if (seatStatuses[1] == '6' || seatStatuses[1] == '7') {
        videoOut.drawBitmap(78, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(78, 143, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(79, 152, warning, 16, 16, warningcolor);
    } 
//  seat 3 
    if (seatStatuses[2] == '5' || seatStatuses[2] == '7') {
        videoOut.drawBitmap(109, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[2] == '6' || seatStatuses[2] == '4') {
        videoOut.drawBitmap(109, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[2] == '6' || seatStatuses[2] == '7') {
        videoOut.drawBitmap(109, 191, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(109, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(111, 192, warning, 16, 16, warningcolor);
    } 

//  seat 4
    if (seatStatuses[3] == '5' || seatStatuses[3] == '7') {
        videoOut.drawBitmap(109, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[3] == '6' || seatStatuses[3] == '4') {
        videoOut.drawBitmap(109, 143, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[3] == '6' || seatStatuses[3] == '7') {
        videoOut.drawBitmap(109, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(109, 143, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(109, 151, warning, 16, 16, warningcolor);
    } 

//  seat 5
    if (seatStatuses[4] == '5' || seatStatuses[4] == '7') {
        videoOut.drawBitmap(140, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[4] == '6' || seatStatuses[4] == '4') {
        videoOut.drawBitmap(140, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[4] == '6' || seatStatuses[4] == '7') {
        videoOut.drawBitmap(140, 191, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(140, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(141, 192, warning, 16, 16, warningcolor);
    } 

//  seat 6
    if (seatStatuses[5] == '5' || seatStatuses[5] == '7') {
        videoOut.drawBitmap(140, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[5] == '6' || seatStatuses[5] == '4') {
        videoOut.drawBitmap(140, 143, seat1, 23, 34, 0x08); 
    }
    if (seatStatuses[5] == '6' || seatStatuses[5] == '7') {
        videoOut.drawBitmap(140, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(140, 143, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(141, 151, warning, 16, 16, warningcolor);
    } 

//  seat 7
    if (seatStatuses[6] == '5' || seatStatuses[6] == '7') {
        videoOut.drawBitmap(171, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[6] == '6' || seatStatuses[6] == '4') {
        videoOut.drawBitmap(171, 183, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[6] == '6' || seatStatuses[6] == '7') {
        videoOut.drawBitmap(171, 191, belt, 24, 24, beltcolor); 
    }
        else {
        videoOut.drawBitmap(171, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(172, 192, warning, 16, 16, warningcolor);
    } 

//  seat 8
    if (seatStatuses[7] == '5' || seatStatuses[7] == '7') {
        videoOut.drawBitmap(171, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[7] == '6' || seatStatuses[7] == '4') {
        videoOut.drawBitmap(171, 143, seat1, 23, 34, seatfullcolor); 
        
    }
    if (seatStatuses[7] == '6' || seatStatuses[7] == '7') {
        videoOut.drawBitmap(171, 151, belt, 20, 20, beltcolor);
    }
        else {
        videoOut.drawBitmap(171, 143, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(172, 152, warning, 16, 16, warningcolor);
    } 

//  seat 9
    if (seatStatuses[8] == '5' || seatStatuses[8] == '7') {
        videoOut.drawBitmap(202, 183, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[8] == '6' || seatStatuses[8] == '4') {
        videoOut.drawBitmap(202, 183, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[8] == '6' || seatStatuses[8] == '7') {
        videoOut.drawBitmap(202, 191, belt, 20, 20, beltcolor);
    }
        else {
        videoOut.drawBitmap(202, 183, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(203, 192, warning, 16, 16, warningcolor);
    } 

//  seat 10
    if (seatStatuses[9] == '5' || seatStatuses[9] == '7') {
        videoOut.drawBitmap(202, 143, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(202, 143, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[9] == '6' || seatStatuses[9] == '7') {
        videoOut.drawBitmap(202, 151, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(202, 143, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(203, 152, warning, 16, 16, warningcolor);
    } 

//  seat 11
    if (seatStatuses[10] == '5' || seatStatuses[10] == '7') {
        videoOut.drawBitmap(202, 72, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(202, 72, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[10] == '6' || seatStatuses[10] == '7') {
        videoOut.drawBitmap(202, 79, belt, 20, 20, beltcolor);
    }
        else {
       videoOut.drawBitmap(202, 72, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(203, 81, warning, 16, 16, warningcolor);
    } 

//  seat 12
    if (seatStatuses[11] == '5' || seatStatuses[11] == '7') {
        videoOut.drawBitmap(202, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(202, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[11] == '6' || seatStatuses[11] == '7') {
        videoOut.drawBitmap(202, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(202, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(202, 39, warning, 16, 16, warningcolor);
    } 

//  seat 13
    if (seatStatuses[12] == '5' || seatStatuses[12] == '7') {
        videoOut.drawBitmap(171, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(171, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[12] == '6' || seatStatuses[12] == '7') {
        videoOut.drawBitmap(171, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(171, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(173, 40, warning, 16, 16, warningcolor);
    } 

//  seat 14
    if (seatStatuses[13] == '5' || seatStatuses[13] == '7') {
        videoOut.drawBitmap(140, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(140, 32, seat1, 23, 34, seatfullcolor); 
    }
    if (seatStatuses[13] == '6' || seatStatuses[13] == '7') {
        videoOut.drawBitmap(140, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(140, 32, seat1, 23, 34, seatemptycolor); 
        videoOut.drawBitmap(141, 40, warning, 16, 16, warningcolor);
    } 

//  seat 15
    if (seatStatuses[14] == '5' || seatStatuses[14] == '7') {
        videoOut.drawBitmap(109, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(109, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[14] == '6' || seatStatuses[14] == '7') {
        videoOut.drawBitmap(109, 39, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(109, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(111, 40, warning, 16, 16, warningcolor);
    } 

//  seat 16
    if (seatStatuses[15] == '5' || seatStatuses[15] == '7') {
       videoOut.drawBitmap(78, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(78, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[15] == '6' || seatStatuses[15] == '7') {
        videoOut.drawBitmap(78, 32, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(78, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(79, 40, warning, 16, 16, warningcolor);
    } 

//  seat 17
    if (seatStatuses[15] == '5' || seatStatuses[15] == '7') {
       videoOut.drawBitmap(47, 32, seat1, 23, 34, seatemptycolor); 
    } else if (seatStatuses[9] == '6' || seatStatuses[9] == '4') {
        videoOut.drawBitmap(47, 32, seat1, 23, 34, seatfullcolor);
    }
    if (seatStatuses[15] == '6' || seatStatuses[15] == '7') {
        videoOut.drawBitmap(47, 40, belt, 20, 20, beltcolor); 
    }
        else {
        videoOut.drawBitmap(47, 32, seat1, 23, 34, seatemptycolor);
        videoOut.drawBitmap(48, 40, warning, 16, 16, warningcolor);
    }
}

// BUSMAP 3

void busmap3(){
    videoOut.fillScreen(0xFF); // Clear the screen
    videoOut.setTextSize(2);
    videoOut.setCursor(67, 113);
    videoOut.setTextColor(0X03);
    videoOut.print("BOS SAYFA");

}
//  //

void parseData(String data) {
    // Remove start and end bytes from the data string
    if (data.length() < 2 || data.charAt(0) != START_BYTE || data.charAt(data.length() - 1) != END_BYTE) {
        Serial.println("Invalid data format");
        return;
    }

    data = data.substring(1, data.length() - 1);

    int boardCount = data.length() / 2; // Each board contributes 2 bytes
    Serial.print("Number of STM8 boards connected: ");
    Serial.println(boardCount);

    for (int i = 0; i < boardCount; i++) {
        seatStatuses[i * 2] = data.charAt(i * 2);
        seatStatuses[i * 2 + 1] = data.charAt(i * 2 + 1);
        Serial.print("Board ");
        Serial.print(i + 1);
        Serial.print(" - Seat ");
        Serial.print(i * 2 + 1);
        Serial.print(" status: ");
        Serial.print(seatStatuses[i * 2]); // Seat 1
        Serial.print(", Seat ");
        Serial.print(i * 2 + 2);
        Serial.print(" status: ");
        Serial.println(seatStatuses[i * 2 + 1]); // Seat 2
    }
}
