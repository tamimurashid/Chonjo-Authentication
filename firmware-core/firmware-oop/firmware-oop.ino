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
#define trigger 7
#define buzzle 2 


//--------------------------------------------------------------------------------
         /* Pin definition   */
//---------------------------------------------------------------------------------
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(5, 6);
#else
#define mySerial Serial1
#endif


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


//initialize an instance of class NewKeypad
Keypad_I2C newKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR); 


String enteredPassword = "";
//--------------------------------------------------------------------------------
         /* sound functions  */
//--------------------------------------------------------------------------------
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
class Peripherials: Sound{
  public:
    uint8_t read_card(){
      // Check if a new card is present
      if (!rfid.PICC_IsNewCardPresent()) {
        return;
      }

      // Select the card
      if (!rfid.PICC_ReadCardSerial()) {
        return;
      }

      // Print UID (Unique Identifier) of the card without spaces
      // Serial.print("Card UID: ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        // Print each byte in HEX format without spaces
        if (rfid.uid.uidByte[i] < 0x10) {   // Add leading zero for single-digit hex values
          Serial.print("0");
        }
        Serial.print("#@!")
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print("?&")
      }
      Serial.println();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Loading ...."); 
      lcd.setCursor(0, 1);
      lcd.print("wait a momemnt ."); 
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

    }

    String password_read(){
      enteredPassword = "";
          char key;
          
          // Gather the password entered by the user
          while (true) {
              key = newKeypad.getKey();
              if (key) {
                  keyPressTone();
                  //Serial.print(key);
                  
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
          
          //Serial.print("Password entered: ");
          Serial.println(enteredPassword);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Loading ...."); 
          lcd.setCursor(0, 1);
          lcd.print("wait a momemnt ."); 
          delay(1000);

    }
    
    int getFingerprintIDez() {
      uint8_t p = finger.getImage();
      if (p != FINGERPRINT_OK)  return -1;

      p = finger.image2Tz();
      if (p != FINGERPRINT_OK)  return -1;

      p = finger.fingerFastSearch();
      if (p != FINGERPRINT_OK)  return -1;

      // found a match!
      Serial.print("Found ID #"); Serial.print(finger.fingerID);
      Serial.print(" with confidence of "); Serial.println(finger.confidence);
      return finger.fingerID;
    }
};
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
String readCommand() {
    String command = "";
    while (command.length() == 0) {
        if (Serial.available() > 0) {
            command = Serial.readStringUntil('\n');
            command.trim(); // Remove any whitespace or newline characters
            Serial.flush(); // Flush the serial buffer to clear any additional data
        }
    }
    return command;
}
void setup() {
  Display display;
  newKeypad.begin(), mySerial.begin(57600); while(!Serial); Serial.begin(9600), finger.begin(57600), delay(5);
  SPI.begin(), rfid.PCD_Init(), finger.getTemplateCount(), pinMode(trigger, OUTPUT), lcd.init(), lcd.backlight();
  lcd.clear(), display.scrollmessage("Hi there ", "please scan  Id "), delay(100);
}
Peripherials peripherials;
void loop(){
 
  // the logic begin here
  Sound sound; 
  peripherials.read_card();
  String read_data = readEspdata();
  if(read_data == "#$170@!z"){
    lcd.print("Card match !");
    lcd.clear();
    sound.successSound();
    delay(50);
    lcd.print("Enter password ");
    peripherials.password_read();

  }
  else if(read_data == "#$172@!z"){// 0685567131
    sound.successSound();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Success");
    lcd.setCursor(0, 1);
    lcd.print("Password OK !");
  }

  
}