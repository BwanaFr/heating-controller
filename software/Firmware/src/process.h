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
    void setPeakTimeParameters(double offTemp, double fullTemp);

    /**
     * Sets the process parameter for off-peak time
    */
    void setOffPeakTimeParameters(double offTemp, double fullTemp);

    /**
     * Gets heater load in percent (0 -> 100)
    */
    inline int getLoadPercent() { return _loadPercent; }
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
};


#endif