/*
  This is the authentication cpp file 
  The main purpose of this file it runs for authentication process 
  In this file only class and functionality are written here but the real activity and operation runs 
  in main.cpp file 
*/
/* Start header declaration */
#include "Authenticate.h"
#include <Arduino.h>
#include <Wire.h>

/* End of header declaration */

#define SDA_PIN D2
#define SCL_PIN D1
 
/* Start of global constant variables pin for board and sensor connecion*/
/* 
   A short notice to follow, The board used here is Esp8266
   It used GPIO pins as ESP32 or Rasberrypie board is quite different from arduino
   So in assigning pins makes sure to find the ESP8266 image to compare labeled pins and actual 
   function pins in the board 
*/
const int trigger = 3; 
const int ledPin1 = 5; 
const int ledPin2 = 4; 
const int buzzle_pin = 0;
/* End of varibale declaration */

/* Beggining of other variable for properties such as status and delay */
const unsigned long warningDelay = 1000;  
const unsigned long successDelay = 1000; 
const unsigned long  backresponse = 5000; // this is the delay for returning back the lock after being triggered 
const unsigned long SoundDelay = 100;
/* End of other varible declaration */

/* Class declaration or creation */

Authenticate::Authenticate(Adafruit_Fingerprint &fingerprintSensor) : finger(fingerprintSensor) {}


uint8_t Authenticate::getFingerprintID() {
    uint8_t p = finger.getImage();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            //delay(1000);
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("No finger detected");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
    
           
            delay(1000);
            return p;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
           
            delay(1000);
            return p;
        default:
            Serial.println("Unknown error");     
            delay(1000);
            return p;
    }

    p = finger.image2Tz();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            //delay(1000);
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            delay(1000);
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
           
            delay(1000);
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
           
            delay(1000);
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            delay(1000);
            return p;
        default:
            Serial.println("Unknown error");
            delay(1000);
            return p;
    }

    p = finger.fingerSearch();
    // this is the succsses when finger print match with the database 
    if (p == FINGERPRINT_OK) {
        Serial.println("Found a print match!");
        Serial.print("Found ID #"); Serial.print(finger.fingerID);
        Serial.print(" with confidence of "); Serial.println(finger.confidence);
       
        return finger.fingerID;// return the finger print id after being recorginised 
      
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

uint8_t Authenticate::authenticateFingerprint() {
    return getFingerprintID();
}
