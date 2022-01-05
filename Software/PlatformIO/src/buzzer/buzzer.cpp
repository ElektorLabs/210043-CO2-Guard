#include <Arduino.h>
#include "buzzer.h"


BUZZER::BUZZER (int pin_buzzer , int tone_value)
{
    pinMode(pin_buzzer,OUTPUT);
    this->tone_value = tone_value;
    this->pin_buzzer = pin_buzzer;
    digitalWrite(pin_buzzer,LOW);
    
}



void BUZZER::setBuzzer()
{
    analogWrite(pin_buzzer , tone_value);
}

void BUZZER::clearBuzzer ()
{
    analogWrite(pin_buzzer , 0);
}

