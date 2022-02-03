#include <Arduino.h>
#include "buzzer.h"


BUZZER::BUZZER (int pin_buzzer , int tone_value)
{
    pinMode(pin_buzzer,OUTPUT);
    this->tone_value = tone_value;
    this->pin_buzzer = pin_buzzer;
    analogWriteRange(1023);
    analogWriteFreq(1000);
    analogWrite(pin_buzzer , 0);
    
}



void BUZZER::setBuzzer()
{
    analogWrite(pin_buzzer , tone_value);
}

void BUZZER::clearBuzzer ()
{
    analogWrite(pin_buzzer , 0);
}

