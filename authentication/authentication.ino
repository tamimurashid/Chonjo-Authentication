#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Pin definitions for Arduino Uno
constexpr uint8_t RST_PIN = 9;  // RST pin for RFID
constexpr uint8_t SS_PIN = 10;  // SDA (SS) pin for RFID

// SoftwareSerial for the fingerprint sensor
SoftwareSerial mySerial(3, 4);  // RX, TX (connected to TX, RX on the fingerprint sensor)

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
MFRC522 rfid(SS_PIN, RST_PIN);

uint8_t id;
String tag;

String readCommand() {
  String command = "";
  while (Serial.available()) {
    char ch = (char)Serial.read();
    command += ch;
    delay(10); // Small delay to ensure all characters are read
  }
  command.trim(); // Remove any leading/trailing whitespace
  return command;
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(8, OUTPUT); // Pin for triggering relay or LED
  while (!Serial);
  delay(100);

  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

uint8_t readnumber() {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = finger.deleteModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Deleted!");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_BADLOCATION:
      Serial.println("Could not delete in that location");
      break;
    case FINGERPRINT_FLASHERR:
      Serial.println("Error writing to flash");
      break;
    default:
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      break;
  }
  return p;
}

void deleteIdfingerprint() {
  Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
  uint8_t id = readnumber();
  if (id == 0) {
    return;
  }
  Serial.print("Deleting ID #");
  Serial.println(id);
  deleteFingerprint(id);
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.print("Creating model for #"); Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else {
    Serial.println("Error during model creation");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else {
    Serial.println("Error storing fingerprint");
    return p;
  }

  return true;
}

void enrollfingerprint() {
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  while (!getFingerprintEnroll());
}

bool rfidAuthentication() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return false;
  }
  if (rfid.PICC_ReadCardSerial()) {
    tag = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.print("RFID Tag: ");
    Serial.println(tag);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    if (tag == "11093393") { // Replace this with your expected RFID tag
      Serial.println("RFID tag matched!");
      return true;
    } else {
      Serial.println("RFID tag did not match.");
      return false;
    }
  }
  Serial.println("Failed to read card.");
  return false;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    Serial.print("Matching ID #"); Serial.println(finger.fingerID);
    return finger.fingerID;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
}

void loop() {
  if (Serial.available()) {
    String command = readCommand();
    if (command == "enroll") {
      enrollfingerprint();
    } else if (command == "delete") {
      deleteIdfingerprint();
    } 
  }

 if(getFingerprintID() > 0 ){
    bool rfid = rfidAuthentication();
    if(rfid == true){
      Serial.println("Access granted!");
      digitalWrite(8, HIGH); // Trigger relay or unlock door
      delay(5000); // Keep the door unlocked for 5 seconds
      digitalWrite(8, LOW); 
    } else {
      Serial.println("Access denied!");
    }
 }
  // if (rfidAuthentication()) {
  //   uint8_t fingerprintID = getFingerprintID();
  //   if (fingerprintID > 0) {
  //     Serial.println("Access granted!");
  //     digitalWrite(8, HIGH); // Trigger relay or unlock door
  //     delay(5000); // Keep the door unlocked for 5 seconds
  //     digitalWrite(8, LOW); // Lock the door again
  //   } else {
  //     Serial.println("Access denied!");
  //   }
  // }

  delay(1000); // Optional delay to prevent constant polling
}
