
#include "buzzer/buzzer.h"

class ALARM : BUZZER

{
    private:


    public:
            int alarm_value;
            int buzz_pin;
            int tone_value;
            int co2_val;
            bool alarm;
        

            ALARM(int low_red_value, int hysterese, int tone, int pin_buzzer);
            
            void checkAlarm (int co2_Value);
           



};