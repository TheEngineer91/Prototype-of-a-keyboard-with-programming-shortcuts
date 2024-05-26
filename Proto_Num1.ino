
// Include the Keypad library
#include <Keypad.h>
// Include keyboard
#include "Keyboard.h"
// Include display librariy and I2C library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Display
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};



// Constants for row and column sizes
const byte ROWS = 3;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
  
};

byte rowPins[ROWS] = {10, 9, 8};
byte colPins[COLS] = {7, 6, 5};

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Rotary RotaryEncoder Pins
#define ENC_A 1
#define ENC_B 2
#define ENCODER_BUTTON 12

// Variables
unsigned long _lastIncReadTime = micros(); 
unsigned long _lastDecReadTime = micros(); 
int _pauseLength = 25000;

volatile int counter = 1;
int Last_counter = 1;

volatile int buttonCounter = 0;
volatile long lastMillis = 0;
volatile bool buttonState = LOW;
volatile bool lastButtonState = LOW;

int menu_state = 0;
int LastMenu_State = 0;

char customKey = '0';


// Function prototypes
void RotaryEncoder();
void EncoderButton();



void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
 
 
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // text display
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  
  display.display();
  delay(2000);
  display.clearDisplay();

  // Set RotaryEncoder pins as inputs
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  // Set button pin as input with internal pull-up resistor
  pinMode(ENCODER_BUTTON, INPUT_PULLUP);

  // Selects keyboard
  Keyboard.begin(KeyboardLayout_sv_SE);


  // Start menu
  MENU_DISP();
}

void loop() {
  // Your main code here

  RotaryEncoder();
  EncoderButton();
  
  if (menu_state == 1){   // Can later add more criterias and more if cases to add more languages
    
    // Get key value if pressed
    customKey = customKeypad.getKey();
 
    if (customKey) {


      // Get key value form funktion
      switch (counter) {
        case 1:
        ARDU_1();
        break;
        case 2:
        ARDU_2();
        break;
        case 3:
        ARDU_3();
        break;
      }
    
    }
  }

}

void RotaryEncoder() {
  // Updates counter
  // if they are valid and have rotated a full indent
 
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // RotaryEncoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table
  old_AB <<=2;  // Remember previous state
  if (digitalRead(ENC_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(ENC_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];
  
  // Update counter if RotaryEncoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) {        // Four steps forward
    int changevalue = 1;
    
    _lastIncReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }
  else if( encval < -3 ) {        // Four steps backward
    int changevalue = -1;
    
    _lastDecReadTime = micros();
    counter = counter + changevalue;              // Update counter
    encval = 0;
  }

  // Limits
  if (counter < 1){
    counter = 3;
  }
  else if (counter > 3){
    counter = 1;
  }

  // Display change
  if (Last_counter != counter){

    if (menu_state == 1){
      if (counter == 1){
        ARDISP_1();
        Last_counter = counter;
      }
      else if (counter == 2){
        ARDISP_2();
        Last_counter = counter;
      }
      else if (counter == 3){
        ARDISP_3();
        Last_counter = counter;
      }

    }
    
  }
  
  //Serial.print("RotaryEncoderCounter: ");
  //Serial.println(counter);
}

void EncoderButton() {
  
  // Read the state of the button
  buttonState = digitalRead(ENCODER_BUTTON);

  // Check if the button state has changed
  if (buttonState != lastButtonState) {

    if (buttonState == LOW) {
      if (menu_state == 0){
        menu_state = 1;
      }
      else if (menu_state == 1){
        menu_state = 0;
      }
    }

    // Update last button state
    lastButtonState = buttonState;

    // Debounce delay to avoid multiple readings during button press
    delay(10);
  }

  // Changing menue
  if (LastMenu_State != menu_state){

    if (menu_state == 0){
      MENU_DISP();
      LastMenu_State = menu_state;
    }
    else if (menu_state == 1){
      counter = 1;
      ARDISP_1();
      LastMenu_State = menu_state;
    }
  }
  //Serial.print("menu state: ");
  //Serial.println(menu_state);
}

void ARDU_1(){

  if (customKey == '1'){                                                     // if
    Keyboard.println("if (condition) {"); 
    Keyboard.println("//statement(s)");
  }
  if (customKey == '2'){                                                     // else if
    Keyboard.println("else if (condition) {"); 
    Keyboard.println("//statement(s)");
  }
  if (customKey == '3'){
    Keyboard.println("else {");                                             // else
    Keyboard.println("//statement(s)");
  }
  if (customKey == '4'){                                                    // switch
    Keyboard.println("switch (var) {"); 
    Keyboard.println("case 1:");
    Keyboard.println("// do something when var equals case");
    Keyboard.println("break;");
  }
  if (customKey == '5'){                                                    // case
    Keyboard.println("case /*num*/:"); 
    Keyboard.println("// do something when var equals case");
    Keyboard.println("break;");
  }
  if (customKey == '6'){
    Keyboard.println("while (condition) {");                                // while
    Keyboard.println("//statement(s)");
  }
  if (customKey == '7'){                                                    // for
    Keyboard.println("for (initialization; condition; increment) {"); 
    Keyboard.println("//statement(s)");
  }
  if (customKey == '8'){                                                   // function
    Keyboard.println("void name() {"); 
    Keyboard.println("// code");
  }
  if (customKey == '9'){                                                  // array
    Keyboard.println("/*int or char*/ arrayName[] = {};"); 
    
  }
  
}

void ARDU_2(){

  if (customKey == '1'){                                                  // input
    Keyboard.println("INPUT"); 
  }
  if (customKey == '2'){                                                  // in pullup
    Keyboard.println("INPUT_PULLUP");
  }
  if (customKey == '3'){                                                  // output
    Keyboard.println("OUTPUT");
  }
  if (customKey == '4'){                                                  // high
    Keyboard.println("HIGH"); 
  }
  if (customKey == '5'){                                                  // include
    Keyboard.println("#include"); 
  }
  if (customKey == '6'){                                                  // interrupt
    Keyboard.println("attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);"); 
  }
  if (customKey == '7'){                                                  // low
    Keyboard.println("LOW"); 
  }
  if (customKey == '8'){                                                  // define
    Keyboard.println("#define"); 
  }
  if (customKey == '9'){                                                  // exit interrupt
    Keyboard.println("detachInterrupt(digitalPinToInterrupt(pin));"); 
  }

}

void ARDU_3(){

  if (customKey == '1'){                                                  // pinMode
    Keyboard.println("pinMode(pin, mode);"); 
  }
  if (customKey == '2'){                                                  // digitalWrite
    Keyboard.println("digitalWrite(pin, value);");
  }
  if (customKey == '3'){                                                  // digitalRead
    Keyboard.println("digitalRead(pin);");
  }
  if (customKey == '4'){                                                  // analogRead
    Keyboard.println("analogRead(pin);"); 
  }
  if (customKey == '5'){                                                  // analogWrite
    Keyboard.println("analogWrite(pin, value);"); 
  }
  if (customKey == '6'){                                                  // map
    Keyboard.println("map(value, fromLow, fromHigh, toLow, toHigh);"); 
  }
  if (customKey == '7'){                                                  // delay
    Keyboard.println("delay(ms);"); 
  }
  if (customKey == '8'){                                                  // delay us
    Keyboard.println("delayMicroseconds(us);"); 
  }
  

}


void MENU_DISP(){

  
  display.clearDisplay();

  display.display();
  display.setCursor(5,10);
  display.print("[ Arduino C++ ]");

  display.display();
  display.setCursor(5,25);
  display.print("Later update");

  display.display();
  display.setCursor(5,40);
  display.print("Later update");

  display.display();
  

}

void ARDISP_1(){

  //display.display();


  display.clearDisplay();

  display.display();
  display.setCursor(5,5);
  display.print("if");

  display.display();
  display.setCursor(45,5);
  display.print("el if");

  display.display();
  display.setCursor(95,5);
  display.print("else");

  display.display();
  display.setCursor(5,28);
  display.print("swit.");

  display.display();
  display.setCursor(45,28);
  display.print("case");

  display.display();
  display.setCursor(95,28);
  display.print("while");

  display.display();
  display.setCursor(5,50);
  display.print("for");

  display.display();
  display.setCursor(45,50);
  display.print("func.");

  display.display();
  display.setCursor(95,50);
  display.print("array");

  display.display();

}

void ARDISP_2(){

  //display.display();


  display.clearDisplay();

  display.display();
  display.setCursor(5,5);
  display.print("input");

  display.display();
  display.setCursor(45,5);
  display.print("inpul.");

  display.display();
  display.setCursor(95,5);
  display.print("outpu");

  display.display();
  display.setCursor(5,28);
  display.print("high");

  display.display();
  display.setCursor(45,28);
  display.print("incl.");

  display.display();
  display.setCursor(95,28);
  display.print("inter");

  display.display();
  display.setCursor(5,50);
  display.print("low");

  display.display();
  display.setCursor(45,50);
  display.print("defi.");

  display.display();
  display.setCursor(95,50);
  display.print("exint");

  display.display();

}

void ARDISP_3(){

  //display.display();


  display.clearDisplay();

  display.display();
  display.setCursor(5,5);
  display.print("pinmo");

  display.display();
  display.setCursor(45,5);
  display.print("dwri.");

  display.display();
  display.setCursor(95,5);
  display.print("drea.");

  display.display();
  display.setCursor(5,28);
  display.print("area.");

  display.display();
  display.setCursor(45,28);
  display.print("awri.");

  display.display();
  display.setCursor(95,28);
  display.print("map");

  display.display();
  display.setCursor(5,50);
  display.print("delay");

  display.display();
  display.setCursor(45,50);
  display.print("delus");

  display.display();

}
