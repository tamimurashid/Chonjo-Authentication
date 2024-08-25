#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D1       // Configurable, connected to GPIO5 (D1)
#define SS_PIN D2        // Configurable, connected to GPIO4 (D2)

MFRC522 rfid(SS_PIN, RST_PIN);  // Create an MFRC522 object

void setup() {
  Serial.begin(9600);    // Initialize serial communications
  SPI.begin();             // Init SPI bus
  rfid.PCD_Init();         // Init MFRC522
  Serial.println("Place your RFID card near the reader...");
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

  // Show UID on serial monitor
  Serial.print("UID:");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
