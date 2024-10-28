#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Keypad_I2C.h>
#include <Keypad.h>        
#include <Wire.h>
#define I2CADDR 0x26 



//--------------------------------------------------------------------------------
         /* Pin definition   */
//---------------------------------------------------------------------------------
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(5, 6);
#else
#define mySerial Serial1
#endif
#define relay 7
#define buzzle 2
//--------------------------------------------------------------------------------
         /*  object creation area  and decralation of global variables */
//---------------------------------------------------------------------------------
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t getFingerprintID();
int getFingerprintIDez();


//--------------------------------------------------------------------------------
         /* Pin definitions for Arduino Uno */
//---------------------------------------------------------------------------------
constexpr uint8_t RST_PIN = 9;  // RST pin for RFID
constexpr uint8_t SS_PIN = 10;  // SDA (SS) pin for RFID

MFRC522 rfid(SS_PIN, RST_PIN);

// LCD I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);
//--------------------------------------------------------------------------------
         /* Keypad setup */
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

//initialize an instance of class NewKeypad
Keypad_I2C newKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR); 


uint8_t id;
String tag;
String enteredPassword = "";
//--------------------------------------------------------------------------------
         /* sound functions  */
//--------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------
         /* Authentication function for rfid reader to scan and read id  */
//--------------------------------------------------------------------------------

// String readRFID() {
//   // Check if a new card is present
//   if (!mfrc522.PICC_IsNewCardPresent()) {
//     return "";  // No card detected, return empty string
//   }

//   // Check if the card can be read
//   if (!mfrc522.PICC_ReadCardSerial()) {
//     return "";  // Card reading failed, return empty string
//   }

//   // Initialize an empty string to store card ID
//   String cardID = "";

//   // Read the card's unique ID
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     // Append each byte in hexadecimal format to cardID
//     cardID += String(mfrc522.uid.uidByte[i], HEX);
//   }

//   // Convert to uppercase for consistent formatting
//   cardID.toUpperCase();

//   // Halt PICC (card) to stop further reading until the card is removed
//   mfrc522.PICC_HaltA();

//   return cardID;  // Return the card ID as a string
// }

uint8_t rfidAuthentication() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return 0; // No new card present
  }
  if (rfid.PICC_ReadCardSerial()) {
    tag = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tag += String(rfid.uid.uidByte[i], HEX);  // Ensure consistent formatting
    }
    
    Serial.print("RFID Tag: ");
    Serial.println(tag);// this will send the id to the esp32 for more computation   
    lcd.clear();
    lcd.print("RFID Tag:");
    lcd.setCursor(0, 1);
    lcd.print(tag);
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}





//--------------------------------------------------------------------------------
         /* Void setup function for all program setup and initialization of 
         important library that are required in the system  exapmle initialization 
         of serial communication and lcd display 16*2.    */
//--------------------------------------------------------------------------------

void setup() {
  newKeypad.begin();
  mySerial.begin(57600);
  while (!Serial);
  Serial.begin(9600);
  //mySerial.begin(9600);
  finger.begin(57600);
  delay(5);
  SPI.begin();
  rfid.PCD_Init();

  finger.getTemplateCount();

  // if (finger.templateCount == 0) {
  //   Serial.println("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  // } else {
  //   Serial.print("Sensor contains "); 
  //   Serial.print(finger.templateCount); 
  //   Serial.println(" templates");
  //   Serial.println("Waiting for valid finger...");
  // }
  pinMode(relay, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  scrollmessage("Hi there ", "please scan  Id "); 
  delay(100);
  lcd.print("Ready ");
}

//--------------------------------------------------------------------------------
         /* lcd function which enable provide message in scroll format  */
//--------------------------------------------------------------------------------
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


//--------------------------------------------------------------------------------
         /* A finger print function to read fingers when detected and return value
          for  further validation.        */
//--------------------------------------------------------------------------------
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    Serial.println("No finger detected");
    return p;
  } else if (p != FINGERPRINT_OK) {
    Serial.println("Image capture failed");
    return p;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Image conversion failed");
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a fingerprint match!");
  } else {
    Serial.println("No fingerprint match found");
  }

  return finger.fingerID;
}

//--------------------------------------------------------------------------------
         /* This is finger print function which returns value when finger is
          detected  */
//--------------------------------------------------------------------------------
// Returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

//--------------------------------------------------------------------------------
         /* A void loop function is like the main function where all code that need 
         to run simultaneously or reapeted to be repeated   */
//--------------------------------------------------------------------------------


// Function to read serial data and return it as a String (assumed to be the server password)
String readEspdata() {
    String inputData = "";
    
    while (Serial.available() > 0) { 
        char incomingByte = Serial.read(); 
        
        // Stop reading if newline character is found
        if (incomingByte == '\n') {
            break;
        }
        
        inputData += incomingByte;
        
        // Small delay to ensure all bytes are read together
        delay(5); 
    }
    
    return inputData;
}

void loop() {
  //this runs the rfid  scanner to scan id and send to the esp32
  uint8_t rfidAuthentication();

  // Obtain the server-provided password
  String server_data = readEspdata();
  
  // Proceed only if Card ID matches the criteria (assuming "#170-?" for valid Card ID)
  if (server_data == "#170-?") {
      lcd.clear();
      lcd.print("card matched ..");
      successSound();
      delay(1000);
      lcd.clear();
      lcd.print("Enter Password:");
      Serial.println("Enter Password:");
      enteredPassword = "";
      char key;
      
      // Gather the password entered by the user
      while (true) {
          key = newKeypad.getKey();
          if (key) {
              keyPressTone();
              Serial.print(key);
              
              if (key == '#') {
                  break;  // Enter key pressed
              } else if (key == '*' || key == 'D') {
                  // Handle backspace or delete
                  if (enteredPassword.length() > 0) {
                      enteredPassword.remove(enteredPassword.length() - 1);
                      lcd.setCursor(0, 1);
                      lcd.print("                ");  // Clear display
                      lcd.setCursor(0, 1);
                      for (int i = 0; i < enteredPassword.length(); i++) {
                          lcd.print('*');  // Display * for each character
                      }
                  }
              } else {
                  enteredPassword += key;
                  lcd.setCursor(0, 1);
                  lcd.print("                ");
                  lcd.setCursor(0, 1);
                  for (int i = 0; i < enteredPassword.length(); i++) {
                      lcd.print('*');  // Display * for each character
                  }
              }
          }
      }
      
      Serial.print("Password entered: ");
      Serial.println(enteredPassword);
      lcd.clear();
      
      // Compare entered password with server password
      if (server_data == enteredPassword) {
          successSound();
          Serial.println("Access granted! Proceeding with fingerprint authentication...");
          scrollmessage("Password OK", "Place Finger...");
          delay(1000);
          
          // Fingerprint authentication
          int fingerID = -1;
          int attemptCount = 0;
          const int maxAttempts = 10;
          
          while (fingerID == -1 && attemptCount < maxAttempts) {
              fingerID = getFingerprintIDez();
              attemptCount++;
              delay(2000);  // Delay before retrying

              if (fingerID > -1) {
                  Serial.println("Fingerprint matched, access granted!");
                  scrollmessage("Fingerprint OK", "Access granted!");
                  successSound();
                  digitalWrite(relay, HIGH);  // Trigger relay
                  delay(5000);  // Keep relay active
                  digitalWrite(relay, LOW);  // Lock again
              } else {
                  if (attemptCount < maxAttempts) {
                      Serial.println("Waiting for valid fingerprint...");
                      scrollmessage("Place Finger", "Try again...");
                  } else {
                      Serial.println("Fingerprint did not match.");
                      scrollmessage("Fingerprint Fail", "Access denied!");
                  }
              }
          }

          if (fingerID == -1) {
              warningSound();
              Serial.println("Max attempts reached. No valid fingerprint detected.");
              scrollmessage("Timeout!", "Access denied!");
          }
      } else {
          warningSound();
          Serial.println("Incorrect password entered.");
          scrollmessage("Password Fail", "Access Denied!");
      }
  }
}