#include "parameters.h"
#include "process.h"
#include "inout.h"
#include <Arduino.h>

//Do we really update UI here?
#include <ui/events.h>
#include "lvgl.h"

#define PROFILE1_INDEX 0
#define PROFILE2_INDEX 1
#define PROFILE_OFF_INDEX 2

Parameters* Parameters::instance_ = nullptr;
/**
 * Best constructor ever :)
*/
Parameters::Parameters() :
    mqttParamGrp_("MQTT"), mqttServer_(mqttParamGrp_, "mqttServer", "MQTT server", "", "server.local"),
    mqttUser_(mqttParamGrp_, "mqttUser", "MQTT user", "user"), mqttPass_(mqttParamGrp_, "mqttPass", "MQTT password", ""),
    mqttPort_(mqttParamGrp_, "mqttPort", "MQTT port", 1883), mqttName_(mqttParamGrp_, "mqttName", "MQTT name", "Heater", "", "{\"required\":\"\"}"),
    heatingParamGrp_("Heating"),
    currentProfile_(heatingParamGrp_, "heatProfile", "Heating profile", "Profile 1;Profile 2;Off", "Profile 1"),
    timeBase_(heatingParamGrp_, "heatTimeBase", "Heating time-base [s]", 60),
    tempLimit_(heatingParamGrp_, "tempLimit", "Floor temperature limit [C]", 35),
    profile1ParamGrp_("Profile 1"),
    profile1HParamGrp_("Peak time"),
    profile1HOffTemp_(profile1HParamGrp_, "profile1HOffTemp", "Off temperature", 5.0),
    profile1HFullTemp_(profile1HParamGrp_, "profile1HFullTemp", "Full temperature", 0.0),
    profile1LParamGrp_("Off-peak time"),
    profile1LOffTemp_(profile1LParamGrp_, "profile1LOffTemp", "Off temperature", 12.0),
    profile1LFullTemp_(profile1LParamGrp_, "profile1LFullTemp", "Full temperature", 5.0),
    profile2ParamGrp_( "Profile 2"),
    profile2HParamGrp_("Peak time"),
    profile2HOffTemp_(profile2HParamGrp_, "profile2HOffTemp", "Off temperature", 5.0),
    profile2HFullTemp_(profile2HParamGrp_, "profile2HFullTemp", "Full temperature", 0.0),
    profile2LParamGrp_("Off-peak time"),
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
      if(portal_){
        portal_->saveParameters();
        Serial.println("Saving parameters!");
      }
    }
}

bool Parameters::validateProfile(ESPEasyCfgParameter<double> *param, double newValue, String& msg, int8_t& action)
{
    if(param == &instance_->profile1HFullTemp_){
        if(newValue > instance_->profile1HOffTemp_.getValue()){
            msg += "Invalid profile 1 peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile1HOffTemp_){
        if(newValue < instance_->profile1HFullTemp_.getValue()){
            msg += "Invalid profile 1 peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile1LFullTemp_){
        if(newValue > instance_->profile1LOffTemp_.getValue()){
            msg += "Invalid profile 1 off-peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile1LOffTemp_){
        if(newValue < instance_->profile1LFullTemp_.getValue()){
            msg += "Invalid profile 1 off-peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile2HFullTemp_){
        if(newValue > instance_->profile2HOffTemp_.getValue()){
            msg += "Invalid profile 2 peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile2HOffTemp_){
        if(newValue < instance_->profile2HFullTemp_.getValue()){
            msg += "Invalid profile 2 peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile2LFullTemp_){
        if(newValue > instance_->profile2LOffTemp_.getValue()){
            msg += "Invalid profile 2 off-peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profile2LOffTemp_){
        if(newValue < instance_->profile2LFullTemp_.getValue()){
            msg += "Invalid profile 2 off-peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }
    return false;
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
        // Adds profile 1 parameters
        heatingParamGrp_.add(&profile1ParamGrp_);
        profile1ParamGrp_.add(&profile1HParamGrp_);
        profile1ParamGrp_.add(&profile1LParamGrp_);
        
        // Adds provile 2 parameters
        profile2ParamGrp_.add(&profile2HParamGrp_);
        profile2ParamGrp_.add(&profile2LParamGrp_);
        heatingParamGrp_.add(&profile2ParamGrp_);
        portal->addParameterGroup(&heatingParamGrp_);
        // Adds parameters for system
        portal->addParameterGroup(&systemParamGrp_);

        profile1HFullTemp_.setValidator(Parameters::validateProfile);
        profile1HOffTemp_.setValidator(Parameters::validateProfile);
        profile1LFullTemp_.setValidator(Parameters::validateProfile);
        profile1LOffTemp_.setValidator(Parameters::validateProfile);
        profile2HFullTemp_.setValidator(Parameters::validateProfile);
        profile2HOffTemp_.setValidator(Parameters::validateProfile);
        profile2LFullTemp_.setValidator(Parameters::validateProfile);
        profile2LOffTemp_.setValidator(Parameters::validateProfile);
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
            peakTimeOffTemp = std::nan("");
            peakTimeFullTemp = std::nan("");
            offPeakTimeOffTemp = std::nan("");
            offPeakTimeFullTemp = std::nan("");
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
    //Refresh also current settings associated with profile
    refreshProfileSettings();
}

void Parameters::setTimeBase(int timeBase)
{
    timeBase_.setValue(timeBase);
    lastParameterChange_ = millis();
    lv_msg_send(EVT_NEW_TIME_BASE, &timeBase);
}

void Parameters::setLimiterTemp(double temperature)
{
    tempLimit_.setValue(temperature);
    lastParameterChange_ = millis();
    lv_msg_send(EVT_NEW_LIMITER_TEMP, &temperature);
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
    if(getCurrentProfileIndex() == PROFILE1_INDEX){
        Process::getInstance()->setPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE1_PT_OFF, &temperature);
}

void Parameters::setProfile1PeakFullTemp(double temperature)
{
    profile1HFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE1_INDEX){
        Process::getInstance()->setPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE1_PT_FULL, &temperature);
}

void Parameters::setProfile1OffPeakOffTemp(double temperature)
{
    profile1LOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE1_INDEX){
        Process::getInstance()->setOffPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE1_OPT_OFF, &temperature);
}

void Parameters::setProfile1OffPeakFullTemp(double temperature)
{
    profile1LFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE1_INDEX){
        Process::getInstance()->setOffPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE1_OPT_FULL, &temperature);
}

void Parameters::setProfile2PeakOffTemp(double temperature)
{
    profile2HOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE2_INDEX){
        Process::getInstance()->setPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE2_PT_OFF, &temperature);
}

void Parameters::setProfile2PeakFullTemp(double temperature)
{
    profile2HFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE2_INDEX){
        Process::getInstance()->setPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE2_PT_FULL, &temperature);
}

void Parameters::setProfile2OffPeakOffTemp(double temperature)
{
    profile2LOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE2_INDEX){
        Process::getInstance()->setOffPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE2_OPT_OFF, &temperature);
}

void Parameters::setProfile2OffPeakFullTemp(double temperature)
{
    profile2LFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE2_INDEX){
        Process::getInstance()->setOffPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE2_OPT_FULL, &temperature);
}

void Parameters::setADCRefVoltage(int voltage)
{
    adcRefVoltage_.setValue(voltage);
    lastParameterChange_ = millis();
    setADCVRef(voltage);
    lv_msg_send(EVT_NEW_ADC_REF_VOLTAGE, &voltage);
}

void Parameters::setADCAverage(int averages)
{
    adcAveraging_.setValue(averages);
    lastParameterChange_ = millis();
    setNbAverages(averages);
    lv_msg_send(EVT_NEW_ADC_AVERAGING, &averages);
}

void Parameters::captivePortalReconfigured()
{
    lastParameterChange_ = millis();
    //Sets ADC parameter
    int adcRefVoltage = getADCRefVoltage();
    setADCVRef(adcRefVoltage);
    lv_msg_send(EVT_NEW_ADC_REF_VOLTAGE, &adcRefVoltage);
    
    int adcAveraging = getADCAveraging();
    setNbAverages(adcAveraging);
    lv_msg_send(EVT_NEW_ADC_AVERAGING, &adcAveraging);

    double tempLimit = getLimiterTemp();
    Process::getInstance()->setTemperatureLimit(tempLimit);
    lv_msg_send(EVT_NEW_LIMITER_TEMP, &tempLimit);

    int timeBase = getTimeBase();
    Process::getInstance()->setTimebase(timeBase);
    lv_msg_send(EVT_NEW_TIME_BASE, &timeBase);
 
    refreshProfileSettings();
    //Refresh profile settings
    double temp = getProfile1OffPeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE1_OPT_FULL, &temp);
    temp = getProfile1OffPeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE1_OPT_OFF, &temp);
    temp = getProfile1PeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE1_PT_FULL, &temp);
    temp = getProfile1PeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE1_PT_OFF, &temp);
    temp = getProfile2OffPeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE2_OPT_FULL, &temp);
    temp = getProfile2OffPeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE2_OPT_OFF, &temp);
    temp = getProfile2PeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE2_PT_FULL, &temp);
    temp = getProfile2PeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE2_PT_OFF, &temp);
}

void Parameters::refreshProfileSettings()
{
    //Sets process parameters
    double profileLOffTemp, profileLFullTemp, profileHOffTemp, profileHFullTemp = 0.0;
    getCurrentProfileSetting(profileHOffTemp, profileHFullTemp, profileLOffTemp, profileLFullTemp);
    Process::getInstance()->setPeakTimeOffTemperature(profileHOffTemp);
    Process::getInstance()->setPeakTimeFullTemperature(profileHFullTemp);
    Process::getInstance()->setOffPeakTimeOffTemperature(profileLOffTemp);
    Process::getInstance()->setOffPeakTimeFullTemperature(profileLFullTemp);

    //Refresh the current profile
    lv_msg_send(EVT_NEW_HEATING_PROFILE, getCurrentProfile().c_str());  
}

void Parameters::resetToDefaults()
{
    portal_->resetToDefaults();
    ESP.restart();
}