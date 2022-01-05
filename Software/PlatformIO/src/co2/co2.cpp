#include <Arduino.h>
#include "lights/Lights.h"
#include "co2.h"



CO2_BORDERS::CO2_BORDERS (int LOWg, int HIGHg, int LOWy, int HIGHy, int LOWr, int HIGHr, int pinr, int piny, int ping) : LIGHTS(pinr,piny,ping)
{
    low_red_value = LOWr;
    high_red_value = HIGHr;
    low_yellow_value = LOWy;
    high_yellow_value = HIGHy;
    low_green_value = LOWg;
    high_green_value = HIGHg;
    setCO2_LIGHTS(0);
    
}

CO2_BORDERS::CO2_BORDERS (int pinr, int piny, int ping) : LIGHTS(pinr,piny,ping) 
{
    low_red_value = INT16_MAX;
    high_red_value = INT16_MAX;
    low_yellow_value = INT16_MAX;
    high_yellow_value = INT16_MAX;
    low_green_value = INT16_MAX;
    high_green_value = INT16_MAX;
    setCO2_LIGHTS(0);
}

void CO2_BORDERS::begin(int LOWg, int HIGHg, int LOWy, int HIGHy, int LOWr, int HIGHr)
{
    low_red_value = LOWr;
    high_red_value = HIGHr;
    low_yellow_value = LOWy;
    high_yellow_value = HIGHy;
    low_green_value = LOWg;
    high_green_value = HIGHg; 
}

void CO2_BORDERS::setCO2_LIGHTS (int co2_value)
{
    if ((low_red_value < co2_value) && (co2_value <= high_red_value))
    {
        set(LIGHTS::lights_color::RED);
    }
    else
    {
        clear(LIGHTS::lights_color::RED);
    }

    if ((low_yellow_value < co2_value) && (co2_value <= high_yellow_value))
    {
        set(LIGHTS::lights_color::YELLOW);
    }
    else
    {
        clear(LIGHTS::lights_color::YELLOW);
    }

    if ((low_green_value < co2_value) && (co2_value <= high_green_value))
    {
        set(LIGHTS::lights_color::GREEN);
    }
    else
    {
        clear(LIGHTS::lights_color::GREEN);
    }

}