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
#define ALARMTHRESHOLD  1420    // 1400
#define PWMVALUE        255
#define HYSTERESE       20

//CO2 Intervals
#define LOWGREENVALUE   400
#define HIGHGREENVALUE  1000
#define LOWYELLOWVALUE  800     // 1001
#define HIGHYELLOWVALUE 2000    // 1400
#define LOWREDVALUE     1400    // 1401
#define HIGHREDVALUE    4000    // 2000


#endif
