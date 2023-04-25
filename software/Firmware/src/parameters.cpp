#include "parameters.h"
#include "process.h"
#include "inout.h"
#include "mqtt.h"
#include <Arduino.h>

//Do we really update UI here?
#include <ui/events.h>
#include "lvgl.h"

#define PROFILE_OFF_INDEX 0
#define PROFILE_ECO_INDEX 1
#define PROFILE_STD_INDEX 2


Parameters* Parameters::instance_ = nullptr;
/**
 * Best constructor ever :)
*/
Parameters::Parameters() :
    mqttParamGrp_("MQTT"), mqttServer_(mqttParamGrp_, "mqttServer", "MQTT server", "", "server.local"),
    mqttUser_(mqttParamGrp_, "mqttUser", "MQTT user", "user"), mqttPass_(mqttParamGrp_, "mqttPass", "MQTT password", ""),
    mqttPort_(mqttParamGrp_, "mqttPort", "MQTT port", 1883), mqttName_(mqttParamGrp_, "mqttName", "MQTT name", "Heater", "", "{\"required\":\"\"}"),
    heatingParamGrp_("Heating"),
    currentProfile_(heatingParamGrp_, "heatProfile", "Heating profile", "Off;Eco;Standard", "Off"),
    timeBase_(heatingParamGrp_, "heatTimeBase", "Heating time-base [s]", 60),
    tempLimit_(heatingParamGrp_, "tempLimit", "Floor temperature limit [C]", 35),
    profileEcoParamGrp_("Eco profile"),
    profileEcoHParamGrp_("Peak time"),
    profileEcoHOffTemp_(profileEcoHParamGrp_, "profileEcoHOffTemp", "Off temperature", 5.0),
    profileEcoHFullTemp_(profileEcoHParamGrp_, "profileEcoHFullTemp", "Full temperature", 0.0),
    profileEcoLParamGrp_("Off-peak time"),
    profileEcoLOffTemp_(profileEcoLParamGrp_, "profileEcoLOffTemp", "Off temperature", 8.0),
    profileEcoLFullTemp_(profileEcoLParamGrp_, "profileEcoLFullTemp", "Full temperature", 3.0),
    profileStdParamGrp_("Standard profile"),
    profileStdHParamGrp_("Peak time"),
    profileStdHOffTemp_(profileStdHParamGrp_, "profileStdHOffTemp", "Off temperature", 8.0),
    profileStdHFullTemp_(profileStdHParamGrp_, "profileStdHFullTemp", "Full temperature", 3.0),
    profileStdLParamGrp_("Off-peak time"),
    profileStdLOffTemp_(profileStdLParamGrp_, "profileStdLOffTemp", "Off temperature", 12.0),
    profileStdLFullTemp_(profileStdLParamGrp_, "profileStdLFullTemp", "Full temperature", 5.0),
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
    if(param == &instance_->profileEcoHFullTemp_){
        if(newValue > instance_->profileEcoHOffTemp_.getValue()){
            msg += "Invalid eco profile peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileEcoHOffTemp_){
        if(newValue < instance_->profileEcoHFullTemp_.getValue()){
            msg += "Invalid eco profile peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileEcoLFullTemp_){
        if(newValue > instance_->profileEcoLOffTemp_.getValue()){
            msg += "Invalid eco profile off-peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileEcoLOffTemp_){
        if(newValue < instance_->profileEcoLFullTemp_.getValue()){
            msg += "Invalid eco profile off-peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileStdHFullTemp_){
        if(newValue > instance_->profileStdHOffTemp_.getValue()){
            msg += "Invalid standard profile peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileStdHOffTemp_){
        if(newValue < instance_->profileStdHFullTemp_.getValue()){
            msg += "Invalid standard profile peak time off temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileStdLFullTemp_){
        if(newValue > instance_->profileStdLOffTemp_.getValue()){
            msg += "Invalid standard profile off-peak time full temperature!";
            action |= ESPEasyCfgAbstractParameter::RELOAD;
            return true;
        }
    }else if(param == &instance_->profileStdLOffTemp_){
        if(newValue < instance_->profileStdLFullTemp_.getValue()){
            msg += "Invalid standard profile off-peak time off temperature!";
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
        heatingParamGrp_.add(&profileEcoParamGrp_);
        profileEcoParamGrp_.add(&profileEcoHParamGrp_);
        profileEcoParamGrp_.add(&profileEcoLParamGrp_);
        
        // Adds provile 2 parameters
        profileStdParamGrp_.add(&profileStdHParamGrp_);
        profileStdParamGrp_.add(&profileStdLParamGrp_);
        heatingParamGrp_.add(&profileStdParamGrp_);
        portal->addParameterGroup(&heatingParamGrp_);
        // Adds parameters for system
        portal->addParameterGroup(&systemParamGrp_);

        profileEcoHFullTemp_.setValidator(Parameters::validateProfile);
        profileEcoHOffTemp_.setValidator(Parameters::validateProfile);
        profileEcoLFullTemp_.setValidator(Parameters::validateProfile);
        profileEcoLOffTemp_.setValidator(Parameters::validateProfile);
        profileStdHFullTemp_.setValidator(Parameters::validateProfile);
        profileStdHOffTemp_.setValidator(Parameters::validateProfile);
        profileStdLFullTemp_.setValidator(Parameters::validateProfile);
        profileStdLOffTemp_.setValidator(Parameters::validateProfile);
    }
}

int Parameters::getCurrentProfileIndex()
{
    if(currentProfile_.toString() == "Eco"){
        return PROFILE_ECO_INDEX;
    }else if(currentProfile_.toString() == "Standard"){
        return PROFILE_STD_INDEX;
    }
    return PROFILE_OFF_INDEX;
}

void Parameters::getProfileSetting(uint16_t index, double& peakTimeOffTemp, double& peakTimeFullTemp, 
                                    double& offPeakTimeOffTemp, double& offPeakTimeFullTemp)
{
    switch(index){
        case PROFILE_ECO_INDEX:
            peakTimeOffTemp = getProfileEcoPeakOffTemp();
            peakTimeFullTemp = getProfileEcoPeakFullTemp();
            offPeakTimeOffTemp = getProfileEcoOffPeakOffTemp();
            offPeakTimeFullTemp = getProfileEcoOffPeakFullTemp();
            break;
        case PROFILE_STD_INDEX:
            peakTimeOffTemp = getProfileStdPeakOffTemp();
            peakTimeFullTemp = getProfileStdPeakFullTemp();
            offPeakTimeOffTemp = getProfileStdOffPeakOffTemp();
            offPeakTimeFullTemp = getProfileStdOffPeakFullTemp();
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
    uint16_t index = getCurrentProfileIndex();
    getProfileSetting(index, peakTimeOffTemp, peakTimeFullTemp, 
                            offPeakTimeOffTemp, offPeakTimeFullTemp);
}

void Parameters::setCurrentProfile(const String& profileName)
{
    if((profileName == "Eco") || (profileName == "Standard") || (profileName == "Off")){
        currentProfile_.setValue(profileName.c_str());
        lastParameterChange_ = millis();
        //Refresh also current settings associated with profile
        refreshProfileSettings();
    }
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
    case PROFILE_ECO_INDEX:
        setProfileEcoPeakOffTemp(peakTimeOffTemp);
        setProfileEcoPeakFullTemp(peakTimeFullTemp);
        setProfileEcoOffPeakOffTemp(offPeakTimeOffTemp);
        setProfileEcoOffPeakFullTemp(offPeakTimeFullTemp);
        break;
    case PROFILE_STD_INDEX:
        setProfileStdPeakOffTemp(peakTimeOffTemp);
        setProfileStdPeakFullTemp(peakTimeFullTemp);
        setProfileStdOffPeakOffTemp(offPeakTimeOffTemp);
        setProfileStdOffPeakFullTemp(offPeakTimeFullTemp);
        break;
    default:
        break;
    }
}

void Parameters::setProfilePeakOffTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE_ECO_INDEX:
        setProfileEcoPeakOffTemp(temperature);
        break;
    case PROFILE_STD_INDEX:
        setProfileStdPeakOffTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfilePeakFullTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE_ECO_INDEX:
        setProfileEcoPeakFullTemp(temperature);
        break;
    case PROFILE_STD_INDEX:
        setProfileStdPeakFullTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfileOffPeakOffTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE_ECO_INDEX:
        setProfileEcoOffPeakOffTemp(temperature);
        break;
    case PROFILE_STD_INDEX:
        setProfileStdOffPeakOffTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfileOffPeakFullTemp(uint16_t index,double temperature)
{
    switch (index)
    {
    case PROFILE_ECO_INDEX:
        setProfileEcoOffPeakFullTemp(temperature);
        break;
    case PROFILE_STD_INDEX:
        setProfileStdOffPeakFullTemp(temperature);
        break;
    default:
        break;
    }
}

void Parameters::setProfileEcoPeakOffTemp(double temperature)
{
    profileEcoHOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_ECO_INDEX){
        Process::getInstance()->setPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_ECO_PT_OFF, &temperature);
}

void Parameters::setProfileEcoPeakFullTemp(double temperature)
{
    profileEcoHFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_ECO_INDEX){
        Process::getInstance()->setPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_ECO_PT_FULL, &temperature);
}

void Parameters::setProfileEcoOffPeakOffTemp(double temperature)
{
    profileEcoLOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_ECO_INDEX){
        Process::getInstance()->setOffPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_ECO_OPT_OFF, &temperature);
}

void Parameters::setProfileEcoOffPeakFullTemp(double temperature)
{
    profileEcoLFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_ECO_INDEX){
        Process::getInstance()->setOffPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_ECO_OPT_FULL, &temperature);
}

void Parameters::setProfileStdPeakOffTemp(double temperature)
{
    profileStdHOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_STD_INDEX){
        Process::getInstance()->setPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_STD_PT_OFF, &temperature);
}

void Parameters::setProfileStdPeakFullTemp(double temperature)
{
    profileStdHFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_STD_INDEX){
        Process::getInstance()->setPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_STD_PT_FULL, &temperature);
}

void Parameters::setProfileStdOffPeakOffTemp(double temperature)
{
    profileStdLOffTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_STD_INDEX){
        Process::getInstance()->setOffPeakTimeOffTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_STD_OPT_OFF, &temperature);
}

void Parameters::setProfileStdOffPeakFullTemp(double temperature)
{
    profileStdLFullTemp_.setValue(temperature);
    lastParameterChange_ = millis();
    if(getCurrentProfileIndex() == PROFILE_STD_INDEX){
        Process::getInstance()->setOffPeakTimeFullTemperature(temperature);
    }
    lv_msg_send(EVT_NEW_PROFILE_STD_OPT_FULL, &temperature);
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
    double temp = getProfileEcoOffPeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE_ECO_OPT_FULL, &temp);
    temp = getProfileEcoOffPeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE_ECO_OPT_OFF, &temp);
    temp = getProfileEcoPeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE_ECO_PT_FULL, &temp);
    temp = getProfileEcoPeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE_ECO_PT_OFF, &temp);
    temp = getProfileStdOffPeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE_STD_OPT_FULL, &temp);
    temp = getProfileStdOffPeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE_STD_OPT_OFF, &temp);
    temp = getProfileStdPeakFullTemp();
    lv_msg_send(EVT_NEW_PROFILE_STD_PT_FULL, &temp);
    temp = getProfileStdPeakOffTemp();
    lv_msg_send(EVT_NEW_PROFILE_STD_PT_OFF, &temp);

    MQTT::getInstance()->reconfigure();
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