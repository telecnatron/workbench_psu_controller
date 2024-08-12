// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
//
// INA291 Bidirectional i2c current and voltage sensor
// datasheet: https://www.ti.com/lit/ds/symlink/ina219.pdf
// -----------------------------------------------------------------------------
#include "config.h"
#include "./ina219.h"
#include "lib/i2c/i2c_master.h"
#include "lib/log.h"


uint16_t INA219_read_register(uint8_t addr, uint8_t reg)
{
    uint16_t data;
    // note that we swap the endianness
    uint8_t *p = (uint8_t *)(&data)+1;
    // address the register
    I2C_START_WRITE(addr);
    i2c_write(reg);
    i2c_stop();
    // read the addressed register
    I2C_START_READ(addr);
    // msb
    *p=i2c_readAck();
    p--;
    // lsb
    *p=i2c_readNak();
    return data;
	
}


void INA219_write_register(uint8_t addr, uint8_t reg, uint16_t data)
{
    // note that we swap the endianness
    uint8_t *p = (uint8_t *)(&data)+1;
    I2C_START_WRITE(addr);
    // address the register
    i2c_write(reg);
    // msb
    i2c_write(*p);
    p--;
    // lsb
    i2c_write(*p);
    i2c_stop();
}


