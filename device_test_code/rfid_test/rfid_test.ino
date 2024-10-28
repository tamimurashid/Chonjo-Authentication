#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10   // Define SDA pin
#define RST_PIN 9   // Define RST pin

MFRC522 rfid(SS_PIN, RST_PIN);  // Create an instance of MFRC522

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  SPI.begin();         // Initialize SPI bus
  rfid.PCD_Init();     // Initialize the RFID reader
  Serial.println("Place your card near the reader...");
}

uint8_t read_card(){
   // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select the card
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID (Unique Identifier) of the card without spaces
  // Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    // Print each byte in HEX format without spaces
    if (rfid.uid.uidByte[i] < 0x10) {   // Add leading zero for single-digit hex values
      Serial.print("0");
    }
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // // Print UID (Unique Identifier) of the card
  // Serial.print("Card UID: ");
  // for (byte i = 0; i < rfid.uid.size; i++) {
  //   Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //   Serial.print(rfid.uid.uidByte[i], HEX);
  // }
  // Serial.println();

  // Halt PICC (stop reading)
  rfid.PICC_HaltA();
  // Stop encryption on PCD (reader)
  rfid.PCD_StopCrypto1();

}

void loop() {
   read_card();
 
}
