#include "process.h"
#include <Arduino.h>

//Do we really update UI here?
#include <ui/events.h>
#include "lvgl.h"

Process::Process() :
    _timebase(0), _ptOffTemp(0.0), _ptFullTemp(0.0),
    _optOffTemp(0.0), _optFullTemp(0.0), _tempLimit(100.0),
    _loadPercent(-1), _cycleStart(0), _relayState(false), 
    _peakTime(false)
{
}

bool Process::setIOState(double external, double floor, bool peakTime)
{
    bool ret = false;
    //Save actual valu to know if something changed
    int prevLoad = _loadPercent;
    bool prevPeakTime = _peakTime;
    if(_loadPercent == -1){
        prevPeakTime = !peakTime;
    }

    double offTemp = peakTime ? _ptOffTemp : _optOffTemp;
    double fullTemp = peakTime ? _ptFullTemp : _ptFullTemp;
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
        if((now-_cycleStart) <= onPeriod){
            //Relay active
            ret = true;
        }
    }
    //Update UI
    if(ret != _relayState){
        _relayState = ret;
        lv_msg_send(EVT_NEW_RELAY_STATE, _relayState ? "On" : "Off");
    } 
    if(prevLoad != _loadPercent){
        prevLoad = _loadPercent;
        lv_msg_send(EVT_NEW_SET_POINT, &_loadPercent);
    }
    if(_peakTime != peakTime){
        _peakTime = peakTime;
        lv_msg_send(EVT_NEW_TARIFF_STATE, _peakTime ? "Peak time" : "Off-peak time");
        lv_msg_send(EVT_NEW_HUNDRED_PC_TEMP, &fullTemp);
        lv_msg_send(EVT_NEW_ZERO_PC_TEMP, &offTemp);
    }
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

void Process::setPeakTimeParameters(double offTemp, double fullTemp)
{
    bool resetProcess = false;
    if((offTemp != _ptOffTemp) || (fullTemp != _ptFullTemp)) {
        resetProcess = true;
    }
    _ptOffTemp = offTemp;
    _ptFullTemp = fullTemp;
    if(resetProcess) {
        _cycleStart = 0;
    }
}

void Process::setOffPeakTimeParameters(double offTemp, double fullTemp)
{
    bool resetProcess = false;
    if((offTemp != _optOffTemp) || (fullTemp != _optFullTemp)) {
        resetProcess = true;
    }
    _optOffTemp = offTemp;
    _optFullTemp = fullTemp;
    if(resetProcess) {
        _cycleStart = 0;
    }
}