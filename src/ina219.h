// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#ifndef _INA219_CTRL_H
#define _INA219_CTRL_H 1


#ifndef INA219_DEFS
#warning Using default config for ina219 .
// ----------------
// To override, define these in (eg) config.h and also define INA219_DEFS
// i2c address of device
#define INA219_ADDR 0x40
// period of time between measurements in ms
#define INA219_MEASUREMENT_PERIOD_MS 50
// ----------------
#endif
#define INA219_MEASUREMENTS_PER_SECOND 1000/INA219_MEASUREMENT_PERIOD_MS

// structure for measurement data 
typedef struct {
    // most recently read voltage in volts
    float voltage;
    // most recently read current in amps
    float current;
    // most recent voltage multiplied by most recent current
    float power;
    // count of number of joules that have been expended.
    float joules;
    
    // averages for power to calculate joules
    float   _power_sum;
    uint8_t _power_num;
    uint32_t _start_seconds;
} ina219_t ;

//global  measurement data: volts, amps etc
extern ina219_t ina219_data;

//!
void ina219_init();

#endif /* _INA219_CTRL_H */

