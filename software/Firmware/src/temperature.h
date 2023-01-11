#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include <cstdint>

double getExternalTemperature(double& milliVolts, uint32_t& resistance);

inline double getExternalTemperature() {
    double mV = 0.0;
    uint32_t res = 0;
    return getExternalTemperature(mV, res);
};

double getFloorTemperature(double& milliVolts, uint32_t& resistance);

inline double getFloorTemperature() {
    double mV = 0.0;
    uint32_t res = 0;
    return getFloorTemperature(mV, res);
};

#endif