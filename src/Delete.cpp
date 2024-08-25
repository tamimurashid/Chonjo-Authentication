#include "Delete.h"
#include <Arduino.h>

Delete::Delete(Adafruit_Fingerprint &fingerprintSensor) : finger(fingerprintSensor) {}

uint8_t Delete::readNumber() {
    uint8_t num = 0;
    while (num == 0) {
        while (!Serial.available());
        num = Serial.parseInt();
        if (num < 1 || num > 127) {
            Serial.println("Invalid ID. Please enter a number between 1 and 127.");
            num = 0; // Reset to 0 to prompt re-entry
        }
    }
    return num;
}

void Delete::deleteFingerprint() {
    Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
    uint8_t id = readNumber();
    if (id == 0) {
        Serial.println("ID #0 is not allowed.");
        return;
    }

    Serial.print("Deleting ID #");
    Serial.println(id);

    uint8_t p = finger.deleteModel(id);
    if (p == FINGERPRINT_OK) {
        Serial.println("Deleted!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
    } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not delete in that location");
    } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
    } else {
        Serial.print("Unknown error: 0x"); 
        Serial.println(p, HEX);
    }
}
