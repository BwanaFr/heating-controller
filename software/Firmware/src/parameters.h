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

    inline double getProfileEcoPeakOffTemp() { return profileEcoHOffTemp_.getValue(); }
    inline double getProfileEcoPeakFullTemp() { return profileEcoHFullTemp_.getValue(); }
    inline double getProfileEcoOffPeakOffTemp() { return profileEcoLOffTemp_.getValue(); }
    inline double getProfileEcoOffPeakFullTemp() { return profileEcoLFullTemp_.getValue(); }

    inline double getProfileStdPeakOffTemp() { return profileStdHOffTemp_.getValue(); }
    inline double getProfileStdPeakFullTemp() { return profileStdHFullTemp_.getValue(); }
    inline double getProfileStdOffPeakOffTemp() { return profileStdLOffTemp_.getValue(); }
    inline double getProfileStdOffPeakFullTemp() { return profileStdLFullTemp_.getValue(); }

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

    void setProfileEcoPeakOffTemp(double temperature);
    void setProfileEcoPeakFullTemp(double temperature);
    void setProfileEcoOffPeakOffTemp(double temperature);
    void setProfileEcoOffPeakFullTemp(double temperature);

    void setProfileStdPeakOffTemp(double temperature);
    void setProfileStdPeakFullTemp(double temperature);
    void setProfileStdOffPeakOffTemp(double temperature);
    void setProfileStdOffPeakFullTemp(double temperature);

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

    //Economy profile
    ESPEasyCfgParameterGroup profileEcoParamGrp_;
    ESPEasyCfgParameterGroup profileEcoHParamGrp_;
    ESPEasyCfgParameter<double> profileEcoHOffTemp_;
    ESPEasyCfgParameter<double> profileEcoHFullTemp_;
    ESPEasyCfgParameterGroup profileEcoLParamGrp_;
    ESPEasyCfgParameter<double> profileEcoLOffTemp_;
    ESPEasyCfgParameter<double> profileEcoLFullTemp_;

    //Standard profile
    ESPEasyCfgParameterGroup profileStdParamGrp_;
    ESPEasyCfgParameterGroup profileStdHParamGrp_;
    ESPEasyCfgParameter<double> profileStdHOffTemp_;
    ESPEasyCfgParameter<double> profileStdHFullTemp_;
    ESPEasyCfgParameterGroup profileStdLParamGrp_;
    ESPEasyCfgParameter<double> profileStdLOffTemp_;
    ESPEasyCfgParameter<double> profileStdLFullTemp_;

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