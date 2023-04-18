#include "parameters.h"
#include <Arduino.h>

#define PROFILE1_INDEX 0
#define PROFILE2_INDEX 1
#define PROFILE_OFF_INDEX 2

Parameters* Parameters::instance_ = nullptr;
/**
 * Best construtor ever :)
*/
Parameters::Parameters() :
    mqttParamGrp_("MQTT"), mqttServer_(mqttParamGrp_, "mqttServer", "MQTT server", "server.local"),
    mqttUser_(mqttParamGrp_, "mqttUser", "MQTT user", "user"), mqttPass_(mqttParamGrp_, "mqttPass", "MQTT password", ""),
    mqttPort_(mqttParamGrp_, "mqttPort", "MQTT port", 1883), mqttName_(mqttParamGrp_, "mqttName", "MQTT name", "Heater", "", "{\"required\":\"\"}"),
    heatingParamGrp_("Heating"), currentProfile_(heatingParamGrp_, "heatProfile", "Heating profile", "Profile 1;Profile 2;Off", "Profile 1"),
    timeBase_(heatingParamGrp_, "heatTimeBase", "Heating time-base [s]", 60),
    tempLimit_(heatingParamGrp_, "tempLimit", "Floor temperature limit [C]", 35),
    profile1ParamGrp_("Profile 1"), profile1HParamGrp_(&profile1ParamGrp_, "Peak time"),
    profile1HOffTemp_(profile1HParamGrp_, "profile1HOffTemp", "Off temperature", 5.0),
    profile1HFullTemp_(profile1HParamGrp_, "profile1HFullTemp", "Full temperature", 0.0),
    profile1LParamGrp_(&profile1ParamGrp_, "Off-peak time"),
    profile1LOffTemp_(profile1LParamGrp_, "profile1LOffTemp", "Off temperature", 12.0),
    profile1LFullTemp_(profile1LParamGrp_, "profile1LFullTemp", "Full temperature", 5.0),
    profile2ParamGrp_("Profile 2"),
    profile2HParamGrp_(&profile2ParamGrp_, "Peak time"),
    profile2HOffTemp_(profile2HParamGrp_, "profile2HOffTemp", "Off temperature", 5.0),
    profile2HFullTemp_(profile2HParamGrp_, "profile2HFullTemp", "Full temperature", 0.0),
    profile2LParamGrp_(&profile2ParamGrp_, "Off-peak time"),
    profile2LOffTemp_(profile2LParamGrp_, "profile2LOffTemp", "Off temperature", 12.0),
    profile2LFullTemp_(profile2LParamGrp_, "profile2LFullTemp", "Full temperature", 5.0),
    systemParamGrp_("System"),
    adcRefVoltage_(systemParamGrp_, "adcRefVoltage", "ADC reference voltage [mV]", 3112, "", "{\"min\":\"0\", \"max\":\"3500\"}"),
    adcAveraging_(systemParamGrp_, "adcAveraging", "ADC averaging [samples]", 100, "", "{\"min\":\"0\", \"max\":\"500\"}"),
    lastParameterChange_(0)
{
    instance_ = this;
}

void Parameters::loop()
{
    unsigned long now = millis();
    //Delayed parameter saving
    if((lastParameterChange_ != 0) && ((now-lastParameterChange_)>delayedParameterSaving)){
      lastParameterChange_ = 0;
      if(portal_)
        portal_->saveParameters();
    }
}

void Parameters::init(ESPEasyCfg* portal)
{
    //Configure captive portal
    //The MQTT password is a password HTML type
    mqttPass_.setInputType("password");
    if(portal){
        portal_ = portal;
        //Finally, add our parameter group to the captive portal
        portal->addParameterGroup(&mqttParamGrp_);
        // Adds parameters for heating process
        portal->addParameterGroup(&heatingParamGrp_);
        // Adds parameters for system
        portal->addParameterGroup(&systemParamGrp_);
    }
}

int Parameters::getCurrentProfileIndex()
{
    if(currentProfile_.toString() == "Profile 1"){
        return PROFILE1_INDEX;
    }else if(currentProfile_.toString() == "Profile 2"){
        return PROFILE2_INDEX;
    }
    return PROFILE_OFF_INDEX;
}

void Parameters::getProfileSetting(uint16_t index, double& peakTimeOffTemp, double& peakTimeFullTemp, 
                                    double& offPeakTimeOffTemp, double& offPeakTimeFullTemp)
{
    switch(index){
        case PROFILE1_INDEX:
            peakTimeOffTemp = getProfile1PeakOffTemp();
            peakTimeFullTemp = getProfile1PeakFullTemp();
            offPeakTimeOffTemp = getProfile1OffPeakOffTemp();
            offPeakTimeFullTemp = getProfile1OffPeakFullTemp();
            break;
        case PROFILE2_INDEX:
            peakTimeOffTemp = getProfile2PeakOffTemp();
            peakTimeFullTemp = getProfile2PeakFullTemp();
            offPeakTimeOffTemp = getProfile2OffPeakOffTemp();
            offPeakTimeFullTemp = getProfile2OffPeakFullTemp();
            break;
        default:
            peakTimeOffTemp = 0.0;
            peakTimeFullTemp = 0.0;
            offPeakTimeOffTemp = 0.0;
            offPeakTimeFullTemp = 0.0;
    }
}

void Parameters::getCurrentProfileSetting(double& peakTimeOffTemp, double& peakTimeFullTemp, 
                                double& offPeakTimeOffTemp, double& offPeakTimeFullTemp)
{
    uint16_t index = PROFILE_OFF_INDEX;
    if(currentProfile_.toString() == "Profile 1"){
        index = PROFILE1_INDEX;
    }else if(currentProfile_.toString() == "Profile 2"){
        index = PROFILE2_INDEX;
    }
    getProfileSetting(index, peakTimeOffTemp, peakTimeFullTemp, 
                            offPeakTimeOffTemp, offPeakTimeFullTemp);
}

void Parameters::setCurrentProfile(const String& profileName)
{
    currentProfile_.setValue(profileName.c_str());
    lastParameterChange_ = millis();
}

void Parameters::setTimeBase(int timeBase)
{
    timeBase_.setValue(timeBase);
    lastParameterChange_ = millis();
}

void Parameters::setLimiterTemp(double temperature)
{
    tempLimit_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfileSetting(uint16_t index, double peakTimeOffTemp, double peakTimeFullTemp, 
                            double offPeakTimeOffTemp, double offPeakTimeFullTemp)
{
    switch (index)
    {
    case PROFILE1_INDEX:
        setProfile1PeakOffTemp(peakTimeOffTemp);
        setProfile1PeakFullTemp(peakTimeFullTemp);
        setProfile1OffPeakOffTemp(offPeakTimeOffTemp);
        setProfile1OffPeakFullTemp(offPeakTimeFullTemp);
        break;
    case PROFILE2_INDEX:
        setProfile2PeakOffTemp(peakTimeOffTemp);
        setProfile2PeakFullTemp(peakTimeFullTemp);
        setProfile2OffPeakOffTemp(offPeakTimeOffTemp);
        setProfile2OffPeakFullTemp(offPeakTimeFullTemp);
        break;
    default:
        break;
    }
}

void Parameters::setProfilePeakOffTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE1_INDEX:
        setProfile1PeakOffTemp(temperature);
        break;
    case PROFILE2_INDEX:
        setProfile2PeakOffTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfilePeakFullTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE1_INDEX:
        setProfile1PeakFullTemp(temperature);
        break;
    case PROFILE2_INDEX:
        setProfile2PeakFullTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfileOffPeakOffTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE1_INDEX:
        setProfile1OffPeakOffTemp(temperature);
        break;
    case PROFILE2_INDEX:
        setProfile2OffPeakOffTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfileOffPeakFullTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE1_INDEX:
        setProfile1OffPeakFullTemp(temperature);
        break;
    case PROFILE2_INDEX:
        setProfile2OffPeakFullTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfile1PeakOffTemp(double temperature)
{
    profile1HOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile1PeakFullTemp(double temperature)
{
    profile1HFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile1OffPeakOffTemp(double temperature)
{
    profile1LOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile1OffPeakFullTemp(double temperature)
{
    profile1LFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile2PeakOffTemp(double temperature)
{
    profile2HOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile2PeakFullTemp(double temperature)
{
    profile2HFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile2OffPeakOffTemp(double temperature)
{
    profile2LOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}

void Parameters::setProfile2OffPeakFullTemp(double temperature)
{
    profile2LFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
}


void Parameters::setADCRefVoltage(int voltage)
{
    adcRefVoltage_.setValue(voltage);
    lastParameterChange_ = millis();
}

void Parameters::setADCAverage(int voltage)
{
    adcAveraging_.setValue(voltage);
    lastParameterChange_ = millis();
}