#include "Enroll.h"
#include <Arduino.h>

Enroll::Enroll(Adafruit_Fingerprint &fingerprintSensor) : finger(fingerprintSensor) {
    // Initialize the used IDs array and userNames array
    for (int i = 0; i < MAX_ID; i++) {
        usedIDs[i] = false;
        userNames[i] = "";
    }
}

uint8_t Enroll::enrollFingerprint(uint8_t id, String name) {
    saveIDWithName(id, name);  // Save the ID with the user's name
    Serial.print("Assigned unique ID: ");
    Serial.println(id);

    // Begin the enrollment process
    return getFingerprintEnroll(id);
}

void Enroll::enrollRFIDUser(String name) {
    // Wait for the RFID data to come in via Serial communication
    Serial.println("Waiting for RFID tag...");
    while (Serial.available() == 0) {
        delay(100);
    }

    String rfidTag = Serial.readStringUntil('\n');  // Read RFID tag ID
    rfidTag.trim();

    if (rfidTag.length() > 0) {
        uint8_t id = getAvailableID();
        if (id != 255) {  // Check if an available ID is found
            Serial.print("RFID Tag: ");
            Serial.println(rfidTag);
            enrollFingerprint(id, name);  // Enroll with available ID and name
        } else {
            Serial.println("No available ID slots.");
        }
    } else {
        Serial.println("No RFID tag received.");
    }
}

uint8_t Enroll::getAvailableID() {
    for (int i = 1; i <= MAX_ID; i++) {
        if (!usedIDs[i - 1]) {
            usedIDs[i - 1] = true;
            return i;
        }
    }
    return 255;  // Return 255 if no available ID is found
}

uint8_t Enroll::getFingerprintEnroll(uint8_t id) {
    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #");
    Serial.println(id);
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
    Serial.print("ID ");
    Serial.println(id);
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
    Serial.print("Creating model for #");
    Serial.println(id);

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

    Serial.print("ID ");
    Serial.println(id);
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

void Enroll::saveIDWithName(uint8_t id, String name) {
    if (id >= 1 && id <= MAX_ID) {
        userNames[id - 1] = name; // Save the name in the array
        Serial.print("ID ");
        Serial.print(id);
        Serial.print(" is associated with user: ");
        Serial.println(name);
    }
}
