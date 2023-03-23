#include "inout.h"
#include "pin_config.h"

#include "lvgl.h"
#include "ui/events.h"
#include "Arduino.h"

#include "ntc.h"
#include <cmath>

#include <esp_adc_cal.h>

//External temperature voltage divider resistance
#define EXT_DIVIDER_RESISTANCE 15000.0
//Florr temperature voltage divider resistance (should be the same as EXT_DIVIDER_RESISTANCE)
#define FLOOR_DIVIDER_RESISTANCE 8200.0

// ADC reference voltage
// TODO: Check if we can automatically get this
static double adcRefVoltage = 3112.0;
// Number of averages to read value
static int averagingNb = 100;


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

double getResistance(uint32_t& mV, uint8_t pin, double dividerResistance)
{
    mV = 0;
    for(int i=0;i<averagingNb;++i){
        mV += analogReadMilliVolts(pin);
        if(mV<=0){
            return 0.0;
        }
    }
    mV /= averagingNb;
    return dividerResistance * (1/((adcRefVoltage/mV)-1));
}

double resistanceToTemperature(double resistance)
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

double getExternalTemperature(uint32_t& milliVolts, double& resistance)
{   
    resistance = getResistance(milliVolts, PIN_EXT_TEMP, EXT_DIVIDER_RESISTANCE);
    double ret = resistanceToTemperature(resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP_VOLT, &milliVolts);
    lv_msg_send(EVT_NEW_EXT_TEMP_RES, &resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP, &ret);    
    return ret;
}

double getFloorTemperature(uint32_t& milliVolts, double& resistance)
{
    resistance = getResistance(milliVolts, PIN_FLOOR_TEMP, FLOOR_DIVIDER_RESISTANCE);
    double ret = resistanceToTemperature(resistance);
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

bool getTariffInput()
{
    bool ret = digitalRead(PIN_TARIFF);
    lv_msg_send(EVT_NEW_TARIFF_STATE, &ret);
    return ret;
}

void setNbAverages(int averages)
{
    averagingNb = averages;
}

void setADCVRef(double vrefmv)
{
    adcRefVoltage = vrefmv;
}