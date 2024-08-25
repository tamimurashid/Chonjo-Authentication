#include<Arduino.h>
void setup() {
  Serial.begin(9600); // Begin serial communication
}

void loop() {
  if (Serial.available()) {
    char key = Serial.read(); // Read the incoming byte from Arduino Nano
    Serial.print("Key Pressed: ");
    Serial.println(key); // Print the key pressed to the serial monitor
  }
}