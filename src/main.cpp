#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

SoftwareSerial mySerial(D5, D6);  // D5 (GPIO14) is RX, D6 (GPIO12) is TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t getFingerprintID();
int getFingerprintIDez();
String command = "";

// Pin definitions for NodeMCU
constexpr uint8_t RST_PIN = D3;  // RST pin for RFID
constexpr uint8_t SS_PIN = D4;   // SDA (SS) pin for RFID

MFRC522 rfid(SS_PIN, RST_PIN);
void  scrollmessage(String title, String message);

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

byte rowPins[ROWS] = {D2, D1, D0, D8};  // Connect to the row pinouts of the keypad
byte colPins[COLS] = {D7, D6, D5, D4};  // Connect to the column pinouts of the keypad

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
      scrollmessage("Warning !!","Unknown card/tag .."); 
      return 1; // RFID tag mismatch
    }
  }
  
  Serial.println("Failed to read card.");
  lcd.clear();
  lcd.print("Failed to read");
  return 0; // Failed to read card
}

void setup() {
  mySerial.begin(57600);
  Serial.begin(9600);
  
  finger.begin(57600);
  delay(5);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  SPI.begin();
  rfid.PCD_Init();
  pinMode(D0, OUTPUT); // Pin for triggering relay or LED

  finger.getTemplateCount();
  
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
    delay(100);                // Adjust delay for scrolling speed
  }
}

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

void enrollFingerprint() {
  int id = finger.templateCount + 1; // Assign a new ID for the fingerprint
  Serial.print("Enrolling fingerprint with ID "); Serial.println(id);
  lcd.clear();
  scrollmessage("Enrolling ID:", String(id));

  // Enroll process
  for (int i = 1; i <= 2; i++) {
    Serial.print("Place finger "); Serial.print(i == 1 ? "for first scan" : "again for confirmation"); Serial.println(".");
    lcd.clear();
    scrollmessage("Place finger", i == 1 ? "First scan" : "Confirmation");
    while (finger.getImage() != FINGERPRINT_OK) {
      delay(100);
    }
    finger.image2Tz(i);
    if (i == 1) {
      Serial.println("Remove finger and place it again.");
      lcd.clear();
      scrollmessage("Remove finger", "Place it again");
      delay(2000);
    }
  }

  if (finger.createModel() == FINGERPRINT_OK) {
    if (finger.storeModel(id) == FINGERPRINT_OK) {
      Serial.println("Fingerprint enrolled successfully!");
      lcd.clear();
      scrollmessage("Success!", "Fingerprint saved");
    } else {
      Serial.println("Failed to store fingerprint.");
      lcd.clear();
      scrollmessage("Failed!", "Save error");
    }
  } else {
    Serial.println("Failed to create fingerprint model.");
    lcd.clear();
    scrollmessage("Failed!", "Model error");
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
                yield();  // Allow WDT to reset
            }
        }
        
        lcd.clear();
        
        if (enteredPassword == "123456789") {
            scrollmessage("Password OK", "Place Finger...");
            delay(1000);
            
            int fingerID = -1;
            int attemptCount = 0;
            const int maxAttempts = 5;

            while (fingerID == -1 && attemptCount < maxAttempts) {
                fingerID = getFingerprintIDez();
                attemptCount++;
                delay(1000);

                if (fingerID > -1) {
                    scrollmessage("Fingerprint OK", "Access granted!");
                    digitalWrite(D0, HIGH); // Trigger relay or unlock door
                    delay(5000); // Keep the door unlocked for 5 seconds
                    digitalWrite(D0, LOW); // Lock the door again
                } else {
                    if (attemptCount < maxAttempts) {
                        scrollmessage("Scan failed", "Try again");
                    } else {
                        scrollmessage("Access denied", "Max attempts reached");
                    }
                }
                yield();  // Allow WDT to reset
            }
        } else {
            scrollmessage("Wrong Password", "Access denied");
        }
    }
    delay(1000);
    yield();  // Allow WDT to reset
}
