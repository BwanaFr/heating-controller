#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <ESPEasyCfg.h>

/**
 * This class holds all parameters
 * 
*/
class Parameters {
public:
    Parameters();
    virtual ~Parameters() = default;
    void loop();
    void init(ESPEasyCfg* portal);
    //Getters for parameters
    inline const String getMQTTServer() { return mqttServer_.getValue(); }
    inline const String getMQTTUser() { return mqttUser_.getValue(); }
    inline const String getMQTTPass() { return mqttPass_.getValue(); }
    inline int getMQTTPort() { return mqttPort_.getValue(); }
    inline const String getMQTTName() { return mqttName_.getValue(); }

    inline const String getCurrentProfile() { return currentProfile_.toString(); }
    int getCurrentProfileIndex();
    inline int getTimeBase() { return timeBase_.getValue(); }
    inline double getLimiterTemp() { return tempLimit_.getValue(); }

    void getProfileSetting(uint16_t index, double& peakTimeOffTemp, double& peakTimeFullTemp, 
                                    double& offPeakTimeOffTemp, double& offPeakTimeFullTemp);

    void getCurrentProfileSetting(double& peakTimeOffTemp, double& peakTimeFullTemp, 
                                    double& offPeakTimeOffTemp, double& offPeakTimeFullTemp);

    inline double getProfile1PeakOffTemp() { return profile1HOffTemp_.getValue(); }
    inline double getProfile1PeakFullTemp() { return profile1HFullTemp_.getValue(); }
    inline double getProfile1OffPeakOffTemp() { return profile1LOffTemp_.getValue(); }
    inline double getProfile1OffPeakFullTemp() { return profile1LFullTemp_.getValue(); }

    inline double getProfile2PeakOffTemp() { return profile2HOffTemp_.getValue(); }
    inline double getProfile2PeakFullTemp() { return profile2HFullTemp_.getValue(); }
    inline double getProfile2OffPeakOffTemp() { return profile2LOffTemp_.getValue(); }
    inline double getProfile2OffPeakFullTemp() { return profile2LFullTemp_.getValue(); }

    inline int getADCRefVoltage() { return adcRefVoltage_.getValue(); }
    inline int getADCAveraging() { return adcAveraging_.getValue(); }

    void setCurrentProfile(const String& profileName);
    void setTimeBase(int timeBase);
    void setLimiterTemp(double temperature);

    void setProfileSetting(uint16_t index, double peakTimeOffTemp, double peakTimeFullTemp, 
                                    double offPeakTimeOffTemp, double offPeakTimeFullTemp);

    void setProfilePeakOffTemp(uint16_t index,double temperature);
    void setProfilePeakFullTemp(uint16_t index,double temperature);
    void setProfileOffPeakOffTemp(uint16_t index,double temperature);
    void setProfileOffPeakFullTemp(uint16_t index,double temperature);

    void setProfile1PeakOffTemp(double temperature);
    void setProfile1PeakFullTemp(double temperature);
    void setProfile1OffPeakOffTemp(double temperature);
    void setProfile1OffPeakFullTemp(double temperature);

    void setProfile2PeakOffTemp(double temperature);
    void setProfile2PeakFullTemp(double temperature);
    void setProfile2OffPeakOffTemp(double temperature);
    void setProfile2OffPeakFullTemp(double temperature);

    void setADCRefVoltage(int voltage);
    void setADCAverage(int averages);

    void captivePortalReconfigured();
    void refreshProfileSettings();

    void resetToDefaults();

    static inline Parameters* getInstance(){ return instance_; }

    static bool validateProfile(ESPEasyCfgParameter<double> *param, double newValue, String& msg, int8_t& action);
private:
    ESPEasyCfgParameterGroup mqttParamGrp_;
    ESPEasyCfgParameter<String> mqttServer_;
    ESPEasyCfgParameter<String> mqttUser_;
    ESPEasyCfgParameter<String> mqttPass_;
    ESPEasyCfgParameter<int> mqttPort_;
    ESPEasyCfgParameter<String> mqttName_;

    // Objects for heating process configuration
    ESPEasyCfgParameterGroup heatingParamGrp_;
    ESPEasyCfgEnumParameter currentProfile_;
    ESPEasyCfgParameter<int> timeBase_;
    ESPEasyCfgParameter<double> tempLimit_;

    ESPEasyCfgParameterGroup profile1ParamGrp_;
    ESPEasyCfgParameterGroup profile1HParamGrp_;
    ESPEasyCfgParameter<double> profile1HOffTemp_;
    ESPEasyCfgParameter<double> profile1HFullTemp_;
    ESPEasyCfgParameterGroup profile1LParamGrp_;
    ESPEasyCfgParameter<double> profile1LOffTemp_;
    ESPEasyCfgParameter<double> profile1LFullTemp_;

    ESPEasyCfgParameterGroup profile2ParamGrp_;
    ESPEasyCfgParameterGroup profile2HParamGrp_;
    ESPEasyCfgParameter<double> profile2HOffTemp_;
    ESPEasyCfgParameter<double> profile2HFullTemp_;
    ESPEasyCfgParameterGroup profile2LParamGrp_;
    ESPEasyCfgParameter<double> profile2LOffTemp_;
    ESPEasyCfgParameter<double> profile2LFullTemp_;

    // System objects for ADC
    ESPEasyCfgParameterGroup systemParamGrp_;
    ESPEasyCfgParameter<int> adcRefVoltage_;
    ESPEasyCfgParameter<int> adcAveraging_;

    unsigned long lastParameterChange_ = 0;                     // Last time one saved parameter was changed (to delay the saving)
    static constexpr unsigned long delayedParameterSaving = 10L * 1000L;   // Delay before saving parameters to flash
    ESPEasyCfg* portal_;
    static Parameters* instance_;
};


#endif