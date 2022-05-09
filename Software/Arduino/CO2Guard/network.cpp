#include "datastore.h"
#include "webfunctions.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <LittleFS.h>
#include "network.h"
#include <MHZ19.h>
#include <sstream>
#include <string>
//#include <secrets.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>
#include "configure.h"
#ifdef use_wifi_manager
  #include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#endif

String APSSID = "CO2 Meter";

#ifdef use_wifi_manager
  WiFiManager wm;
#endif

//We load the settings from eeprom
mqttsettings_t mqtt_settings ;

MHZ19 sensorMHZ19;
String ssid ="";
String pass="";
IPAddress ip;
bool IsAP=false;
int co2=400;
float temp=0;
WiFiClient client;

ESP8266WebServer * server =NULL;


/**************************************************************************************************
 *    Function      : MQTT
 *    Description   :  
 *    Input         : 
 *    Output        : 
 *    Remarks       : none
 **************************************************************************************************/
//const char* ssid = "........";
//const char* password = "........";
//const char* mqtt_server = MQTT_server;
bool checkMQTT = false;

//WiFiClient espClient;
PubSubClient clientPubSub(client);
unsigned long lastMsg = 0;
unsigned long lastMsgTS = 0;
#define MSG_BUFFER_SIZE	(50)
//char msg[MSG_BUFFER_SIZE];
int value = 0;

void reconnect() {
  // Loop until we're reconnected
  if (!clientPubSub.connected()) {
    if (checkMQTT == false){
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
      if (clientPubSub.connect(clientId.c_str())) {
      Serial.println("connected");
      checkMQTT = true;
      }
    } 
    else {
      if (checkMQTT == false){
      Serial.print("failed, rc=");
      Serial.print(clientPubSub.state());
      Serial.println(" try again in 5 seconds");
      checkMQTT = false;

      // Wait 5 seconds before retrying
      delay(5000);
      }
    }
  }
}

static bool mqtt_reload=false;

void setupMQTT() {
  //We load the settings from eeprom
  if(clientPubSub.connected()==true){
    clientPubSub.disconnect();
  }
  mqtt_settings = eepread_mqttsettings( );
  clientPubSub.setServer(mqtt_settings.mqttservername, mqtt_settings.mqttserverport);
  mqtt_reload=false;
}


void reloadMQTTSettings(){
  mqtt_reload=true;
}


void publishmsg() {
  if(true==mqtt_reload){
    setupMQTT();
  }
  
  if(mqtt_settings.enable==false){
    if(true==clientPubSub.connected()){
       clientPubSub.disconnect();
    }
    return;
  }

  if (!clientPubSub.connected()) {
    reconnect();
  }

//Creating Json document and serialize it into string
StaticJsonDocument<64> doc2;
doc2["CO2Value"] = co2;
doc2["Temp"] = temp;
std::string JsonPayload;
serializeJson(doc2, JsonPayload);

  unsigned long now = millis();
  if (now - lastMsg > (mqtt_settings.mqtttxinterval*1000)) {
    lastMsg = now;
    ++value;
    //snprintf (msg, MSG_BUFFER_SIZE, "CO2 Value #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(JsonPayload.c_str());

    //Puts the Json document as Payload for the MQTT publish
    clientPubSub.publish(mqtt_settings.mqtttopic, JsonPayload.c_str());
  }
}


/**************************************************************************************************
 *    Function      : ThingSpeak
 *    Description   :  
 *    Input         : 
 *    Output        : 
 *    Remarks       : none
 **************************************************************************************************/
thingspeaksettings_t tssettings;
bool reloadtssettings=false;

void load_ts_settings(){
    tssettings = eepread_thingspeaksettings();
    reloadtssettings=false;
}

void useThingSpeak()
{
  ThingSpeak.begin(client);
  load_ts_settings();
}

void setCO2field()
{
  ThingSpeak.setField(1,co2);
}

void writeFields()
{
  //ChannelID , parameter one, is not used to we can set it to zero
  ThingSpeak.writeFields(0 ,tssettings.apikey);
}

void setTEMPfield()
{
  ThingSpeak.setField(2,temp);
}


void postThingSpeak( void ){
  if(true==reloadtssettings){
    load_ts_settings();
  }

  if( false == tssettings.ena ){
    return;
  }

  unsigned long now = millis();
  if (now - lastMsgTS > (mqtt_settings.mqtttxinterval*1000)) {
    lastMsgTS = now;
    Serial.println("Update ThingSpeak");
    setCO2field();
    setTEMPfield();
    writeFields();
  }

}

void realoadTSSettings(){
  reloadtssettings=true;
}


/**************************************************************************************************
 *    Function      : SSIDList
 *    Description   : Returns the SSID List 
 *    Input         : String separator 
 *    Output        : String
 *    Remarks       : none
 **************************************************************************************************/
String SSIDList(String separator ) {
  Serial.println("Scanning networks");
  String ssidList;
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    Serial.println(String(i) + ": " + ssid);
    if (ssidList.indexOf(ssid) != -1) {
      Serial.println("SSID already in list");
    }
    else {
      if (ssidList != "")
        ssidList += separator;
      ssidList += ssid;
    }
  }
  return ssidList;
}


/**************************************************************************************************
 *    Function      : getSSIDList
 *    Description   : Sends a SSID List to the client 
 *    Input         : none
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void getSSIDList() {
  Serial.println("SSID list requested");
  sendData(SSIDList());
}


/**************************************************************************************************
 *    Function      : setWiFiSettings
 *    Description   : Applies the WiFi settings 
 *    Input         : none
 *    Output        : none
 *    Remarks       : Store the wifi settings configured on the webpage and restart the esp to connect to this network
 **************************************************************************************************/
void setWiFiSettings() {
  credentials_t c;
  Serial.println("WiFi settings received");
  ssid = server->arg("ssid");
  pass = server->arg("pass");

  #ifdef use_wifi_manager
    if( (ssid=="") && (pass=="") ){
      wm.resetSettings();
      Serial.println("Reset WM settings");
    }
    server->send(200);   
  #else
  String response = "Attempting to connect to '" + ssid + "'. The WiFi module restarts and tries to connect to the network.";
  sendData(response);
  Serial.println("Saving network credentials and restart.");
  strncpy((char*)(c.ssid),(char*)(ssid.c_str()),128);
  strncpy((char*)(c.pass),(char*)(pass.c_str()),128);
  Serial.printf("write ssid:%s ,pass:%s \n\r",c.ssid,c.pass);
  write_credentials(c);
  
  c = read_credentials();
  Serial.printf("read ssid:%s ,pass:%s \n\r",c.ssid,c.pass);
  /* if we do this we end up in flashloader */
  WiFi.softAPdisconnect(true);
  delay(2000);
  pinMode(D0,INPUT_PULLUP);
  ESP.restart();
  #endif
}

//send the wifi settings to the connected client of the webserver
void getWiFiSettings() {
  Serial.println("WiFi settings requested");
  String response;
  #ifdef use_wifi_manager
    ssid = WiFi.SSID();
    response += ssid + ",";
  #else
    response += ssid + ",";
    response += SSIDList(";");
  #endif
  sendData(response);
}

void storeCO2(int co2val){

  co2 = co2val;

}

void storeTemp(float t ){
 temp=t;
}


/**************************************************************************************************
 *    Function      : getCO2value
 *    Description   : none 
 *    Input         : none
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void getCO2value() {

Serial.println("getco2value");
String output;
/*char data[128];
snprintf(data, sizeof(data),"%i",co2);
sendData(data);*/
StaticJsonDocument<32> doc;

doc["CO2Value"] = co2;

serializeJson(doc, output);
sendData(output);

}


/**************************************************************************************************
 *    Function      : getCO2value
 *    Description   : none 
 *    Input         : none
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void getTempvalue() {

Serial.println("getTempvalue");
String output;
/*char data[128];
snprintf(data, sizeof(data),"%i",co2);
sendData(data);*/
StaticJsonDocument<32> doc;

doc["Temp"] = temp;

serializeJson(doc, output);
sendData(output);

}

void getData(){
  Serial.println("getTempvalue");
  String output;
  StaticJsonDocument<64> doc;
  doc["CO2Value"] = co2;
  doc["Temp"] = temp;

  serializeJson(doc, output);
  sendData(output);
}


/**************************************************************************************************
 *    Function      : setWiFiSettings
 *    Description   : Applies the WiFi settings 
 *    Input         : none
 *    Output        : none
 *    Remarks       : restart the esp as requested on the webpage
 **************************************************************************************************/
void restart() {
  sendData("The ESP will restart and you will be disconnected from the '" + APSSID + " " + WiFi.macAddress() + "' network.");
  delay(1000);
  pinMode(D0,INPUT_PULLUP);
  ESP.restart();
}


/**************************************************************************************************
 *    Function      : getContentType
 *    Description   : Gets the contenttype depending on a filename 
 *    Input         : String
 *    Output        : String
 *    Remarks       : none
 **************************************************************************************************/
String getContentType(String filename) {
  if (server->hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


/**************************************************************************************************
 *    Function      : sendFile
 *    Description   : Sends a requestedfile from SPIFFS /LittleFS
 *    Input         : none 
 *    Output        : none
 *    Remarks       : send a file from the SPIFFS to the connected client of the webserver
 **************************************************************************************************/
void sendFile() {
  
  String path = server->uri();
  Serial.println("Got request for: " + path);
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  Serial.println("Content type: " + contentType);
  if (LittleFS.exists(path)) {
    Serial.println("File " + path + " found");
    File file = LittleFS.open(path, "r");
    server->streamFile(file, contentType);
    file.close();
  }
  else {
    Serial.println("File '" + path + "' doesn't exist");
    server->send(404, "text/plain", "The requested file doesn't exist");
  }
  
}


/**************************************************************************************************
 *    Function      : sendFile
 *    Description   : Sends a requestedfile from SPIFFS
 *    Input         : String 
 *    Output        : none
 *    Remarks       : send data to the connected client of the webserver
 **************************************************************************************************/
void sendData(String data) {
  server->send(200, "text/plain", data);
}


/**************************************************************************************************
 *    Function      : initWiFi
 *    Description   : initializes the WiFi
 *    Input         : String 
 *    Output        : none
 *    Remarks       : initialize wifi by connecting to a wifi network or creating an accesspoint
 **************************************************************************************************/
void initWiFi() {

#ifdef use_wifi_manager
  
  wm.setConfigPortalBlocking(false);
    if(wm.autoConnect(APSSID.c_str())){
          Serial.println("connected with stored credentials");
          configureServer();

           if (!MDNS.begin("co2guard")) {             // Start the mDNS responder for esp8266.local
                Serial.println("Error setting up MDNS responder!");
            } else {
                Serial.println("mDNS responder started");
                MDNS.addService("http", "tcp", 80);
            }
    } else {
          Serial.println("connection failed, starting portal");
    }
  #else
  
      credentials_t c =  read_credentials();
      Serial.print("WiFi: ");
    
      
      if ( 0==c.ssid[0]) {
        Serial.println("AP");
        configureSoftAP();
      
      }
      else {
        Serial.println("STA");
      
    
          ssid=String(c.ssid);
          pass=String(c.pass);
          if(true==connectWiFi()){
            configureServer();
          } else {
            configureSoftAP();
          }  
      }
  #endif
}


/**************************************************************************************************
 *    Function      : connectWiFi
 *    Description   : trys to establish a WiFi connection
 *    Input         : none 
 *    Output        : bool
 *    Remarks       : connect the esp to a wifi network, retuns false if failed
 **************************************************************************************************/
bool connectWiFi() {
  
  if (ssid == "") {
    Serial.println("SSID unknown");

    return false;
  }
  WiFi.mode(WIFI_STA);

  Serial.println("Attempting to connect to " + ssid + ", pass: " + pass);
 
   
  WiFi.begin(( char*)ssid.c_str(), ( char*)pass.c_str());
  for (int timeout = 0; timeout < 15; timeout++) { //max 15 seconds
    int status = WiFi.status();
    if ((status == WL_CONNECTED)  || (status == WL_NO_SSID_AVAIL) || (status == WL_CONNECT_FAILED))
      break;
    Serial.print(".");
   
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to " + ssid);
    Serial.println("Second attempt " + ssid);
    WiFi.disconnect(true);
    WiFi.begin(( char*)ssid.c_str(), ( char*)pass.c_str());
    for (int timeout = 0; timeout < 15; timeout++) { //max 15 seconds
      int status = WiFi.status();
      if ((status == WL_CONNECTED)  || (status == WL_NO_SSID_AVAIL) || (status == WL_CONNECT_FAILED))
        break;
      Serial.print(".");
     
      delay(1000);
    }
  
  }
  Serial.println();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to " + ssid);
    Serial.println("WiFi status: " + WiFiStatusToString());  
    WiFi.disconnect();

    return false;
  }
  Serial.println("Connected to " + ssid);
  Serial.print("Local IP: ");
  ip = WiFi.localIP();
  Serial.println(ip);  
   
  return true;
}


/**************************************************************************************************
 *    Function      : configureSoftAP
 *    Description   : Configures the ESP as SoftAP
 *    Input         : none 
 *    Output        : none
 *    Remarks       : configure the access point of the esp
 **************************************************************************************************/
void configureSoftAP() {
 IsAP=true;

/**
 * Get the station interface MAC address.
 * @return String mac
 */

  APSSID = "CO2 Meter " + WiFi.macAddress();
  Serial.println("Configuring AP: " + String(APSSID));
 
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(APSSID.c_str(), NULL, 1, 0, 1);
  delay(500); 
  ip = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(ip);
 
  if (!MDNS.begin("co2meter")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  configureServer();
}


/**************************************************************************************************
 *    Function      : configureServer
 *    Description   : Setup for the Webserver
 *    Input         : none 
 *    Output        : none
 *    Remarks       : initialize the webserver on port 80
 **************************************************************************************************/
void configureServer() {
  server = new ESP8266WebServer (80);
  server->on("/setWiFiSettings", HTTP_GET, setWiFiSettings);
  server->on("/getWiFiSettings", HTTP_GET, getWiFiSettings);
  server->on("/getSSIDList", HTTP_GET, getSSIDList);
  server->on("/restart", HTTP_GET, restart);
  server->on("/timesettings", HTTP_GET, response_settings);
  server->on("/settime.dat", HTTP_POST, settime_update); /* needs to process date and time */
  server->on("/ntp.dat",HTTP_POST,ntp_settings_update); /* needs to process NTP_ON, NTPServerName and NTP_UPDTAE_SPAN */
  server->on("/timezone.dat",timezone_update); /*needs to handel timezoneid */
  server->on("/overrides.dat",timezone_overrides_update); /* needs to handle DLSOverrid,  ManualDLS, dls_offset, ZONE_OVERRRIDE and GMT_OFFSET */
  server->on("/notes.dat",HTTP_GET,read_notes);
  server->on("/notes.dat",HTTP_POST,update_notes);
  server->on("/co2value", HTTP_GET, getCO2value);
  server->on("/temp", HTTP_GET, getTempvalue);
  server->on("/data", HTTP_GET, getData);
  server->on("/mqtt/settings",HTTP_POST,mqttsettings_update);
  server->on("/mqtt/settings",HTTP_GET,read_mqttsetting);
  server->on("/thingspeak/settings.dat", HTTP_POST, SetThingspeakServerSettings);
  server->on("/thingspeak/settings.json", HTTP_GET, GetThingspeakSeverSettings);
  server->onNotFound(sendFile); //handle everything except the above things
  server->begin();
  Serial.println("Webserver started");
}


/**************************************************************************************************
 *    Function      : WiFiStatusToString
 *    Description   : Gives a string representing the WiFi status
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
String WiFiStatusToString() {
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:     return "IDLE"; break;
    case WL_NO_SSID_AVAIL:   return "NO SSID AVAIL"; break;
    case WL_SCAN_COMPLETED:  return "SCAN COMPLETED"; break;
    case WL_CONNECTED:       return "CONNECTED"; break;
    case WL_CONNECT_FAILED:  return "CONNECT_FAILED"; break;
    case WL_CONNECTION_LOST: return "CONNECTION LOST"; break;
    case WL_DISCONNECTED:    return "DISCONNECTED"; break;
    case WL_NO_SHIELD:       return "NO SHIELD"; break;
    default:                 return "Undefined: " + String(WiFi.status()); break;
  }
}


/**************************************************************************************************
 *    Function      : NetworkTask
 *    Description   : All things that needs to be done in the superloop
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void NetworkTask(){
  #ifdef use_wifi_manager
    wm.process(); //WiFi Manager 
    
  #endif

  #ifdef use_wifi_manager
    if(wm.getConfigPortalActive()==false){
      if(server==NULL){
        configureServer();
      }
    }
  #endif
  if(server != NULL){
    server->handleClient();
  }
  if(MDNS.isRunning()){
    MDNS.update();
  } else {
    if (!MDNS.begin("co2guard")) {             // Start the mDNS responder for esp8266.local
                Serial.println("Error setting up MDNS responder!");
    } else {
                Serial.println("mDNS responder started");
                MDNS.addService("http", "tcp", 80);
    }
  }
}


/**************************************************************************************************
 *    Function      : getQuality
 *    Description   : Gets the Signalquality
 *    Input         : none 
 *    Output        : none
 *    Remarks       : Return the quality (Received Signal Strength Indicator)
                      of the WiFi network.
                      Returns a number between 0 and 100 if WiFi is connected.
                      Returns -1 if WiFi is disconnected.
 **************************************************************************************************/
int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}
