#include "Rfid.h"

// Constructor
Rfid::Rfid(uint8_t ssPin, uint8_t rstPin) : rfid(ssPin, rstPin) {}

// Initialize the RFID reader
void Rfid::begin() {
    SPI.begin(); // Initialize SPI bus
    rfid.PCD_Init(); // Initialize the RFID reader
}

// Check for RFID card presence and read UID
bool Rfid::checkForCard() {
    // Look for a new card
    if (!rfid.PICC_IsNewCardPresent()) {
        return false;
    }
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) {
        return false;
    }
    return true;
}

// Get the UID of the RFID card
String Rfid::getUID() {
    String uidStr = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        uidStr += String(rfid.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase();
    return uidStr;
}
