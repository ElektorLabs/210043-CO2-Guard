#include "Arduino.h"
#include "configure/configure.h"


class LIGHTS
{
    //Eigenschaft
    private:
    int PINr;              //Pin-Eigenschaft ist nur die Pin Nummer
    int PINy;
    int PINg;
    
    //Methode
 
    public:

    enum lights_color {RED, YELLOW, GREEN};


    //Constructors
    
    LIGHTS (int pinr, int piny, int ping);

    void set(lights_color color);
    void clear(lights_color color);
    void setRED();
    void setYELLOW();
    void setGREEN();
    void clearRED();
    void clearYELLOW();
    void clearGREEN();
    

   
};

