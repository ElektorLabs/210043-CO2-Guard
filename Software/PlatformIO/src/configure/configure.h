//File to set all the Pins for the LEDs, Sensor, MQTT and Buzzer
#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__


#define use_wifi_manager

//LEDs
#define LEDred      12
#define LEDyellow   13
#define LEDgreen    15

//Sensor settings
#define RXPIN 4
#define TXPIN 5

#define BAUDRATE 9600

//Buzzer settings
#define BUZZERPIN       0
#define ALARMTHRESHOLD  1400
#define PWMVALUE        255
#define HYSTERESE       20

//CO2 Intervalls
#define LOWGREENVALUE   100
#define HIGHGREENVALUE  1000
#define LOWYELLOWVALUE  800
#define HIGHYELLOWVALUE 2000    
#define LOWREDVALUE     1400
#define HIGHREDVALUE    5000


#endif