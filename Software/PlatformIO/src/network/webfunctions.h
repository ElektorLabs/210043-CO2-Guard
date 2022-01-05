#ifndef WEBFUNCTION_H_
#define WEBFUNCTION_H_

/**************************************************************************************************
*    Function      : response_settings
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/     
void response_settings( void );

/**************************************************************************************************
*    Function      : ntp_settings_update
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void ntp_settings_update( void );

/**************************************************************************************************
*    Function      : timezone_update
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void timezone_update( void );

/**************************************************************************************************
*    Function      : timezone_overrides_update
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void timezone_overrides_update( void );

/**************************************************************************************************
*    Function      : settime_update
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void settime_update( void );

/**************************************************************************************************
*    Function      : update_pinballscore
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void update_pinballscore( void );

/**************************************************************************************************
*    Function      : update_display_wheelsize
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void update_display_wheelsize( void );

/**************************************************************************************************
*    Function      : update_display_datetimemode
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void update_display_datetimemode( void );

/**************************************************************************************************
*    Function      : update_display_sleepmode
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void update_display_sleepmode( void );

/**************************************************************************************************
*    Function      : display_readstatus
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void display_readstatus( void );

/**************************************************************************************************
*    Function      : force_display_adjust
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void force_display_adjust( void );

/**************************************************************************************************
*    Function      : update_notes
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void update_notes();

/**************************************************************************************************
*    Function      : read_notes
*    Description   : none
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/ 
void read_notes();

/**************************************************************************************************
*    Function      : updates the mqtt settings
*    Description   : Parses POST for new MQTT settings 
*    Input         : none
*    Output        : none
*    Remarks       : none
**************************************************************************************************/
void mqttsettings_update( void );


/**************************************************************************************************
*    Function      : Reads back the MQTT settings
*    Description   : Sends a json string 
*    Input         : none
*    Output        : none
*    Remarks       : only contains if a password is set no data itself
**************************************************************************************************/
void read_mqttsetting( void );

void SetThinkspeakServerSettings( void );

void GetThinkspeakSeverSettings( void );
#endif
