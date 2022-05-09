#include "Arduino.h"


class CO2_BORDERS : LIGHTS
{
    private:

    //lights intervals
 
    public:
    int co2_value;
    int low_red_value;
    int high_red_value;
    int low_yellow_value;
    int high_yellow_value;
    int low_green_value;
    int high_green_value;

    CO2_BORDERS(int LOWr, int HIGHr, int LOWy, int HIGHy, int LOWg, int HIGHg, int pinr, int piny, int ping);
    CO2_BORDERS (int pinr, int piny, int ping);

    void setCO2_LIGHTS(int co2_value);

    void begin(int LOWg, int HIGHg, int LOWy, int HIGHy, int LOWr, int HIGHr);

};
