#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5        // SDA pin for RFID
#define RST_PIN 1       // RST pin for RFID
#define RELAY_PIN 15    // Relay control pin

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance

// Registered card UID (example UID)
byte registeredCard[4] = {0xDE, 0xAD, 0xBE, 0xEF}; 

void setup() {
  SPI.begin();             // Initialize SPI bus
  rfid.PCD_Init();         // Initialize MFRC522 RFID
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure relay is off initially
  Serial.begin(9600);      // Initialize serial communication
}

void loop() {
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Check if the UID matches the registered card
  if (checkCard()) {
    Serial.println("Authorized card detected. Activating relay.");
    digitalWrite(RELAY_PIN, HIGH); // Trigger relay
    delay(5000); // Keep relay on for 5 seconds
    digitalWrite(RELAY_PIN, LOW);  // Turn relay off
  } else {
    Serial.println("Unauthorized card detected.");
  }

  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

bool checkCard() {
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != registeredCard[i]) {
      return false;
    }
  }
  return true;
}
