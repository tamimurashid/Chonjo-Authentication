#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
#include <SoftwareSerial.h>
SoftwareSerial mySerial(14, 15); // RX, TX
#define buzzle 16

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

uint8_t id;
uint8_t p; // Declare p here to avoid redeclaration issues

void setup() {
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor setup");

  // set the data rate for the sensor serial port
   finger.begin(57600);

  // if (finger.verifyPassword()) {
  //   Serial.println("Found fingerprint sensor!");
  // } else {
  //   Serial.println("Did not find fingerprint sensor :(");
  //   while (1) { delay(1); }
  // }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  showMenu();
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case '1': // Enroll
        lcd.clear();
        lcd.print("Enter ID and #");
        id = readNumber();
        if (id == 0) {
          lcd.clear();
          lcd.print("Invalid ID");
          delay(2000);
          showMenu();
          return;
        }
        lcd.clear();
        lcd.print("place a finger ..");
        Serial.print("Enrolling ID #");
        Serial.println(id);
        if (getFingerprintEnroll()) {
          lcd.clear();
          lcd.print("Enrolled!");
        } else {
          lcd.clear();
          lcd.print("Enroll Failed");
        }
        delay(2000);
        showMenu();
        break;
        
      case '3': // Delete
        lcd.clear();
        lcd.print("Enter ID and #");
        id = readNumber();
        if (id == 0) {
          lcd.clear();
          lcd.print("Invalid ID");
          delay(2000);
          showMenu();
          return;
        }
        Serial.print("Deleting ID #");
        Serial.println(id);
        p = finger.deleteModel(id); // Use existing p
        if (p == FINGERPRINT_OK) {
          lcd.clear();
          lcd.print("Deleted!");
        } else {
          lcd.clear();
          lcd.print("Delete Failed");
        }
        delay(2000);
        showMenu();
        break;
        
      case '4': // Check Finger Present
        lcd.clear();
        lcd.print("Checking Finger");
        Serial.println("Checking for finger presence...");
        p = finger.getImage(); // Use existing p
        if (p == FINGERPRINT_OK) {
          lcd.clear();
          lcd.print("Finger Present");
        } else {
          lcd.clear();
          lcd.print("No Finger");
        }
        delay(2000);
        showMenu();
        break;
        
      default:
        lcd.clear();
        lcd.print("Invalid option");
        delay(2000);
        showMenu();
        break;
    }
  }
}

uint8_t readNumber(void) {
  uint8_t num = 0;
  lcd.clear();
  lcd.print("Enter ID:");
  while (num == 0) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      num = num * 10 + (key - '0');
      lcd.print(key);
    } else if (key == '#') {
      return num;
    }
  }
  return num;
}

void showMenu() {
  lcd.clear();
  lcd.print("1: Enroll");
  lcd.setCursor(0, 1);
  lcd.print("3: Delete");
  lcd.setCursor(0, 2);
  lcd.print("4: Check");
  lcd.setCursor(0, 2);
}

uint8_t getFingerprintEnroll() {
  int result = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (result != FINGERPRINT_OK) {
    result = finger.getImage();
    switch (result) {
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

  result = finger.image2Tz(1);
  switch (result) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return result;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return result;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return result;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return result;
    default:
      Serial.println("Unknown error");
      return result;
  }

  Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  result = 0;
  while (result != FINGERPRINT_NOFINGER) {
    result = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  result = -1;
  lcd.clear();
  Serial.println("Place same finger again");
  lcd.print("Place same finger");
  while (result != FINGERPRINT_OK) {
    result = finger.getImage();
    switch (result) {
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

  result = finger.image2Tz(2);
  switch (result) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return result;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return result;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return result;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return result;
    default:
      Serial.println("Unknown error");
      return result;
  }

  Serial.print("Creating model for #"); Serial.println(id);
  lcd.clear();
  lcd.print("Creating model");
  result = finger.createModel();
  if (result == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (result == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return result;
  } else if (result == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return result;
  } else {
    Serial.println("Unknown error");
    return result;
  }

  Serial.print("ID "); Serial.println(id);
  result = finger.storeModel(id);
  if (result == FINGERPRINT_OK) {
    Serial.println("Stored!");
    return true;
  } else if (result == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return result;
  } else if (result == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return result;
  } else if (result == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return result;
  } else {
    Serial.println("Unknown error");
    return result;
  }
}
