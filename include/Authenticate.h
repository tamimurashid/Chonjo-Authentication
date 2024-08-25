// Writtten By Tamimu Rashid 
#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

#include <Adafruit_Fingerprint.h>

class Authenticate {
public:
    Authenticate(Adafruit_Fingerprint &fingerprintSensor);
    uint8_t getFingerprintID();
    uint8_t authenticateFingerprint();

private:
    Adafruit_Fingerprint &finger;
};

#endif // AUTHENTICATE_H
