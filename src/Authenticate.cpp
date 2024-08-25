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
#include "Indicator.h"
#include "Lock.h"
#include "AlertSound.h"
#include "LCD_Display.h"
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
AlertSound alertsound(buzzle_pin, SoundDelay);
Lock lock(trigger, backresponse);
Indicator indicator(ledPin1, ledPin2,  warningDelay, successDelay);
Authenticate::Authenticate(Adafruit_Fingerprint &fingerprintSensor) : finger(fingerprintSensor) {}
LCD_Display lcd1(0x27, 16, 2);
/*End of class declaration */

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
            lcd1.printMessage("Communication error", 0);
            indicator.error();
            delay(1000);
            return p;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            indicator.error();
            delay(1000);
            return p;
        default:
            Serial.println("Unknown error");
            lcd1.printMessage("Unknown error", 1);
            indicator.error();
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
            lcd1.printMessage("Error ...", 0);
            lcd1.printMessage("Put finger print correct", 1);
            delay(1000);
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            indicator.error();
            delay(1000);
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            indicator.error();
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
        lcd1.printMessage("Thankyou .. ", 0);
        lcd1.printMessage("FInger match  with id: " + finger.fingerID, 1);
        indicator.success();// this will light the green led to show succsses 
        //servoControl.rotateOnSuccess();
        alertsound.Success_alert();
        lock.Open();// this will trigger the solenoid lock to open and remain open for five minutes 
        return finger.fingerID;// return the finger print id after being recorginised 
      
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
         indicator.error();
        return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
        Serial.println("Did not find a match");
        lcd1.printMessage("Did not find a match", 0);
         indicator.error();
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }
}

uint8_t Authenticate::authenticateFingerprint() {
    return getFingerprintID();
}
