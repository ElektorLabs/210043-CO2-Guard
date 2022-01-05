
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <Hash.h>

#include "../timekeeping/timecore.h"
#include "../timekeeping/ntp_client.h"
#include "../datastore/datastore.h"



#include "webfunctions.h"

extern Timecore timec;
extern NTP_Client NTPC;
extern void displayrefesh( void );
extern void sendData(String data);
extern ESP8266WebServer * server;
extern void reloadMQTTSettings(void);
extern void realoadTSSettings(void);


/**************************************************************************************************
*    Function      : response_settings
*    Description   : Sends the timesettings as json 
*    Input         : non
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void response_settings(){
String response="";  

StaticJsonDocument<350> root;
char strbuffer[129];

  memset(strbuffer,0,129);
  datum_t d = timec.GetLocalTimeDate();
  snprintf(strbuffer,64,"%02d:%02d:%02d",d.hour,d.minute,d.second);
  strbuffer;

  root["time"] = strbuffer;
 
  memset(strbuffer,0,129);
  snprintf(strbuffer,64,"%04d-%02d-%02d",d.year,d.month,d.day);
  root["date"] = strbuffer;

  memset(strbuffer,0,129);
  snprintf(strbuffer,129,"%s",NTPC.GetServerName());
  root["ntpname"] = strbuffer;
  root["tzidx"] = (int32)timec.GetTimeZone();
  root["ntpena"] = NTPC.GetNTPSyncEna();
  root["ntp_update_span"]=NTPC.GetSyncInterval();
  root["zoneoverride"]=timec.GetTimeZoneManual();;
  root["gmtoffset"]=timec.GetGMT_Offset();;
  root["dlsdis"]=!timec.GetAutomacitDLS();
  root["dlsmanena"]=timec.GetManualDLSEna();
  uint32_t idx = timec.GetDLS_Offset();
  root["dlsmanidx"]=idx;
  serializeJson(root,response);

  sendData(response);
}


/**************************************************************************************************
*    Function      : settime_update
*    Description   : Parses POST for new local time
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void settime_update( ){ /* needs to process date and time */
  datum_t d;
  d.year=2000;
  d.month=1;
  d.day=1;
  d.hour=0;
  d.minute=0;
  d.second=0;

  bool time_found=false;
  bool date_found=false;
  
  if( ! server->hasArg("date") || server->arg("date") == NULL ) { // If the POST request doesn't have username and password data
    /* we are missong something here */
  } else {
   
    Serial.printf("found date: %s\n\r",server->arg("date").c_str());
    uint8_t d_len = server->arg("date").length();
    Serial.printf("datelen: %i\n\r",d_len);
    if(server->arg("date").length()!=10){
      Serial.println("date len failed");
    } else {   
      String year=server->arg("date").substring(0,4);
      String month=server->arg("date").substring(5,7);
      String day=server->arg("date").substring(8,10);
      d.year = year.toInt();
      d.month = month.toInt();
      d.day = day.toInt();
      date_found=true;
    }   
  }

  if( ! server->hasArg("time") || server->arg("time") == NULL ) { // If the POST request doesn't have username and password data
    
  } else {
    if(server->arg("time").length()!=8){
      Serial.println("time len failed");
    } else {
    
      String hour=server->arg("time").substring(0,2);
      String minute=server->arg("time").substring(3,5);
      String second=server->arg("time").substring(6,8);
      d.hour = hour.toInt();
      d.minute = minute.toInt();
      d.second = second.toInt();     
      time_found=true;
    }
     
  } 
  if( (time_found==true) && ( date_found==true) ){
    Serial.printf("Date: %i, %i, %i ", d.year , d.month, d.day );
    Serial.printf("Time: %i, %i, %i ", d.hour , d.minute, d.second );
    timec.SetLocalTime(d);
  }
  
  server->send(200);   
 
 }


/**************************************************************************************************
*    Function      : ntp_settings_update
*    Description   : Parses POST for new ntp settings
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void ntp_settings_update( ){ /* needs to process NTP_ON, NTPServerName and NTP_UPDTAE_SPAN */

  if( ! server->hasArg("NTP_ON") || server->arg("NTP_ON") == NULL ) { // If the POST request doesn't have username and password data
    NTPC.SetNTPSyncEna(false);  
  } else {
    NTPC.SetNTPSyncEna(true);  
  }

  if( ! server->hasArg("NTPServerName") || server->arg("NTPServerName") == NULL ) { // If the POST request doesn't have username and password data
      
  } else {
     NTPC.SetServerName( server->arg("NTPServerName") );
  }

  if( ! server->hasArg("ntp_update_delta") || server->arg("ntp_update_delta") == NULL ) { // If the POST request doesn't have username and password data
     
  } else {
    NTPC.SetSyncInterval( server->arg("ntp_update_delta").toInt() );
  }
  NTPC.SaveSettings();
  server->send(200);   
  
 }

/**************************************************************************************************
*    Function      : timezone_update
*    Description   : Parses POST for new timezone settings
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/  
void timezone_update( ){ /*needs to handel timezoneid */
  if( ! server->hasArg("timezoneid") || server->arg("timezoneid") == NULL ) { // If the POST request doesn't have username and password data
    /* we are missong something here */
  } else {
   
    Serial.printf("New TimeZoneID: %s\n\r",server->arg("timezoneid").c_str());
    uint32_t timezoneid = server->arg("timezoneid").toInt();
    timec.SetTimeZone( (TIMEZONES_NAMES_t)timezoneid );   
  }
  timec.SaveConfig();
  server->send(200);    

 }

 
/**************************************************************************************************
*    Function      : timezone_overrides_update
*    Description   : Parses POST for new timzone overrides
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/  
 void timezone_overrides_update( ){ /* needs to handle DLSOverrid,  ManualDLS, dls_offset, ZONE_OVERRRIDE and GMT_OFFSET */

  bool DLSOverrid=false;
  bool ManualDLS = false;
  bool ZONE_OVERRRIDE = false;
  int32_t gmt_offset = 0;
  DLTS_OFFSET_t dls_offsetidx = DLST_OFFSET_0;
  if( ! server->hasArg("dlsdis") || server->arg("dlsdis") == NULL ) { // If the POST request doesn't have username and password data
      /* we are missing something here */
  } else {
    DLSOverrid=true;  
  }

  if( ! server->hasArg("dlsmanena") || server->arg("dlsmanena") == NULL ) { // If the POST request doesn't have username and password data
      /* we are missing something here */
  } else {
    ManualDLS=true;  
  }

  if( ! server->hasArg("ZONE_OVERRRIDE") || server->arg("ZONE_OVERRRIDE") == NULL ) { // If the POST request doesn't have username and password data
      /* we are missing something here */
  } else {
    ZONE_OVERRRIDE=true;  
  }

  if( ! server->hasArg("gmtoffset") || server->arg("gmtoffset") == NULL ) { // If the POST request doesn't have username and password data
      /* we are missing something here */
  } else {
    gmt_offset = server->arg("gmtoffset").toInt();
  }

  if( ! server->hasArg("dlsmanidx") || server->arg("dlsmanidx") == NULL ) { // If the POST request doesn't have username and password data
      /* we are missing something here */
  } else {
    dls_offsetidx = (DLTS_OFFSET_t) server->arg("dlsmanidx").toInt();
  }
  timec.SetGMT_Offset(gmt_offset);
  timec.SetDLS_Offset( (DLTS_OFFSET_t)(dls_offsetidx) );
  timec.SetAutomaticDLS(!DLSOverrid);
  timec.SetManualDLSEna(ManualDLS);
  timec.SetTimeZoneManual(ZONE_OVERRRIDE);

 
  timec.SaveConfig();
  server->send(200);    

  
 }

/**************************************************************************************************
*    Function      : update_notes
*    Description   : Parses POST for new notes
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void update_notes(){
  char data[501]={0,};
  if( ! server->hasArg("notes") || server->arg("notes") == NULL ) { // If the POST request doesn't have username and password data
    /* we are missing something here */
  } else {
   
    Serial.printf("New Notes: %s\n\r",server->arg("notes").c_str());
    /* direct commit */
    uint32_t str_size = server->arg("notes").length();
    if(str_size<501){
      strncpy((char*)data,server->arg("notes").c_str(),501);
      eepwrite_notes((uint8_t*)data,501);
    } else {
      Serial.println("Note > 512 char");
    }
  }

  server->send(200);    
}

/**************************************************************************************************
*    Function      : read_notes
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : Retunrs the notes as plain text
**************************************************************************************************/ 
void read_notes(){
  char data[501]={0,};
  eepread_notes((uint8_t*)data,501);
  sendData(data);    
}


void mqttsettings_update( ){
 
  mqttsettings_t Data;
  //loadMQTTSettings(&Data); 
  Data = eepread_mqttsettings();
  
  if( ! server->hasArg("MQTT_USER") || server->arg("MQTT_USER") == NULL ) { 
    /* we are missong something here */
  } else { 
    String value = server->arg("MQTT_USER");
    bzero(Data.mqttusername,sizeof(Data.mqttusername));
    strncpy(Data.mqttusername, value.c_str(),128);
  }

  if( ! server->hasArg("MQTT_PASS") || server->arg("MQTT_PASS") == NULL ) { 
    /* we are missong something here */
  } else { 
    String value = server->arg("MQTT_PASS");
    bzero(Data.mqttpassword,sizeof(Data.mqttpassword));
    strncpy(Data.mqttpassword, value.c_str(),128);
  }

  if( ! server->hasArg("MQTT_SERVER") || server->arg("MQTT_SERVER") == NULL ) { 
    /* we are missong something here */
  } else { 
    String value = server->arg("MQTT_SERVER");
    bzero(Data.mqttservername,sizeof(Data.mqttservername));
    strncpy(Data.mqttservername, value.c_str(),128);
  }

  if( ! server->hasArg("MQTT_HOST") || server->arg("MQTT_HOST") == NULL ) { 
    /* we are missong something here */
  } else { 
    String value = server->arg("MQTT_HOST");
    bzero(Data.mqtthostname,sizeof(Data.mqtthostname));
    strncpy(Data.mqtthostname, value.c_str(),64);
  }

  if( ! server->hasArg("MQTT_PORT") || server->arg("MQTT_PORT") == NULL ) { 
    /* we are missong something here */
  } else { 
    int32_t value = server->arg("MQTT_PORT").toInt();
    if( (value>=0) && ( value<=UINT16_MAX ) ){
      Data.mqttserverport = value;
    }
  }
  
  if( ! server->hasArg("MQTT_TOPIC") || server->arg("MQTT_TOPIC") == NULL ) { 
    /* we are missong something here */
  } else { 
    String value = server->arg("MQTT_TOPIC");
    bzero(Data.mqtttopic,sizeof(Data.mqtttopic));
    strncpy(Data.mqtttopic, value.c_str(),500);
  }

  if( ! server->hasArg("MQTT_ENA") || server->arg("MQTT_ENA") == NULL ) { 
    /* we are missing something here */
  } else { 
    bool value = false;
    if(server->arg("MQTT_ENA")=="true"){
      value = true;
    }
    Data.enable = value;
  }
  

  if( ! server->hasArg("MQTT_TXINTERVALL") || server->arg("MQTT_TXINTERVALL") == NULL ) { 
    /* we are missong something here */
  } else { 
    uint32_t value = server->arg("MQTT_TXINTERVALL").toInt();
    Data.mqtttxintervall = value;
  }
  /* write data to the eeprom */
  eepwrite_mqttsettings(Data);   
  reloadMQTTSettings();
  server->send(200); 

}

void read_mqttsetting(){
  String response =""; 
  mqttsettings_t Data;
  Data = eepread_mqttsettings();
  //loadMQTTSettings(&Data);  
  DynamicJsonDocument  root(2000); 
  
 
  root["mqttena"]= (bool)(Data.enable);
  root["mqttserver"] = String(Data.mqttservername);
  root["mqtthost"] = String(Data.mqtthostname);
  root["mqttport"] = Data.mqttserverport;
  root["mqttuser"] = String(Data.mqttusername);
  root["mqtttopic"] = String(Data.mqtttopic);
  root["mqtttxintervall"] = Data.mqtttxintervall;
  if(Data.mqttpassword[0]!=0){
    root["mqttpass"] = "********";
  } else {
    root["mqttpass"] ="";
  }

  serializeJson(root,response);
  sendData(response);

}

/**************************************************************************************************
 *    Function      : GetThinkspeakSeverSettings
 *    Description   : response with settings to client
 *    Input         : void
 *    Output        : none
 *    Remarks       : None
 **************************************************************************************************/
void GetThinkspeakSeverSettings(){
    if(server == nullptr){
        return;
    }
    thinkspeaksettings_t settings = eepread_thinkspeaksettings();
    String response ="";
    DynamicJsonDocument root(2048);
    root["APIKEY"]= settings.apikey;
    root["THINKSPEAK_ENA"] =(bool)(settings.ena);
    root["THINKSPEAK_TXINTERVALL"] = settings.uploadinterval;
    root["THINKSPEAK_CHID"] = settings.channelid;
    serializeJson(root, response);
    server->send(200, "text/plain", response);

}





/**************************************************************************************************
 *    Function      : SetThinkspeakServerSettings
 *    Description   : Process POST with new Settings
 *    Input         : void
 *    Output        : none
 *    Remarks       : None
 **************************************************************************************************/
void SetThinkspeakServerSettings( void ){
    if(server == nullptr){
        return;
    }

    thinkspeaksettings_t settings = eepread_thinkspeaksettings();
    //We read everything we have from the post command....
    if( ! server->hasArg("APIKEY") || server->arg("APIKEY") == NULL ) { 
    /* we are missong something here */
    } else { 
        String value = server->arg("APIKEY");
        strncpy(settings.apikey,value.c_str(),sizeof(settings.apikey));
        #ifdef DEBUG_SERIAL
         Serial.print("New Thinkspeak Key:");
         Serial.println(value);
        #endif
       
    }

    if( ! server->hasArg("THINKSPEAK_ENA") || server->arg("THINKSPEAK_ENA") == NULL ) { 
    /* we are missing something here */
    } else { 
        bool value = false;
        if(server->arg("THINKSPEAK_ENA")=="true"){
            value = true;
            #ifdef DEBUG_SERIAL
             Serial.println("Thinkspeak enable");
            #endif
        } else {
            #ifdef DEBUG_SERIAL
             Serial.println("Thinkspeak disable");
            #endif
        }
        settings.ena=value;
    }

    if( ! server->hasArg("THINKSPEAK_TXINTERVALL") || server->arg("THINKSPEAK_TXINTERVALL") == NULL ) { 
            /* we are missong something here */
    } else { 
        uint32_t value = server->arg("THINKSPEAK_TXINTERVALL").toInt();
        if( (value >=0 ) && ( value < UINT16_MAX )){
            settings.uploadinterval=value;
            #ifdef DEBUG_SERIAL
             Serial.print("New Thinkspeak TX interva:");
             Serial.println(value);
            #endif
        }
    }

    if( ! server->hasArg("THINKSPEAK_CHID") || server->arg("THINKSPEAK_CHID") == NULL ) { 
            /* we are missong something here */
    } else { 
        uint32_t value = strtoul(server->arg("THINKSPEAK_ID").c_str(),NULL,0);
        if( (value >=0 ) && ( value < UINT16_MAX )){
            settings.channelid=value;
            #ifdef DEBUG_SERIAL
             Serial.print("New Thinkspeak CHannel ID:");
             Serial.println(value);
            #endif
        }
    }

    eepwrite_thinkspeaksettings(settings);
    //Set a flag that the settings have been changed...
    realoadTSSettings();
    server->send(200); 
}





