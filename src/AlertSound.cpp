#include "AlertSound.h"
#include <Arduino.h>  // Include Arduino functions like tone() and delay()

// Constructor: Initializes the buzzer pin and sound delay
AlertSound::AlertSound(int buzzle_pin, unsigned long SoundDelay)
    : buzzle_pin(buzzle_pin), SoundDelay(SoundDelay) {
    pinMode(buzzle_pin, OUTPUT);
}

// Private method to play a tone
void AlertSound::playTone(int frequency, unsigned long duration) {
    tone(buzzle_pin, frequency, duration);
    delay(duration + SoundDelay);  // Add a delay between tones
}

// Method to play the warning tone
void AlertSound::Warning_alert() {
    playTone(1000, 500);  // Example frequency and duration for warning
}

// Method to play the success tone
void AlertSound::Success_alert() {
    playTone(2000, 300);  // Example frequency and duration for success
}

// Method to play the error tone
void AlertSound::Error_alert() {
    playTone(500, 1000);  // Example frequency and duration for error
}
