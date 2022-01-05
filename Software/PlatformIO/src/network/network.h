/**************************************************************************************************
 *    Function      : SSIDList
 *    Description   : Returns the SSID List 
 *    Input         : String separator 
 *    Output        : String
 *    Remarks       : none
 **************************************************************************************************/
String SSIDList(String separator = ",");

/**************************************************************************************************
 *    Function      : getSSIDList
 *    Description   : Sends a SSID List to the client 
 *    Input         : none
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void getSSIDList() ;


/**************************************************************************************************
 *    Function      : setWiFiSettings
 *    Description   : Applies the WiFi settings 
 *    Input         : none
 *    Output        : none
 *    Remarks       : Store the wifi settings configured on the webpage and restart the esp to connect to this network
 **************************************************************************************************/
void setWiFiSettings();

//send the wifi settings to the connected client of the webserver
void getWiFiSettings() ;

/**************************************************************************************************
 *    Function      : getCO2value
 *    Description   : none 
 *    Input         : none
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void getCO2value();

/**************************************************************************************************
 *    Function      : setWiFiSettings
 *    Description   : Applies the WiFi settings 
 *    Input         : none
 *    Output        : none
 *    Remarks       : restart the esp as requested on the webpage
 **************************************************************************************************/
void restart() ;

/**************************************************************************************************
 *    Function      : getContentType
 *    Description   : Gets the contenttype depending on a filename 
 *    Input         : String
 *    Output        : String
 *    Remarks       : none
 **************************************************************************************************/
String getContentType(String filename) ;

/**************************************************************************************************
 *    Function      : sendFile
 *    Description   : Sends a requestedfile from SPIFFS
 *    Input         : none 
 *    Output        : none
 *    Remarks       : send a file from the SPIFFS to the connected client of the webserver
 **************************************************************************************************/
void sendFile();

/**************************************************************************************************
 *    Function      : sendFile
 *    Description   : Sends a requestedfile from SPIFFS
 *    Input         : String 
 *    Output        : none
 *    Remarks       : send data to the connected client of the webserver
 **************************************************************************************************/
void sendData(String data);

/**************************************************************************************************
 *    Function      : initWiFi
 *    Description   : initializes the WiFi
 *    Input         : String 
 *    Output        : none
 *    Remarks       : initialize wifi by connecting to a wifi network or creating an accesspoint
 **************************************************************************************************/
void initWiFi() ;

/**************************************************************************************************
 *    Function      : connectWiFi
 *    Description   : trys to establish a WiFi connection
 *    Input         : none 
 *    Output        : bool
 *    Remarks       : connect the esp to a wifi network, retuns false if failed
 **************************************************************************************************/
bool connectWiFi();


/**************************************************************************************************
 *    Function      : configureSoftAP
 *    Description   : Configures the ESP as SoftAP
 *    Input         : none 
 *    Output        : none
 *    Remarks       : configure the access point of the esp
 **************************************************************************************************/
void configureSoftAP() ;

/**************************************************************************************************
 *    Function      : configureServer
 *    Description   : Setup for the Webserver
 *    Input         : none 
 *    Output        : none
 *    Remarks       : initialize the webserver on port 80
 **************************************************************************************************/
void configureServer() ;



/**************************************************************************************************
 *    Function      : WiFiStatusToString
 *    Description   : Gives a string representing the WiFi status
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
String WiFiStatusToString() ;


/**************************************************************************************************
 *    Function      : NetworkTask
 *    Description   : All things that needs to be done in the superloop
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void NetworkTask();


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
int getQuality() ;

void storeCO2(int co2val);

void storeTemp(float t );

void useThingSpeak();

void setTEMPfield();

void setCO2field();

void writeFields();

void publishmsg();

void callbackMQTT();

void setupMQTT();

void reloadMQTTSettings();

void postThinkspeak( void );

void realoadTSSettings( void );