#ifndef ENROLL_H
#define ENROLL_H

#include <Adafruit_Fingerprint.h>
#include <Arduino.h>

#define MAX_ID 127
#define NAME_MAX_LENGTH 32

class Enroll {
public:
    Enroll(Adafruit_Fingerprint &fingerprintSensor);
    uint8_t enrollFingerprint(uint8_t id, String name); // Correct function signature
    void enrollRFIDUser(String name);  // New function for enrolling via RFID
private:
    Adafruit_Fingerprint &finger;
    bool usedIDs[MAX_ID];
    String userNames[MAX_ID];

    uint8_t getFingerprintEnroll(uint8_t id);
    void saveIDWithName(uint8_t id, String name);
    uint8_t getAvailableID();  // New function to find an available ID
};

#endif
