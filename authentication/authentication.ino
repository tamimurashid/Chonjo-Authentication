#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(14, 15);  // D1 is RX, D2 is TX
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t getFingerprintID();
void enrollfingerprint();
uint8_t getFingerprintEnroll();
uint8_t readnumber(void);
int getFingerprintIDez();
String command = "";



//SoftwareSerial mySerial(14, 15); // RX, TX

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
    
    if (tag == "27b25580") { // Replace this with your expected RFID tag in HEX
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
/*----------------------------------------------



*///--------------------------------------------
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


void enrollfingerprint(){

   Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (! getFingerprintEnroll() );

}


/*----------------------------------------------



*///--------------------------------------------
void setup() {
  mySerial.begin(57600);
  while (!Serial);
  Serial.begin(9600);
  //mySerial.begin(9600);
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
  pinMode(2, OUTPUT); // Pin for triggering relay or LED

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.println("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.print("Sensor contains "); 
    Serial.print(finger.templateCount); 
    Serial.println(" templates");
    Serial.println("Waiting for valid finger...");
  }
  
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
            Serial.println("Access granted! Proceeding with fingerprint authentication...");
            scrollmessage("Password OK", "Place Finger...");
            delay(1000);
            
           int fingerID = -1;
            int attemptCount = 0;
            const int maxAttempts = 5; // Maximum attempts or time to wait for fingerprint

            while (fingerID == -1 && attemptCount < maxAttempts) {
                fingerID = getFingerprintIDez();
                attemptCount++;
                delay(1000); // Wait for 1 second before checking again

                if (fingerID > -1) {
                    Serial.println("Fingerprint matched, access granted!");
                    scrollmessage("Fingerprint OK", "Access granted!");
                    digitalWrite(1, HIGH); // Trigger relay or unlock door
                    delay(5000); // Keep the door unlocked for 5 seconds
                    digitalWrite(1, LOW); // Lock the door again
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
                // Handle case where no valid fingerprint was detected after maximum attempts
                Serial.println("Max attempts reached. No valid fingerprint detected.");
                scrollmessage("Timeout!", "Access denied!");
            }

            lcd.clear();
            scrollmessage("Hi there", "Please scan ID");
            
        } else {
            Serial.println("Access denied!");
            scrollmessage("Error !!", "Wrong password!!");
            delay(100);
            scrollmessage("Ooops !!", "Access denied!!");
            delay(100);
            lcd.clear();
            lcd.print("Scan ID again...");
        }
    }
    
    delay(1000); // Optional delay to prevent constant polling
}
