//File to set all the Pins for the LEDs, Sensor, MQTT and Buzzer
#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__


#define use_wifi_manager

//LEDs
#define LEDred      12
#define LEDyellow   13
#define LEDgreen    15

//Sensor settings
#define RXPIN 5    
#define TXPIN 4
#define BAUDRATE 9600

//Buzzer settings
#define BUZZERPIN       0
#define ALARMTHRESHOLD  1400
#define PWMVALUE        255
#define HYSTERESE       20

//CO2 Intervalls
#define LOWGREENVALUE   400
#define HIGHGREENVALUE  1000
#define LOWYELLOWVALUE  1001
#define HIGHYELLOWVALUE 1400    
#define LOWREDVALUE     1401
#define HIGHREDVALUE    2000


#endif
