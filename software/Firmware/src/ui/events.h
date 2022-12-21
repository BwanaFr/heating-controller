#ifndef _EVENTS_UI_H
#define _EVENTS_UI_H

typedef enum UIEvents{
    EVT_NEW_EXT_TEMP = 1,       //New external temperature
    EVT_NEW_SET_POINT,          //New set point (load percentage)
    EVT_NEW_FLOOR_TEMP,         //New floor temperature
    EVT_NEW_HEATING_PROFILE,    //New heating profile
    EVT_NEW_ZERO_PC_TEMP,       //New zero percent temperature (start heating)
    EVT_NEW_HUNDRED_PC_TEMP,    //New 100% temperature (full load)
    EVT_NEW_TIME_BASE,          //New timebase
}UIEvents;


#endif