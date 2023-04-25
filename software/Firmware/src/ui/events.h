#ifndef _EVENTS_UI_H
#define _EVENTS_UI_H

/**
 * This enumeration contains events published to UI
*/
typedef enum UIEvents{
    // Process events
    EVT_NEW_EXT_TEMP = 1,       //New external temperature
    EVT_NEW_SET_POINT,          //New set point (load percentage)
    EVT_NEW_FLOOR_TEMP,         //New floor temperature
    EVT_NEW_ZERO_PC_TEMP,       //New zero percent temperature (start heating)
    EVT_NEW_HUNDRED_PC_TEMP,    //New 100% temperature (full load)
    EVT_SYSTEM_READY,           //System is ready
    
    // Network events
    EVT_NEW_WIFI_SSID,          //Wifi SSID
    EVT_NEW_WIFI_MODE,          //Wifi mode
    EVT_NEW_WIFI_RSSI,          //Wifi RSSI
    EVT_NEW_IP_ADDRESS,         //IP address
    EVT_NEW_IP_MASK,            //IP network mask
    EVT_NEW_IP_GATEWAY,         //IP gateway
    EVT_NEW_IP_DNS,             //IP DNS server

    // I/O values
    EVT_NEW_EXT_TEMP_VOLT,      //External temperature ADC voltage
    EVT_NEW_EXT_TEMP_RES,       //External temperature ADC resistance
    EVT_NEW_FLOOR_TEMP_VOLT,    //Limiter temperature ADC voltage
    EVT_NEW_FLOOR_TEMP_RES,     //Limiter temperature ADC resistance
    EVT_NEW_TARIFF_STATE,       //Tariff input
    EVT_NEW_RELAY_STATE,        //Relay ouput

    // MQTT events
    EVT_NEW_MQTT_PREFIX,        //MQTT services prefix
    EVT_NEW_MQTT_BROKER,        //MQTT broker server
    EVT_NEW_MQTT_BROKER_PORT,   //MQTT broker port
    EVT_NEW_MQTT_STATUS,        //MQTT status string

    // Application settings
    EVT_NEW_HEATING_PROFILE,        //New heating profile
    EVT_NEW_TIME_BASE,              //New timebase
    EVT_NEW_LIMITER_TEMP,           //New limiter temperature
    EVT_NEW_PROFILE_ECO_PT_OFF,     //New eco profile peak time off load temperature
    EVT_NEW_PROFILE_ECO_PT_FULL,    //New eco profile peak time full load temperature
    EVT_NEW_PROFILE_ECO_OPT_OFF,    //New eco profile off-peak time off load temperature
    EVT_NEW_PROFILE_ECO_OPT_FULL,   //New eco profile off-peak time full load temperature
    EVT_NEW_PROFILE_STD_PT_OFF,     //New standard profile peak time off load temperature
    EVT_NEW_PROFILE_STD_PT_FULL,    //New standard profile peak time full load temperature
    EVT_NEW_PROFILE_STD_OPT_OFF,    //New standard profile off-peak time off load temperature
    EVT_NEW_PROFILE_STD_OPT_FULL,   //New standard profile off-peak time full load temperature
    EVT_NEW_ADC_REF_VOLTAGE,        //New ADC reference voltage
    EVT_NEW_ADC_AVERAGING,          //New ADC averaging samples
}UIEvents;


#endif