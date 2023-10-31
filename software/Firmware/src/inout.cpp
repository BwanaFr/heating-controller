#include "inout.h"
#include "pin_config.h"

#include "lvgl.h"
#include "ui/events.h"
#include "Arduino.h"

#include "ntc.h"
#include <cmath>

#include "SPI.h"

static const double MAX6682VRef = 1.22;
static const double MAX6682Rext = 1200.0;
static double extResistance = 0.0;
static double floorResistance = 0.0;
static double extMillivolts = 0.0;
static double floorMillivolts = 0.0;


static SemaphoreHandle_t adcLock = xSemaphoreCreateMutex();

/**
 * Coumputes VRext/VR+ ratio using the formula found
 * in the MAX6682 data sheet :
 * Dout = ((Ratio-0.174387)*8)/0.010404
*/
double getVoltageRatio(int16_t adcValue)
{
    return ((adcValue*0.010404) / 8) + 0.174387;
}

/**
 * Read a MAX6682
 * @param cs Chip select pin to use
 * @return signed 10 bit value
*/
int16_t readMAX6682(int8_t cs)
{
    uint32_t temp = 0;
    digitalWrite(cs, LOW);
    delayMicroseconds(1);
    for(int i=0;i<16;++i){
        if(i<11){
            temp |= (digitalRead(PIN_MAX6682_SO) & 0x1) << (10-i);
        }
        digitalWrite(PIN_MAX6682_SCK, HIGH);
        delayMicroseconds(1);
        digitalWrite(PIN_MAX6682_SCK, LOW);
        delayMicroseconds(1);
    }
    digitalWrite(cs, HIGH);
    int16_t ret = (temp << 5) & 0xFFFF;
    ret = ret / 32;
    return ret;
}

/**
 * Get resistance of specified chip select pin
*/
double getResistance(int8_t pin)
{
    double ret = 0;
    int16_t rawReading = readMAX6682(pin);
    //Get ratio
    double ratio = getVoltageRatio(rawReading);
    //Using ratio, computes voltages across resistors
    double vRext = ratio * MAX6682VRef;
    double vTh = MAX6682VRef*(1-ratio);
    //Determine current flow accross all resistors
    double iR = vRext / MAX6682Rext;
    //With current and voltage, we can get thermistor value
    ret = vTh / iR;

    xSemaphoreTake(adcLock, portMAX_DELAY);
    if(pin == PIN_EXT_TEMP_CS){
        extResistance = ret;
        extMillivolts = vTh * 1000.0;
    }else{
        floorResistance = ret;
        floorMillivolts = vTh * 1000.0;
    }
    xSemaphoreGive(adcLock);
    return ret;
}

/**
 * FreeRTOS task to read ADCs
*/
void adcReadTask(void* params)
{
    while(true){
        delay(2100);
        getResistance(PIN_EXT_TEMP_CS);
        getResistance(PIN_FLOOR_TEMP_CS);
    }
}

void setup_inputs_outputs()
{
    //Configure pins
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_USER_LED, OUTPUT);
    pinMode(PIN_TARIFF, INPUT);

    pinMode(PIN_MAX6682_SO, INPUT_PULLUP);
    pinMode(PIN_MAX6682_SCK, OUTPUT);
    digitalWrite(PIN_MAX6682_SCK, LOW);

    pinMode(PIN_EXT_TEMP_CS, OUTPUT);
    digitalWrite(PIN_EXT_TEMP_CS, HIGH);
    getResistance(PIN_EXT_TEMP_CS);

    pinMode(PIN_FLOOR_TEMP_CS, OUTPUT);
    digitalWrite(PIN_FLOOR_TEMP_CS, HIGH);
    getResistance(PIN_FLOOR_TEMP_CS);
    xTaskCreate(adcReadTask, "adcReadTask", 2048, NULL, 1, NULL);
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
    resistance = extResistance;
    milliVolts = extMillivolts;
    double ret = resistanceToTemperature(resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP_VOLT, &milliVolts);
    lv_msg_send(EVT_NEW_EXT_TEMP_RES, &resistance);
    lv_msg_send(EVT_NEW_EXT_TEMP, &ret);
    return ret;
}

double getFloorTemperature(uint32_t& milliVolts, double& resistance)
{
    resistance = floorResistance;
    milliVolts = floorMillivolts;
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
    bool ret = !digitalRead(PIN_TARIFF);
    return ret;
}

void setNbAverages(int averages)
{
    /*xSemaphoreTake(adcLock, portMAX_DELAY);
    averagingNb = averages;
    xSemaphoreGive(adcLock);*/
}

void setADCVRef(double vrefmv)
{
    // adcRefVoltage = vrefmv;
}
