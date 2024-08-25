#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include "Authenticate.h"
#include "Enroll.h"
#include "Delete.h"
#include "Indicator.h"

// For ESP8266
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(13, 15);  // For ESP8266
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger(&mySerial);
Authenticate auth(finger);
Enroll enroll(finger);
Delete del(finger);

// WiFi credentials
const char* ssid = "tembo";          // Replace with your WiFi SSID
const char* password = "!@#Itap@2024";  // Replace with your WiFi password

// Server URL (change to your server's IP or domain)
const char* serverUrl = "http://your_server_address/path_to_api_endpoint";

// Function prototypes
String readCommand();
void sendDataToServer(int authResult);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    delay(100);
    Serial.println("\n\nFingerprint System");

    // Initialize WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");

    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) { delay(1); }
    }
}

void loop() {
    Serial.println("Waiting for command...");
    String command = readCommand();

    if (command == "ENROLL" || command == "enroll" || command == "2") {
        Serial.println("Starting enrollment...");
        enroll.enrollFingerprint();
    } else if (command == "AUTH" || command == "A" || command == "a" || command == "1") {
        Serial.println("Entering continuous authentication mode.");
        while (true) {
            int authResult = auth.authenticateFingerprint();

            // Send authentication result to server
            sendDataToServer(authResult);

            // Check for 'S' command to stop authentication mode
            if (Serial.available() > 0) {
                String stopCommand = readCommand();
                if (stopCommand == "S" || stopCommand == "s" || stopCommand == "0") {
                    Serial.println("Exiting authentication mode.");
                    break;
                }
            }
            delay(100); // Add a slight delay to prevent rapid looping
        }
    } else if (command == "DELETE" || command == "delete" || command == "4") {
        Serial.println("Starting deletion...");
        del.deleteFingerprint();
    } else if (command == "EXIT") {
        Serial.println("Exiting...");
        while (1); // Halt the program
    } else {
        Serial.println("Invalid command, please try again.");
    }

    delay(1000); // Delay before processing the next command
}

// Function to send data to the server
void sendDataToServer(int authResult) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        // Start connection to server
        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Prepare data to send
        String postData = "authResult=" + String(authResult);
        
        // Send the request
        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        // Close connection
        http.end();
    } else {
        Serial.println("WiFi not connected");
    }
}

// Function definition
String readCommand() {
    String command = "";
    while (command.length() == 0) {
        if (Serial.available() > 0) {
            command = Serial.readStringUntil('\n');
            command.trim(); // Remove any whitespace or newline characters
            Serial.flush(); // Flush the serial buffer to clear any additional data
        }
    }
    return command;
}
