#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Pin definitions for Arduino Uno
constexpr uint8_t RST_PIN = 0;  // RST pin for RFID
constexpr uint8_t SS_PIN = 10;  // SDA (SS) pin for RFID

MFRC522 rfid(SS_PIN, RST_PIN);

// LCD I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

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

uint8_t id;
String tag;
String enteredPassword = "";

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
    Serial.println(tag);
    
    lcd.clear();
    lcd.print("RFID Tag:");
    lcd.setCursor(0, 1);
    lcd.print(tag);
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    
    if (tag == "6e5d273") { // Replace this with your expected RFID tag in HEX
      Serial.println("RFID tag matched!");
      lcd.clear();
      lcd.print("Tag matched!");
      return 2; // RFID tag matched
    } else {
      Serial.println("RFID tag did not match.");
      lcd.clear();
      lcd.print("card/ tag  not match!");
      lcd.clear();
      return 1; // RFID tag mismatch
    }
  }
  
  Serial.println("Failed to read card.");
  lcd.clear();
  lcd.print("Failed to read");
  return 0; // Failed to read card
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(8, OUTPUT); // Pin for triggering relay or LED
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  scrollmessage("Hi there ", "please scan  Id "); 
  delay(100);
  lcd.print("Ready ");
}

void scrollmessage(String title, String message) {
  for (int position = 16; position >= 0; position--) {
    lcd.clear();               // Clear the LCD
    lcd.setCursor(0, 0);       // Set cursor to row 0, column 0
    lcd.print(title);          // Print title in the first row
    lcd.setCursor(position, 1); // Set cursor to row 1 at current position
    lcd.print(message);        // Print message in the second row
    delay(200);                // Adjust delay for scrolling speed
  }
}

void loop() {
  uint8_t rfidStatus = rfidAuthentication();
  if (rfidStatus > 1) {  // RFID tag matched
    lcd.clear();
    lcd.print("card matched ..");
    delay(1000);
    lcd.clear();
    lcd.print("Enter Password:");
    Serial.println("Enter Password:");
    enteredPassword = "";
    char key;
    while (true) {
      key = keypad.getKey();
      if (key) {
        if (key == '#') {
          break;  // Enter key pressed
        } else if (key == 'D') {
          if (enteredPassword.length() > 0) {
            enteredPassword.remove(enteredPassword.length() - 1); // Remove the last character
            lcd.setCursor(0, 1);
            lcd.print("                "); // Clear the previous display line
            lcd.setCursor(0, 1);
            for (int i = 0; i < enteredPassword.length(); i++) {
              lcd.print('*'); // Print * for each character in enteredPassword
            }
          }
        } else {
          enteredPassword += key;
          lcd.setCursor(0, 1);
          lcd.print("                "); // Clear the previous display line
          lcd.setCursor(0, 1);
          for (int i = 0; i < enteredPassword.length(); i++) {
            lcd.print('*'); // Print * for each character entered
          }
        }
      }
    }
    Serial.print("Password entered: ");
    Serial.println(enteredPassword);
    lcd.clear();
    
    if (enteredPassword == "123456789") {
      Serial.println("Access granted!");
      scrollmessage("Thank you ", "Access granted!"); 
      digitalWrite(1, HIGH); // Trigger relay or unlock door
      delay(5000); // Keep the door unlocked for 5 seconds
      digitalWrite(1, LOW); // Lock the door again
      lcd.clear();
      lcd.print("Hi there! ");

    } else {
      Serial.println("Access denied!");
      scrollmessage("Ooops !! ", "Access denied!"); 
      delay(100);
      lcd.clear();
      lcd.print("Scan id again..");
    }
  }
  
  delay(1000); // Optional delay to prevent constant polling
}
