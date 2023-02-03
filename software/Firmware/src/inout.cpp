#include "inout.h"
#include "pin_config.h"

#include "lvgl.h"
#include "ui/events.h"
#include "Arduino.h"

#include "ntc.h"
#include <cmath>

#define ADC_REF_VOLTAGE_MV 3112.0
#define DIVIDER_RESISTANCE 15000.0
#define NB_AVERAGE 50

void setup_inputs_outputs()
{
    //Configure pins
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_USER_LED, OUTPUT);
    pinMode(PIN_TARIFF, INPUT);
    adcAttachPin(PIN_EXT_TEMP);
    adcAttachPin(PIN_FLOOR_TEMP);
    analogSetClockDiv(1);
}

double getResistance(uint32_t& mV, uint8_t pin)
{
    mV = 0;
    for(int i=0;i<NB_AVERAGE;++i){
        mV += analogReadMilliVolts(pin);
        if(mV<=0){
            return 0.0;
        }
    }
    mV /= NB_AVERAGE;
    return DIVIDER_RESISTANCE * (1/((ADC_REF_VOLTAGE_MV/mV)-1));
}

Temperature resistanceToTemperature(double resistance)
{
    if((resistance == 0.0) || (std::isinf(resistance))){
        return INFINITY;
    }
	int startIndex = 0;
	int midIndex = ntc_vals_size/2;
	int endIndex = ntc_vals_size-1;
	for(int i=0;i<ntc_vals_size; ++i){
		double startRes = ntc_vals[startIndex].resistance;
		double midRes = ntc_vals[midIndex].resistance;
		double endRes = ntc_vals[endIndex].resistance;
		if(resistance == startRes){
			return ntc_vals[startIndex].temperature;
		}else if(resistance == midRes){
			return ntc_vals[midIndex].temperature;			
		}else if(resistance == endRes){
			return ntc_vals[endIndex].temperature;			
		}else if(resistance > midRes){
			//Lower window			
			endIndex = midIndex;		
		}else{
			//Upper window
			startIndex = midIndex;		
		}
		midIndex = ((endIndex-startIndex)/2) + startIndex;
		if((endIndex-startIndex) < 2){
			break;
		}
	}
	double ratio = 0.0;
        double offset = 0.0;
        ratio = (ntc_vals[endIndex].temperature - ntc_vals[startIndex].temperature)/(ntc_vals[endIndex].resistance-ntc_vals[startIndex].resistance);
        offset = ntc_vals[startIndex].temperature - ratio*ntc_vals[startIndex].resistance;
        return resistance*ratio + offset;
}

Temperature getExternalTemperature(uint32_t& milliVolts, double& resistance)
{    
    resistance = getResistance(milliVolts, PIN_EXT_TEMP);
    Temperature ret = resistanceToTemperature(resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP_VOLT, &milliVolts);
    lv_msg_send(EVT_NEW_EXT_TEMP_RES, &resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP, &ret);    
    return ret;
}

Temperature getFloorTemperature(uint32_t& milliVolts, double& resistance)
{
    resistance = getResistance(milliVolts, PIN_FLOOR_TEMP);
    Temperature ret = resistanceToTemperature(resistance);
    lv_msg_send(EVT_NEW_FLOOR_TEMP_VOLT, &milliVolts);
    lv_msg_send(EVT_NEW_FLOOR_TEMP_RES, &resistance);
    lv_msg_send(EVT_NEW_FLOOR_TEMP, &ret);
    return ret;
}

void setRelay(bool status)
{
    digitalWrite(PIN_RELAY, status);
}

void setUserLed(bool status)
{
    digitalWrite(PIN_USER_LED, status);
}

/**
 * Gets tariff input state
 * @return true if input is active (230V on it)
*/
bool getTariffInput()
{
    return digitalRead(PIN_TARIFF);
}