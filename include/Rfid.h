#ifndef RFID_H
#define RFID_H

#include <SPI.h>
#include <MFRC522.h>

class Rfid {
public:
    // Constructor
    Rfid(uint8_t ssPin, uint8_t rstPin);
    
    // Initialize the RFID reader
    void begin();
    
    // Check for RFID card presence and read UID
    bool checkForCard();
    
    // Get the UID of the RFID card
    String getUID();

private:
    MFRC522 rfid;
};

#endif // RFID_H
