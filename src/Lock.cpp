#include"Lock.h"
#include<Arduino.h>


Lock::Lock(int trigger, unsigned long backresponse): trigger(backresponse){

    pinMode(trigger, OUTPUT);

}
void Lock:: Open(){
    digitalWrite(trigger, HIGH);
    delay(backresponse);
    digitalWrite(trigger, LOW);

}