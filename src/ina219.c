// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include <string.h>

#include "lib/devices/ina219.h"
#include "lib/mmp/mmp_cmd.h"
#include "lib/log.h"
#include "lib/sysclk.h"
#include "lib/task.h"

#include "config.h"
#include "ina219.h"
#include "lcd.h"

// -------------------------------------------------
// globals
// i2c address of ina219 device
uint8_t ina219_addr = INA219_ADDR;
// measurement data
ina219_t ina219_data;

// -------------------------------------------------
void ina219_init()
{
    // reset the device
    INA219_RESET(ina219_addr);

    // write the configuration to it.
    // 16V FSR, PGA x4, 12 bits 4 samples, read both bus and shunt continiously.
    // conversion time is 2130us for each of bus and shunt ADCs
    INA219_WRITE_CONFIG(ina219_addr, INA219_CONFIG_BUS_RANGE_16V \
    			           | INA219_CONFIG_PGA_4_160MV \
			           | INA219_CONFIG_BADC_RES_12BIT_4S \
			           | INA219_CONFIG_SADC_RES_12BIT_4S \
			           | INA219_CONFIG_MODE_SB_CONTINUOUS );
}

// -------------------------------------------------
void task_ina219()
{
    int16_t reg;
    static uint8_t state=0;
    if(state){
	// read bus voltage
	reg = INA219_READ_BUS_VOLTAGE(ina219_addr);
	// to calculate volts:
	//   FSR(here=16V) * vbus_reg / 2^15
	ina219_data.voltage= 16.0 * reg / 32768;
	//LOG_INFO_FP("vbus reg: 0x%04x volts: %6.3fV", reg, voltage);
    }else{
	// read shunt voltage
	reg = INA219_READ_SHUNT_VOLTAGE(ina219_addr);
	// calculate amps
	// here fullscale corresponds to 160mV drop across shunt.
	// Vshunt = 0.16 * vshunt_reg / 2^15
	// current = Vshunt / Rshunt
	ina219_data.current= 0.16 * reg / 32768 /0.1 *2;
	//LOG_INFO_FP("shunt reg: 0x%04x, %i amps: %fA", reg,reg, current);	
    }
    state=!state;
    ina219_data.power = ina219_data.current * ina219_data.voltage;
    // keep values for averaging power so energy can be calculated
    ina219_data._power_sum +=  ina219_data.power;
    ina219_data._power_num++;
    // ina219_data.joules += (ina219_data.power * INA219_MEASUREMENT_PERIOD_MS/1000);
    task_set_tick_timer(INA219_MEASUREMENT_PERIOD_MS);
}

// -------------------------------------------------
void ina219_calc_energy()
{
    // num seconds since last calculation
    uint32_t secs = sysclk_get_seconds_count() - ina219_data._start_seconds;
    //LOG_INFO_FP("secs: %u", secs);
    // reset seconds to now
    ina219_data._start_seconds = sysclk_get_seconds_count();
    // calc average power 
    float pa=0;
    if (ina219_data._power_num >0)
	pa=ina219_data._power_sum / ina219_data._power_num;
    //LOG_INFO_FP("pa: %f, dj: %f", pa, pa*secs);    
    // reset count and sum
    ina219_data._power_num = 0;
    ina219_data._power_sum = 0;
    // energy since last calculation = average_power (J/s) * number of seconds
    ina219_data.joules +=  pa * secs;
}

// -------------------------------------------------
void task_energy()
{
    ina219_calc_energy();
    // every 5 seconds
    task_set_seconds_timer(5);
    // work out whether fan should be on or off and then turn it on or off accordingly:
    // power being dissipated by regulating transistor.
    // hs_power = (unreg_dc_volts(= ~15V) - output voltage) * current
    float hs_power = (16 - ina219_data.voltage) * ina219_data.current;
    // more than about 2W being dissipated in heat sink then turn fan on, or off otherwise.
    if (hs_power > 2.1){
	FAN_ON();
    }else if(hs_power < 1.9){
	FAN_OFF();
    }
}

// -------------------------------------------------
/** 
 * Send MMP reply message containing voltage, current, power etc
 * The reply data is copy of ina219_t contained in the ina219_data global variable
 *
 * @param handle MMP handle to pass to call to mmp_cmd_reply()
 * @param cmd The MMP command number
 * @param data_len Number of bytes of data pointed to by 'data' parameter
 * @param data_max_len  Max length of buffer pointed to by reply_data
 * @param data  The data being passed to this function
 * @param reply_data Pointer to the reply data that will be returned in the mmp reply message.
 */
void cmd_measurements(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    uint8_t status=1;
    uint8_t rsize=0;
    uint8_t subcmd=data[0];
    switch(subcmd){
	case 0:
	    // read the data
	    if(data_max_len >= sizeof(ina219_t)){
		memcpy(reply_data, &(ina219_data.voltage), sizeof(float));
		reply_data+=sizeof(float);
		memcpy(reply_data, &(ina219_data.current), sizeof(float));
		reply_data+=sizeof(float);
		memcpy(reply_data, &(ina219_data.power), sizeof(float));
		reply_data+=sizeof(float);
		memcpy(reply_data, &(ina219_data.joules), sizeof(float));
		reply_data+=sizeof(float);
		status=0;
		rsize=sizeof(float)*4;
	    }
	    break;
	case 1:
	    // reset joules counter
	    ina219_data.joules=0;
	    status=0;
	    break;
    }
    mmp_cmd_reply(handle, status, rsize);
}
