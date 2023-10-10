#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_
#include <cstdint>


/**
 * Setups the I/O (digital and analog)
*/
void setup_inputs_outputs();

double getExternalTemperature(uint32_t& milliVolts, double& resistance);

inline double getExternalTemperature() {
    uint32_t mV = 0.0;
    double res = 0;
    return getExternalTemperature(mV, res);
};

double getFloorTemperature(uint32_t& milliVolts, double& resistance);

inline double getFloorTemperature() {
    uint32_t mV = 0.0;
    double res = 0;
    return getFloorTemperature(mV, res);
};

/**
 * Sets relay output
 * @param status True if relay will be closed (heating ON)
*/
void setRelay(bool status);

/**
 * Sets the user LED status
 * @param status True if the LED is ON
*/
void setUserLed(bool status);

/**
 * Gets tariff input state
 * @return true if input is active (230V on it)
*/
bool getTariffInput();

/**
 * Sets number of averages for temperature reading
*/
void setNbAverages(int averages);

/**
 * Sets the ADC voltage reference
*/
void setADCVRef(double vrefmv);

#endif
