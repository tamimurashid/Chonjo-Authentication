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

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(5, 6); // RX, TX for fingerprint sensor
#else
#define mySerial Serial1
#endif

MFRC522 rfid(10, 9); // RST_PIN, SS_PIN
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {4, 5, 6, 7};
byte colPins[COLS] = {0, 1, 2, 3};

// Keypad setup
Keypad_I2C newKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);

String enteredPassword = "";

// Classes and Instantiations
class Sound {
public:
  void keyPressTone() { tone(buzzle, 500, 100); delay(100); noTone(buzzle); }
  void warningSound() { tone(buzzle, 500, 500); delay(500); noTone(buzzle); }
  void successSound() { tone(buzzle, 1000, 300); delay(300); noTone(buzzle); }
};

class Display {
public:
  void showMessage(String line1, String line2) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(line1);
    lcd.setCursor(0, 1); lcd.print(line2);
  }
};

class Peripherals : public Sound {
public:
  void init() {
    SPI.begin(); rfid.PCD_Init();
    newKeypad.begin(); finger.begin(57600);
    lcd.init(); lcd.backlight();
  }

  bool readCard() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return false;
    Serial.print("Card UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.println();
    rfid.PICC_HaltA(); rfid.PCD_StopCrypto1();
    return true;
  }

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

  String enterPassword() {
    enteredPassword = ""; char key;
    while (true) {
      key = newKeypad.getKey();
      if (key) {
        keyPressTone();
        if (key == '#') break;
        else if (key == '*' && enteredPassword.length() > 0) {
          enteredPassword.remove(enteredPassword.length() - 1);
        } else enteredPassword += key;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        for (int i = 0; i < enteredPassword.length(); i++) lcd.print('*');
      }
    }
    return enteredPassword;
  }
};

// Instantiate global objects
Peripherals peripherals;
Display display;
Sound sound;

// Function declaration for ESP32-CAM response handling
String waitForEspResponse();

void setup() {
  Serial.begin(115200);
  peripherals.init();
  display.showMessage("System Ready", "Scan ID or Finger");
}

void loop() {
  bool rfidAuthenticated = false;
  bool fingerprintAuthenticated = false;
  bool passwordAuthenticated = false;

  // Step 1: Check RFID
  while (!rfidAuthenticated) {
    if (peripherals.readCard()) {
      display.showMessage("Card detected", "Proceed to Finger");
      sound.successSound();
      delay(1000);
      rfidAuthenticated = true;
    }
  }

  // Step 2: Check Fingerprint
  while (!fingerprintAuthenticated) {
    int attemptCount = 0;
    const int maxAttempts = 10;

    while (attemptCount < maxAttempts) {
      int fingerId = peripherals.getFingerprintIDez();
      attemptCount++;
      delay(2000);

      if (fingerId > -1) {
        display.showMessage("Fingerprint OK", "Proceed to PIN");
        sound.successSound();
        delay(1000);
        fingerprintAuthenticated = true;
        break;
      } else {
        if (attemptCount < maxAttempts) {
          Serial.println("Waiting for valid fingerprint...");
          display.showMessage("Place Finger", "Try again...");
        } else {
          Serial.println("Fingerprint did not match.");
          display.showMessage("Fingerprint Fail", "Access denied!");
          sound.warningSound();
          delay(2000);
          display.showMessage("System Ready", "Scan ID or Finger");
          break;
        }
      }
    }

    // If fingerprint authentication failed after max attempts, reset to RFID check
    if (!fingerprintAuthenticated) {
      rfidAuthenticated = false;
      break;
    }
  }

  // Step 3: Enter Password
  if (rfidAuthenticated && fingerprintAuthenticated) {
    display.showMessage("Enter Password", "");
    String password = peripherals.enterPassword();
    Serial.println(password); // Send password to ESP32 for verification
    display.showMessage("Verifying", "Please wait...");

    // Check ESP32 response for password authentication
    String response = waitForEspResponse();
    if (response == "0000") {
      passwordAuthenticated = true;
    }

    // Final Access Decision
    if (passwordAuthenticated) {
      sound.successSound();
      display.showMessage("Access", "Granted");
    } else {
      sound.warningSound();
      display.showMessage("Access", "Denied");
    }

    delay(3000);
    display.showMessage("System Ready", "Scan ID or Finger");
  }
}

// ESP32-CAM response function
String waitForEspResponse() {
  String response = "";
  while (true) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n') break;
      response += c;
    }
  }
  response.trim();
  return response;
}
