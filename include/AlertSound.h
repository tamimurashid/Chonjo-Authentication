#ifndef ALERTSOUND_H
#define ALERTSOUND_H

class AlertSound {
public:
    AlertSound(int buzzle_pin, unsigned long SoundDelay);
    void Warning_alert();
    void Success_alert();
    void Error_alert();   // Added for error alert

private:
    void playTone(int frequency, unsigned long duration);  // Internal method for playing tones
    int buzzle_pin;
    unsigned long SoundDelay;
};

#endif
