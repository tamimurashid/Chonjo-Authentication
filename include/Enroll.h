#ifndef ENROLL_H
#define ENROLL_H

#include <Arduino.h>

class Enroll {
public:
    Enroll();
    void begin();
    void enrollUser();
    void handleSerial();
private:
    void sendToNodeMCU(const String& data);
    String readRFID(); // Function to read data from the RFID sensor
};

#endif // ENROLL_H
