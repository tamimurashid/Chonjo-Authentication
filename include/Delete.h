#ifndef DELETE_H
#define DELETE_H

#include <Adafruit_Fingerprint.h>

class Delete {
public:
    Delete(Adafruit_Fingerprint &fingerprintSensor);
    void deleteFingerprint();  // Function to delete a fingerprint by ID

private:
    Adafruit_Fingerprint &finger;
    uint8_t readNumber();  // Function to read and validate a number input from the user
};

#endif
