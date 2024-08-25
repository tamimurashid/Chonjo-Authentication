#ifndef INDICATOR_H
#define INDICATOR_H

#include <Arduino.h>

class Indicator {
public:
    Indicator(int ledPin1, int ledpin2, unsigned long delayWarning, unsigned long delaySuccess);
    void error();
    void success();

private:
    int warning_led;
    int success_led;
    unsigned long Delay_warning;
    unsigned long Delay_success;
};

#endif
