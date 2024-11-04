#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Keypad_I2C.h>
#include <Keypad.h>        

#define I2CADDR 0x26
#define trigger 7
#define buzzle 2 

// Pin definition
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(5, 6);
#else
#define mySerial Serial1
#endif
String correctPassword = "1245";

// Object creation and global variables
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
MFRC522 rfid(10, 9);  // RST_PIN, SS_PIN

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROWS = 4;  
const byte COLS = 4;  
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {4, 5, 6, 7}; 
byte colPins[COLS] = {0, 1, 2, 3}; 

Keypad_I2C newKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR); 

String enteredPassword = "";

// Sound and Display classes (same as before)

class Sound{
  public:
    void keyPressTone() {
    tone(buzzle, 500, 100); // Play a 0.5kHz tone for 100ms
    delay(100); // delay for tone 
    noTone(buzzle);
  }

  // Function to play a warning sound
  void warningSound() {
    tone(buzzle, 500, 500); // Play a 500Hz tone for 500ms
    delay(500);
    noTone(buzzle);
  }

  // Function to play a success sound
  void successSound() {
    tone(buzzle, 1000, 300); // Play a 1.5kHz tone for 300ms
    delay(300);
    noTone(buzzle);
    tone(buzzle, 1500, 300); // Play a 2kHz tone for 300ms
    delay(300);
    noTone(buzzle);
  }

};

class Display{
  public:
  void scrollmessage(String title, String message) {
    for (int position = 16; position >= 0; position--) {
      lcd.clear();               // Clear the LCD
      lcd.setCursor(0, 0);       // Set cursor to row 0, column 0
      lcd.print(title);          // Print title in the first row
      lcd.setCursor(position, 1); // Set cursor to row 1 at current position
      lcd.print(message);        // Print message in the second row
      delay(100);                // Adjust delay for scrolling speed
    }
  }


};

class Peripherials : public Sound {
  public:
    void read_card() {
      if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return; // No card present
      }

      // Print UID
      Serial.print("Card UID: ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();
      
      lcd.clear();
      lcd.print("Loading ...."); 
      lcd.setCursor(0, 1);
      lcd.print("wait a moment."); 
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }

    String password_read() {
    enteredPassword = "";
    char key;

    // Gather the password entered by the user
    while (true) {
        key = newKeypad.getKey();
        if (key) {
            keyPressTone(); // Sound when a key is pressed

            if (key == '#') {
                break;  // Enter key pressed
            } else if (key == '*' || key == 'D') {
                // Handle backspace or delete
                if (enteredPassword.length() > 0) {
                    enteredPassword.remove(enteredPassword.length() - 1);
                }
            } else {
                enteredPassword += key; // Append key to password
            }

            // Update the display
            lcd.setCursor(0, 1);
            lcd.print("                ");  // Clear display
            lcd.setCursor(0, 1);
            for (int i = 0; i < enteredPassword.length(); i++) {
                lcd.print('*');  // Display * for each character
            }
        }
    }

    Serial.println(enteredPassword); // For debugging purposes
    return enteredPassword; // Return the entered password
}


    void displayPassword() {
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear display
      lcd.setCursor(0, 1);
      for (int i = 0; i < enteredPassword.length(); i++) {
        lcd.print('*');  // Display * for each character
      }
    }

    // Other methods...
};

String readEspdata() {
    String inputData = "";
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) { // 1-second timeout
        while (Serial.available() > 0) {
            char incomingByte = Serial.read();
            if (incomingByte == '\n') {
                return inputData;
            }
            inputData += incomingByte;
            delay(10); // Adjust if needed for data consistency
        }
    }
    return ""; // Return empty if timeout reached
}


void setup() {
  Display display;
  newKeypad.begin(), mySerial.begin(57600); while(!Serial); Serial.begin(9600), finger.begin(57600), delay(5);
  SPI.begin(), rfid.PCD_Init(), finger.getTemplateCount(), pinMode(trigger, OUTPUT), lcd.init(), lcd.backlight();
  lcd.clear(), display.scrollmessage("Hi there ", "please scan  Id "), delay(100);
}
Peripherials peripherials;

void loop() {
    Sound sound; 
    peripherials.read_card();
    String read_data = readEspdata();

    if (read_data == "#$170@!z") { // Card verified
        lcd.clear();
        lcd.print("Card match!");
        sound.successSound();
        delay(50);

        lcd.clear();
        lcd.print("Enter password");
        String enteredPassword = peripherials.password_read();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Loading ....");
        lcd.setCursor(0, 1);
        lcd.print("wait a moment.");

        // Send password for verification
        Serial.println(enteredPassword); // Send password to ESP
        delay(1000);

        // Wait for response from ESP
        String postPasswordResponse = readEspdata();
        postPasswordResponse.trim();
        // Handle responses based on ESP's message
        if (postPasswordResponse == "a") {
            sound.successSound();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Success");
            lcd.setCursor(0, 1);
            lcd.print("Password OK!");
        } else if (postPasswordResponse == "#$501@!z") {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Failed");
            lcd.setCursor(0, 1);
            lcd.print("Invalid Password");
        } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Error");
            lcd.setCursor(0, 1);
            lcd.print("Unknown Response");
        }
    }
}
