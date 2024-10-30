#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <SPI.h>

#define I2CADDR 0x26
#define trigger 7
#define buzzle 2 
const String CARD_MATCH_CODE = "#$170@!z";
const String PASSWORD_OK_CODE = "#$172@!z";


// Pin definition for SoftwareSerial
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(5, 6);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
constexpr uint8_t RST_PIN = 9;  // RST pin for RFID
constexpr uint8_t SS_PIN = 10;  // SDA (SS) pin for RFID
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

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

class Sound {
public:
    void keyPressTone() {
        playTone(500, 100);
        delay(100);
    }
    void warningSound() {
        playTone(500, 500);
        delay(100);
    }
    void successSound() {
        playTone(1000, 300);
        delay(100);
        playTone(1500, 300);
    }
private:
    void playTone(int frequency, int duration) {
        tone(buzzle, frequency, duration);
        delay(duration);
        noTone(buzzle);
    }
};

class Display {
public:
    void scrollMessage(String title, String message) {
        for (int position = 16; position >= 0; position--) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(title);
            lcd.setCursor(position, 1);
            lcd.print(message);
            delay(100);
        }
    }
    void displayMessage(const String& line1, const String& line2 = "") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);

}

};

class Peripherials : public Sound {
public:
    void setup();
    uint8_t read_card();
    String password_read();
    int getFingerprintIDez();
    
};
void Peripherials::setup() {
    Display display;
    newKeypad.begin();
    mySerial.begin(57600);
    while (!Serial);
    Serial.begin(9600);
    finger.begin(57600);
    delay(5);
    SPI.begin();
    rfid.PCD_Init();
    pinMode(trigger, OUTPUT);
    lcd.init();
    lcd.backlight();
    lcd.clear();
    display.scrollMessage("Hi there", "please scan ID");
    delay(100);
}

uint8_t Peripherials::read_card() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return 0; // Return 0 if no card is present
    }

    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) {
            Serial.print("0");
        }
        Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loading ....");
    lcd.setCursor(0, 1);
    lcd.print("wait a moment .");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return 1; // Return 1 if card is read successfully
}

String Peripherials::password_read() {
    enteredPassword = "";
    char key;
    while (true) {
        key = newKeypad.getKey();
        if (key) {
            keyPressTone();
            if (key == '#') {
                break;
            } else if (key == '*' || key == 'D') {
                if (enteredPassword.length() > 0) {
                    enteredPassword.remove(enteredPassword.length() - 1);
                    lcd.setCursor(0, 1);
                    lcd.print("                ");
                    lcd.setCursor(0, 1);
                    for (int i = 0; i < enteredPassword.length(); i++) {
                        lcd.print('*');
                    }
                }
            } else {
                enteredPassword += key;
                lcd.setCursor(0, 1);
                lcd.print("                ");
                lcd.setCursor(0, 1);
                for (int i = 0; i < enteredPassword.length(); i++) {
                    lcd.print('*');
                }
            }
        }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loading ....");
    lcd.setCursor(0, 1);
    lcd.print("wait a moment .");
    delay(1000);
    return enteredPassword;
}

int Peripherials::getFingerprintIDez() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) return -1;

    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID;
}
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
Peripherials peripherials;

class Handler : public Sound, public Display{
  public:
  void handleCardMatch();
  void handlePasswordMatch();

};

void Handler::handleCardMatch() {

    displayMessage("Card match!");
    successSound();
    displayMessage("Enter password ");
    peripherials.password_read();
}
void Handler::handlePasswordMatch() {
    successSound();
    displayMessage("Success", "Password OK!");
}

void setup() {
    peripherials.setup();
}

void loop() {
    Handler handler;
    String responseCode = readEspdata();

    if (responseCode == CARD_MATCH_CODE) {
        handler.handleCardMatch();
    } else if (responseCode == PASSWORD_OK_CODE) {
        handler.handlePasswordMatch();
    } else {
        Serial.println("Unknown response received: " + responseCode);
    }
}
