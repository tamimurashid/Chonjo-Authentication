#include "Indicator.h"

Indicator::Indicator(int ledPin1, int ledPin2, unsigned long delayWarning, unsigned long delaySuccess) 
    : warning_led(ledPin1), success_led(ledPin2), Delay_warning(delayWarning), Delay_success(delaySuccess) {
    
    pinMode(warning_led, OUTPUT);
    pinMode(success_led, OUTPUT);
}

void Indicator::error() {
    digitalWrite(warning_led, HIGH);
    delay(Delay_warning);
    digitalWrite(warning_led, LOW);
    delay(Delay_warning);
}

void Indicator::success() {
    digitalWrite(success_led, HIGH);
    delay(Delay_success);
    digitalWrite(success_led, LOW);
    delay(Delay_success);
}
