#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Keypad_I2C.h>
#include<Keypad.h>
#include <Wire.h>

// I2C Address for Keypad
#define I2CADDR 0x26 // Use appropriate I2C address for your setup

// Keypad configuration
const byte ROWS = 4;  // 4 rows
const byte COLS = 4;  // 4 columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//--------------------------------------------------------------------------------
         /* Keypad setup pins declaration  */
//--------------------------------------------------------------------------------
byte rowPins[ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad


//initialize an instance of class keypad
Keypad_I2C keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR); 


// Software Serial for Fingerprint (RX, TX pins)
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(6, 5);  // RX, TX
#define buzzle 2
#else
#define mySerial Serial1
#endif

// Fingerprint and LCD initialization
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t id;
uint8_t p;  // Use existing 'p' variable to avoid redeclaration issues

// Sound Feedback Functions
void keyPressTone() {
  tone(buzzle, 1000, 100); // 1kHz tone for 100ms
  delay(100); 
  noTone(buzzle);
}

void warningSound() {
  tone(buzzle, 500, 500);  // 500Hz tone for 500ms
  delay(500); 
  noTone(buzzle);
}

void successSound() {
  tone(buzzle, 1500, 300);  // 1.5kHz tone for 300ms
  delay(300); 
  noTone(buzzle);
  tone(buzzle, 2000, 300);  // 2kHz tone for 300ms
  delay(300); 
  noTone(buzzle);
}

void setup() {
  Serial.begin(9600);
  finger.begin(57600);
  keypad.begin();

  // Wait for Serial
  while (!Serial);
  delay(100);

  Serial.println("\nAdafruit Fingerprint sensor setup");
  finger.getTemplateCount();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  if (finger.templateCount == 0) {
    Serial.println("No fingerprints in sensor.");
    lcd.print("No fingerprint data");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Enroll mode");
    lcd.setCursor(0, 1);
    lcd.print("Sensor count: ");
    lcd.print(finger.templateCount);
    delay(100);
    Serial.print("Sensor contains "); 
    Serial.println(finger.templateCount);
  }

  finger.getParameters();
  showMenu();
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    keyPressTone();
    switch (key) {
      case '1': // Enroll fingerprint
        enrollFinger();
        break;
      case '3': // Delete fingerprint
        deleteFinger();
        break;
      case '4': // List all stored fingerprints
        listFingerprints();
        break;
      default:
        lcd.clear();
        lcd.print("Invalid option");
        warningSound();
        delay(2000);
        showMenu();
        break;
    }
  }
}

void enrollFinger() {
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
  lcd.print("Press # to Confirm");
  while (keypad.getKey() != '#');
  
  lcd.clear();
  lcd.print("Place finger...");
  Serial.print("Enrolling ID #"); Serial.println(id);

  if (getFingerprintEnroll()) {
    lcd.clear();
    lcd.print("Enrolled!");
    successSound();
  } else {
    lcd.clear();
    lcd.print("Enroll Failed");
    warningSound();
  }
  delay(2000);
  showMenu();
}

void deleteFinger() {
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

  Serial.print("Deleting ID #"); Serial.println(id);
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Deleted!");
    successSound();
  } else {
    lcd.clear();
    lcd.print("Delete Failed");
    warningSound();
  }
  delay(2000);
  showMenu();
}

uint8_t readNumber(void) {
  uint8_t num = 0;
  lcd.clear();
  lcd.print("Enter ID:");
  while (true) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      num = num * 10 + (key - '0');
      lcd.print(key);
    } else if (key == '#') {
      return num;
    }
  }
}

void showMenu() {
  lcd.clear();
  lcd.print("1: Enroll");
  lcd.setCursor(0, 1);
  lcd.print("3: Delete 4: List");
}

uint8_t getFingerprintEnroll() {
  int result = -1;
  Serial.println("Waiting for finger...");
  while (result != FINGERPRINT_OK) {
    result = finger.getImage();
    if (result == FINGERPRINT_OK) {
      Serial.println("Image taken");
    } else if (result == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else {
      return result;
    }
  }

  result = finger.image2Tz(1);
  if (result != FINGERPRINT_OK) {
    return result;
  }

  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);

  while (finger.getImage() != FINGERPRINT_NOFINGER);

  lcd.clear();
  lcd.print("Place again...");
  while (result != FINGERPRINT_OK) {
    result = finger.getImage();
    if (result == FINGERPRINT_OK) {
      Serial.println("Image taken");
    } else if (result == FINGERPRINT_NOFINGER) {
      Serial.print(".");
    } else {
      return result;
    }
  }

  result = finger.image2Tz(2);
  if (result != FINGERPRINT_OK) {
    return result;
  }

  lcd.clear();
  lcd.print("Creating model");
  result = finger.createModel();
  if (result == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else {
    return result;
  }

  lcd.clear();
  lcd.print("Storing ID...");
  result = finger.storeModel(id);
  return (result == FINGERPRINT_OK);
}

void listFingerprints() {
  lcd.clear();
  lcd.print("Stored IDs:");
  for (int i = 1; i < 127; i++) {
    uint8_t p = finger.loadModel(i);
    if (p == FINGERPRINT_OK) {
      Serial.print("ID "); Serial.println(i);
      lcd.setCursor(0, 1);
      lcd.print(i);
      delay(1000); 
    }
  }
  Serial.println("Listing complete");
}

