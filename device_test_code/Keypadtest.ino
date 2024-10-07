#include <Keypad_I2C.h>
#include <Keypad.h>        // GDY120705
#include <Wire.h>
#define I2CADDR 0x26// change address 
// Keypad setup
//--------------------------------------------------------------------------------
const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
//--------------------------------------------------------------------------------
         /* Keypad setup pins declaration  */
//--------------------------------------------------------------------------------
byte rowPins[ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad

// byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
// byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad


// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//initialize an instance of class NewKeypad
Keypad_I2C newKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR); 

void setup() {
  newKeypad.begin();
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  //Serial.print("hi");
   char key = newKeypad.getKey();  // Get the pressed key

  if (key) {
    Serial.println(key);  // Print the pressed key to the Serial Monitor

  }
} 


