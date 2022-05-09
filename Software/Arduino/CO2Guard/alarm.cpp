#include "alarm.h"
#include "Lights.h"
#include "co2.h"
//#include "buzzer.h"


//Hysterese 20ppm, um Schwingen zu verhindern

ALARM :: ALARM(int low_red_value, int hysterese, int tone, int pin_buzzer)  : BUZZER(pin_buzzer ,tone)
    {
        clearBuzzer();
        alarm_value = low_red_value;
    }

void ALARM::checkAlarm (int co2_val)
    {
        if ( ((co2_val) > alarm_value + 20) && alarm == false )
        {
            {
                alarm = true;
                setBuzzer();
            }
        }
        else 
        {
            
                if ( ((co2_val) < alarm_value - 20) && alarm == true )
                {               
                clearBuzzer();
                alarm = false;
                }
        }   
       
    }    

void ALARM::testAlarm(bool on){
    if(true==on){
         setBuzzer();
    } else {
         clearBuzzer();
    }
}
