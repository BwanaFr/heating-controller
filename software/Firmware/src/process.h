#ifndef _PROCESS_H_
#define _PROCESS_H_

/**
 * Controls of the heating process
*/
class Process{

public:
    Process();
    virtual ~Process() = default;

    /**
     * Sets io values read from probes
     * and compute relay status
     * @param external External temperature value
     * @param floor Floor temperature value
     * @return Relay status (true when powered)
    */
    bool setIOState(double external, double floor, bool peakTime);

    /**
     * Sets timebase of the process
    */
    void setTimebase(int timebase);

    /**
     * Sets the temperature limiter value
    */
    inline void setTemperatureLimit(double limit) { _tempLimit = limit; }

    /**
     * Sets the process parameter for peak time
    */
    void setPeakTimeOffTemperature(double temperature);

    /**
     * Sets the process parameter for peak time
    */
    void setPeakTimeFullTemperature(double temperature);
 
    /**
     * Sets the process parameter for off-peak time
    */
    void setOffPeakTimeOffTemperature(double temperature);

    /**
     * Sets the process parameter for off-peak time
    */
    void setOffPeakTimeFullTemperature(double temperature);

    /**
     * Gets heater load in percent (0 -> 100)
    */
    inline int getLoadPercent() { return _loadPercent; }

    /**
     * Gets relay state
    */
    inline bool getRelayState() { return _relayState; }

    /**
     * Gets peak time state
    */
    inline bool getPeakTimeState() { return _peakTime; }

    /**
     * Gets floor temperature
    */
    inline double getFloorTemperature() { return _floorTemp; }

    /**
     * Gets external temperature
    */
    inline double getExternalTemperature() { return _externalTemp; }

    /**
     * Gets pointer to instance
    */
    static inline Process* getInstance() { return instance_; }
    
private:
    int _timebase;              //Timebase in seconds
    double _ptOffTemp;          //Peak time off temperature (0%)
    double _ptFullTemp;         //Peak time full temperature (100%)
    double _optOffTemp;         //Off-peak time off temperature (0%)
    double _optFullTemp;        //Off-peak time full temperature (100%)
    double _tempLimit;          //Floor temperature limiter
    int _loadPercent;           //Heating load in percent    
    unsigned long _cycleStart;  //Start of a new cycle
    bool _relayState;           //Relay state cache
    bool _peakTime;             //Peak time active
    double _floorTemp;          //Floor temperature
    double _externalTemp;       //External temperature
    bool _limiterActive;        //Is limiter active
    static Process* instance_;  //Singleton
};


#endif