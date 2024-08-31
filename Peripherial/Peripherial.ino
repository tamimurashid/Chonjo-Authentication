#include <Keypad.h>
// Keypad setup
const byte ROWS = 4;  // four rows
const byte COLS = 4;  // four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  //Serial.print("hi");
   char key = keypad.getKey();  // Get the pressed key

  if (key) {
    Serial.println(key);  // Print the pressed key to the Serial Monitor

  }
} 


