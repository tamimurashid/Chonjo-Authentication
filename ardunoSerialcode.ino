#include <Keypad.h>
#include <SoftwareSerial.h>

// Define the serial pins for communication with NodeMCU
SoftwareSerial mySerial(10, 11); // RX, TX

// Define the rows and columns of the keypad
const byte ROWS = 4;
const byte COLS = 4;

// Define the symbols on the buttons of the keypad
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Connect the keypad rows and columns to the corresponding pins
byte rowPins[ROWS] = {12, 11, 10, 9}; // Connect to the row pins
byte colPins[COLS] = {8, 7, 6, 5};    // Connect to the column pins

// Create a Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  mySerial.begin(9600); // Begin serial communication with NodeMCU
}

void loop() {
  // Get the key being pressed
  char key = keypad.getKey();

  if (key) {
    // If a key is pressed, print it to the serial monitor
    Serial.println(key);

    // Send the key press to the NodeMCU
    mySerial.print(key);
  }
}
