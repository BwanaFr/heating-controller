#include "temperature.h"
#include "pin_config.h"

#include "lvgl.h"
#include "ui/events.h"

double getExternalTemperature(double& milliVolts, uint32_t& resistance)
{
    double ret = 0.0;

    milliVolts = 12.3;
    resistance = 123;
    lv_msg_send(EVT_NEW_EXT_TEMP_VOLT, &milliVolts);
    lv_msg_send(EVT_NEW_EXT_TEMP_RES, &resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP, &ret);
    return ret;
}

double getFloorTemperature(double& milliVolts, uint32_t& resistance)
{
    double ret = 0.0;

    milliVolts = 23.45;
    resistance = 2345;
    lv_msg_send(EVT_NEW_FLOOR_TEMP_VOLT, &milliVolts);
    lv_msg_send(EVT_NEW_FLOOR_TEMP_RES, &resistance);
    lv_msg_send(EVT_NEW_FLOOR_TEMP, &ret);
    return ret;
}