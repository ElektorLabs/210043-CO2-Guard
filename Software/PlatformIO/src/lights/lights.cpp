     #include "Lights.h"
     #include "arduino.h"
     
    

    LIGHTS::LIGHTS (int pinr , int piny, int ping ){
        pinMode(pinr,OUTPUT);
        pinMode(piny,OUTPUT);
        pinMode(ping,OUTPUT);
        PINr = pinr;
        PINy = piny;
        PINg = ping;
        digitalWrite(pinr,LOW);
        digitalWrite(piny,LOW);
        digitalWrite(ping,LOW);

    }

    void LIGHTS::set(lights_color color)
        {
            
            switch(color)
            {
                case RED:
                    {
                    setRED();
                    break;
                    }
                case YELLOW:
                    {
                    setYELLOW();
                    }
                    break;
                case GREEN:
                    {
                    setGREEN(); 
                    }
                    break;
                default:{}
                break;                                        //für Debug-Informationen, Fehlerbehandlung, 
            }
        }

         void LIGHTS::clear(lights_color color)
        {
            
            switch(color)
            {
                case RED:
                {
                clearRED();
                break;
                }
                case YELLOW:
                {
                clearYELLOW(); 
                break;
                }
                case GREEN:
                clearGREEN(); 
                break;
                default:{}
                break;
            }
        }

        void LIGHTS::setRED()
        {
            digitalWrite(PINr, HIGH);            
        }
    
         void LIGHTS::setYELLOW()
        {       
            digitalWrite(PINy, HIGH);        
        }

        void LIGHTS::setGREEN()
        {           
            digitalWrite(PINg, HIGH);            
        }

        void LIGHTS::clearRED()
        {           
            digitalWrite(PINr, LOW);            
        }
        
        void LIGHTS::clearYELLOW()
        {           
            digitalWrite(PINy, LOW);            
        }

        void LIGHTS::clearGREEN()
        {           
            digitalWrite(PINg, LOW);            
        }

        
