#include <SPI.h>
#include <MFRC522.h>

// Define the RFID pins based on your new connections
#define SS_PIN 4    // SDA/SS connected to GPIO4 (D2)
#define RST_PIN 2   // RST pin connected to GPIO2 (D4)

// Create an instance of the MFRC522 class with your defined pins
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
    Serial.begin(9600);   // Initialize serial communication
    SPI.begin();          // Initialize SPI bus
    rfid.PCD_Init();      // Initialize the RFID reader

    Serial.println("RFID reader initialized.");
}

void loop() {
    // Look for new cards
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        // Get the UID of the card
        Serial.print("Card detected! UID: ");
        for (byte i = 0; i < rfid.uid.size; i++) {
            Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();

        // Halt PICC
        rfid.PICC_HaltA();
        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
    } else {
        Serial.println("No card detected.");
    }

    delay(1000);  // Delay for a second before checking again
}
