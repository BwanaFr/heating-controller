#include "process.h"
#include <Arduino.h>

//Do we really update UI here?
#include <ui/events.h>
#include "lvgl.h"

Process* Process::instance_ = nullptr;

Process::Process() :
    _timebase(0), _ptOffTemp(0.0), _ptFullTemp(0.0),
    _optOffTemp(0.0), _optFullTemp(0.0), _tempLimit(100.0),
    _loadPercent(-1), _cycleStart(0), _relayState(false), 
    _peakTime(false)
{
    instance_ = this;
}

bool Process::setIOState(double external, double floor, bool peakTime)
{
    bool ret = false;
    //Save actual value to know if something changed
    int prevLoad = _loadPercent;
    bool prevPeakTime = _peakTime;
    bool refreshUI = false;
    if(_loadPercent == -1){
        prevPeakTime = !peakTime;
        _loadPercent = 0;
    }
    if(_cycleStart == 0){
        refreshUI = true;
    }

    double offTemp = peakTime ? _ptOffTemp : _optOffTemp;
    double fullTemp = peakTime ? _ptFullTemp : _ptFullTemp;
    if(!std::isnan(offTemp) && !std::isnan(fullTemp)){
        //Profile activated
        if(floor < _tempLimit) {
            //Limiter not reached
            //Compute percentage
            if(external >= offTemp){
                //Heater off
                _loadPercent = 0;
            }else if(external <= fullTemp){
                //Full load
                _loadPercent = 100;
            }else{
                //Make interpolation
                double ratio = -100.0/(offTemp-fullTemp);
                double offset = 100.0-ratio*fullTemp;
                _loadPercent = (int)(ratio*external + offset);
            }
            //Computes relay time
            unsigned long now = millis();
            if((_cycleStart == 0) || ((now-_cycleStart) >= _timebase)){
                //Complete a cycle
                _cycleStart = now;
            }
            unsigned long onPeriod = (_timebase * _loadPercent)/100;
            if(((now - _cycleStart) <= onPeriod) && (onPeriod != 0)){
                //Relay active
                ret = true;
            }
        }
    }
    //Update UI
    if(refreshUI || (ret != _relayState)){
        _relayState = ret;
        lv_msg_send(EVT_NEW_RELAY_STATE, _relayState ? "On" : "Off");
    } 
    if(refreshUI || (prevLoad != _loadPercent)){
        prevLoad = _loadPercent;
        lv_msg_send(EVT_NEW_SET_POINT, &_loadPercent);
    }
    if(refreshUI || (_peakTime != peakTime)){
        _peakTime = peakTime;
        lv_msg_send(EVT_NEW_TARIFF_STATE, _peakTime ? "Peak time" : "Off-peak time");
        lv_msg_send(EVT_NEW_HUNDRED_PC_TEMP, std::isnan(fullTemp) ? NULL : &fullTemp);
        lv_msg_send(EVT_NEW_ZERO_PC_TEMP, std::isnan(offTemp) ? NULL : &offTemp);
    }
    _floorTemp = floor;
    _externalTemp = external;
    return ret;
}

void Process::setTimebase(int timebase)
{
    int newTimebase = timebase*1000;
    if(newTimebase != _timebase) {
        _timebase = newTimebase;
        lv_msg_send(EVT_NEW_TIME_BASE, &timebase);
    }
}

void Process::setPeakTimeOffTemperature(double temperature)
{
    if(temperature != _ptOffTemp) {
        _ptOffTemp = temperature;
        _cycleStart = 0;
    }
}

void Process::setPeakTimeFullTemperature(double temperature)
{
    if(temperature != _ptFullTemp) {
        _ptFullTemp = temperature;
        _cycleStart = 0;
    }    
}

void Process::setOffPeakTimeOffTemperature(double temperature)
{
    if(temperature != _optOffTemp) {
        _optOffTemp = temperature;
        _cycleStart = 0;
    }
}

void Process::setOffPeakTimeFullTemperature(double temperature)
{
    if(temperature != _optFullTemp) {
        _optFullTemp = temperature;
        _cycleStart = 0;
    }
}