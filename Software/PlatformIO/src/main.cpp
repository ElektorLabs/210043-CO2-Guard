/* 
 * 
 */

#include <Arduino.h>          //damit es keine Probleme des "byte" aus der MHZ19.h gibt
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Hash.h>
#include <ESPAsyncUDP.h>
#include <SPI.h>
#include <LittleFS.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include <Ticker.h>
#include <TimeLib.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>

#include "timekeeping/ntp_client.h"
#include "timekeeping/timecore.h"
#include "network/network.h"
#include "lights/Lights.h"
#include "co2/co2.h"
#include "alarm/alarm.h"
#include "datastore/datastore.h"


Timecore timec;
NTP_Client NTPC;
Ticker TimeKeeper;
CO2_BORDERS BORDERS(LEDred,LEDyellow,LEDgreen);
ALARM BIEP(ALARMTHRESHOLD,HYSTERESE,PWMVALUE,BUZZERPIN);
MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RXPIN, TXPIN);                     // (Uno example) create device to MH-Z19 serial

void callback( void );
void _1SecondTick(void);

volatile uint8_t rtc_second_tick=0;
volatile uint8_t internal_1Hz_tick=0;
unsigned long getDataTimer = 0;


/**************************************************************************************************
 *    Function      : setup
 *    Description   : Get all components in ready state
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void setup()
{  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  #ifdef use_wifi_manager
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    
  #endif
  /* Next we setup the serial console with 115k2 8N1 */
  Serial.begin (115200);
  /* The next is to initilaize the datastore, here the eeprom emulation */
  datastoresetup();
  /* This is for the flash file system to access the webcontent */
  LittleFS.begin();
  /* Now we start with the config for the Timekeeping and sync */
  TimeKeeper.attach_ms(10, callback);
  initWiFi();
  yield();
  
  /* We read the Config from flash */
  timecoreconf_t cfg = read_timecoreconf();
  timec.SetConfig(cfg);
  yield();
  /* Next is the NTP Setup */
  NTPC.ReadSettings();
  NTPC.begin( &timec );
  NTPC.Sync();
  yield();

  
  BORDERS.begin(LOWGREENVALUE,HIGHGREENVALUE,LOWYELLOWVALUE,HIGHYELLOWVALUE,LOWREDVALUE,HIGHREDVALUE);    //Set CO2 limits 
 
  mySerial.begin(BAUDRATE);                               // Software Serial device to MH-Z19 serial start   
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

  myMHZ19.autoCalibration();  
  
  useThingSpeak();
  setupMQTT();

}



/**************************************************************************************************
 *    Function      : _1SecondTick
 *    Description   : Runs all fnctions inside once a second
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void _1SecondTick( void ){
     timec.RTC_Tick();
     NTPC.Tick();
   
}

/**************************************************************************************************
 *    Function      : callback
 *    Description   : internal timer callback, every 10ms called
 *    Input         : none 
 *    Output        : none
 *    Remarks       : If RTC is not avalible it is used as time base
 **************************************************************************************************/
void callback()
{
  static uint8_t prescaler_1s=0;
  prescaler_1s++;
  if(prescaler_1s>=100){
    prescaler_1s=0;
    internal_1Hz_tick++;
   /* if this is the time base we do the housekeeping */
    _1SecondTick();

  }
 
 
}

/**************************************************************************************************
 *    Function      : loop
 *    Description   : Superloop
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/



void loop()
{
 
  NetworkTask();
  timec.Task();
  NTPC.Task();

  if (millis() - getDataTimer >= 2000) //Reading every 2 seconds 
    {
       int CO2;
       float Temp;
       getDataTimer = millis();
       /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
       if below background CO2 levels or above range (useful to validate sensor). You can use the 
       usual documented command with getCO2(false) */
       
        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
        storeCO2(CO2);
               
        Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
        storeTemp(Temp);
       
        Serial.print("CO2 (ppm): ");                      
        Serial.print(CO2);     
        Serial.print(" Temperature (C): ");                  
        Serial.println(Temp);  
                                     
        
        /* This sets the date for thingspeak */
        postThinkspeak();
        /* This will do a mqtt publish if configured */
        publishmsg();

        BIEP.checkAlarm(CO2);                          
        BORDERS.setCO2_LIGHTS(CO2);
      
        
    }
        
}












 


