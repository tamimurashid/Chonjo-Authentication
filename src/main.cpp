#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include "Authenticate.h"
#include "Enroll.h"
#include "Delete.h"
#include "Indicator.h"
#include "LCD_Display.h"

#define SDA_PIN D2
#define SCL_PIN D1

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(13, 15); // for ESP8266 this is pin no 8 and 7
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger(&mySerial);
Authenticate auth(finger);
Enroll enroll(finger);
Delete del(finger);
LCD_Display lcd(0x27, 16, 2);

// Function prototypes
String readCommand();
String readArduinoData();

void setup() {
    Serial.begin(9600);
    Wire.begin(SDA_PIN, SCL_PIN);
    lcd.begin();
    while (!Serial);
    delay(100);
    Serial.println("\n\nFingerprint System");

    lcd.printMessage("Hi there", 0);
    delay(1000);

    lcd.scrollMessage("Welcome to Authentication system", "Talion's System", 1);

    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) { delay(1); }
    }
}

void loop() {
    Serial.println("Waiting for command...");
    lcd.clearMessage();
    lcd.printMessage("Waiting for", 0);
    lcd.printMessage("command...", 1);
    String command = readCommand();

    if (command == "ENROLL" || command == "enroll" || command == "2") {
        Serial.println("Starting enrollment...");
        lcd.clearMessage();
        lcd.scrollMessage("Starting enrollment...", "Registration..", 1);
        
        // Get the ID and name from the Arduino
        Serial.println("Enter user ID and Name from Arduino:");
        String data = readArduinoData();
        if (data.length() > 0) {
            int separatorIndex = data.indexOf(',');
            if (separatorIndex != -1) {
                uint8_t id = data.substring(0, separatorIndex).toInt();
                String name = data.substring(separatorIndex + 1);
                enroll.enrollFingerprint(id, name);
            } else {
                Serial.println("Invalid data format. Expected format: ID,Name");
            }
        } else {
            Serial.println("No data received from Arduino.");
        }

    } else if (command == "AUTH" || command == "A" || command == "a" || command == "1") {
        Serial.println("Entering continuous authentication mode.");
        lcd.clearMessage();
        lcd.printMessage("Authentication..", 0);
        lcd.printMessage("mode On progress", 1);
        while (true) {
            auth.authenticateFingerprint();

            // Check for 'S' command to stop authentication mode
            if (Serial.available() > 0) {
                char c = Serial.read();
                if (c == 'S' || c == 's') {
                    Serial.println("Stopping authentication mode.");
                    lcd.clearMessage();
                    lcd.printMessage("Authentication", 0);
                    lcd.printMessage("mode stopped.", 1);
                    break;
                }
            }
        }
    } else if (command == "DELETE" || command == "d" || command == "D" || command == "3") {
        Serial.println("Starting delete mode...");
        lcd.clearMessage();
        lcd.scrollMessage("Starting delete mode...", "Registration..", 1);

        del.deleteFingerprint();
    } else {
        Serial.println("Invalid command.");
        lcd.clearMessage();
        lcd.printMessage("Invalid command", 0);
        lcd.printMessage("Try again.", 1);
    }
}

String readCommand() {
    String command = "";
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                break;
            } else {
                command += c;
            }
        }
    }
    return command;
}

String readArduinoData() {
    String data = "";
    while (Serial.available() == 0) {
        // Wait for data to be available
    }
    while (Serial.available() > 0) {
        char c = Serial.read();
        data += c;
    }
    return data;
}
