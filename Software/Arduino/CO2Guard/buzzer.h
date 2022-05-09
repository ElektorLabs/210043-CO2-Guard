class BUZZER 
{
private:
        
        int pin_buzzer;
        int tone_value;
        
public:
        
        BUZZER(int pin_buzzer, int tone_value);
        
        void setBuzzer ();
        void clearBuzzer ();
        
};
