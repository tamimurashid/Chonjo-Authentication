#include "Enroll.h"
#include <MFRC522.h> // Include your RFID library here

// Define RFID pins
#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

Enroll::Enroll() {}

void Enroll::begin() {
    Serial.begin(9600); // Start serial communication at 9600 baud
    SPI.begin(); // Initialize SPI bus
    mfrc522.PCD_Init(); // Initialize RFID reader
}

void Enroll::enrollUser() {
    String rfidData = readRFID();
    if (rfidData.length() > 0) {
        String userName = "John Doe"; // Replace with actual user name or input method
        String message = "ENROLL:" + rfidData + ":" + userName;
        sendToNodeMCU(message);
    } else {
        Serial.println("No RFID tag detected.");
    }
}

void Enroll::handleSerial() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        Serial.println("Received from NodeMCU: " + data);
        // Add code here to handle the response from NodeMCU if needed
    }
}

void Enroll::sendToNodeMCU(const String& data) {
    Serial.println(data); // Send the data to NodeMCU
}

String Enroll::readRFID() {
    String rfidData = "";
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            rfidData += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
        }
        rfidData.toUpperCase();
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    }
    return rfidData;
}
