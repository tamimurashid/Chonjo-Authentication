#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(D1, D2);  // D1 is RX, D2 is TX
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

#define TRIGGER_PIN D8
void handleCommand();
uint8_t getFingerprintID();
int getFingerprintIDez();
String command = "";

void setup() {
  Serial.begin(9600);
  mySerial.begin(57600);
  while (!Serial);  // Wait for Serial to be ready

  Serial.println("\n\nAdafruit Fingerprint Sensor Test");

  finger.begin(57600);
  delay(5);

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

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.println("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.print("Sensor contains "); 
    Serial.print(finger.templateCount); 
    Serial.println(" templates");
    Serial.println("Waiting for valid finger...");
  }

  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      handleCommand();
      command = "";  // Clear the command after handling it
    } else {
      command += inChar;
    }
  }
}

void handleCommand() {
  command.trim();
  
  if (command == "AUTH" || command == "A" || command == "a" || command == "1") {
    Serial.println("Performing authentication...");
    int fingerID = getFingerprintIDez();
    if (fingerID > -1) {
      digitalWrite(TRIGGER_PIN, HIGH); // Trigger pin D8
      Serial.println("Fingerprint matched, access granted!");
      Serial.println("SUCCESS");
      delay(1000); // Keep the pin high for 1 second
      digitalWrite(TRIGGER_PIN, LOW);
    } else {
      Serial.println("Fingerprint did not match.");
      Serial.println("ERROR");
    }
  } else if (command == "EXIT" || command == "0") {
    Serial.println("Exiting...");
    while (1); // Stop the loop
  } else {
    Serial.println("Invalid command, please try again.");
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    Serial.println("No finger detected");
    return p;
  } else if (p != FINGERPRINT_OK) {
    Serial.println("Image capture failed");
    return p;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Image conversion failed");
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a fingerprint match!");
  } else {
    Serial.println("No fingerprint match found");
  }

  return finger.fingerID;
}

// Returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
