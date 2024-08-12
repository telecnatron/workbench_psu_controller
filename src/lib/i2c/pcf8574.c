// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------

#include "config.h"
#include "pcf8574.h"
#include "./i2c_master.h"
#include "../log.h"

// -----------------------------------------------------
void pcf8574_write(uint8_t addr, uint8_t byte)
{
    uint8_t r=i2c_start(addr << 1 | I2C_WRITE);
    if(!r){
	// found device at this address
	//LOG_INFO_FP("addr: 0x%x, start returned %u",addr, r);
	i2c_write(byte);
    }
    i2c_stop();
}


// -----------------------------------------------------
uint8_t pcf8574_read(uint8_t addr)
{
    i2c_start_wait(addr << 1 | I2C_READ);
    uint8_t data=~i2c_readNak();
    i2c_stop();
    return data;
}
