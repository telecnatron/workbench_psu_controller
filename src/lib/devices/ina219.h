// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
//
// INA291 Bidirectional i2c current and voltage sensor
// datasheet: https://home.ss/doc/data/datasheets/misc_ics/sensors/ina219-current-voltage-sensor.pdf
// -----------------------------------------------------------------------------
#ifndef _INA219_H
#define _INA219_H 1

#include <stdint.h>

// ina219 register indexes
#define INA219_REG_CONFIG        0x0 
#define INA219_REG_SHUNT_VOLTAGE 0x1
#define INA219_REG_BUS_VOLTAGE   0x2
#define INA219_REG_POWER         0x3
#define INA219_REG_CURRENT       0x4
#define INA219_REG_CALIBRATION   0x5

/** 
 @brief read the specified register and return its value
 @param    addr address of i2c deviceand transfer direction of I2C device
 @param    reg the register to be read, one of the INA219_REG_XXX defines
 */
uint16_t INA219_read_register(uint8_t addr, uint8_t reg);
//!
void INA219_write_register(uint8_t addr, uint8_t reg, uint16_t data);

// convenience macros for reading the registers.
#define INA219_READ_CONFIG(addr)        INA219_read_register(addr, INA219_REG_CONFIG)
#define INA219_READ_SHUNT_VOLTAGE(addr) INA219_read_register(addr, INA219_REG_SHUNT_VOLTAGE )
#define INA219_READ_BUS_VOLTAGE(addr)   INA219_read_register(addr, INA219_REG_BUS_VOLTAGE   )
#define INA219_READ_POWER(addr)         INA219_read_register(addr, INA219_REG_POWER         )
#define INA219_READ_CURRENT(addr)       INA219_read_register(addr, INA219_REG_CURRENT       )
#define INA219_READ_CALIBRATION(addr)   INA219_read_register(addr, INA219_REG_CALIBRATION   )

#define INA219_WRITE_CONFIG(addr, data)      INA219_write_register(addr, INA219_REG_CONFIG, data)
#define INA219_WRITE_CALIBRATION(addr, data) INA219_write_register(addr, INA219_REG_CALIBRATION, data)
// device is reset by setting bit 15 of the config register
#define INA219_RESET(addr)    INA219_write_register(addr, INA219_REG_CONFIG, 0x8000)

// Config register settings bitmasks.
// See datasheet section 8.6.2
// These defines are from: https://github.com/adafruit/Adafruit_INA219/blob/master/Adafruit_INA219.h

// BRNG bit: bus voltage FSR
#define INA219_CONFIG_BUS_RANGE_16V  (0x0000)
#define INA219_CONFIG_BUS_RANGE_32V  (0x2000)

// PGA bits. Shunt voltage gain.
#define INA219_CONFIG_PGA_1_40MV   (0x0000)
#define INA219_CONFIG_PGA_2_80MV   (0x0800)
#define INA219_CONFIG_PGA_4_160MV  (0x1000)
#define INA219_CONFIG_PGA_8_320MV  (0x1800)

// Bus ADC resolution, (number of samples)
#define INA219_CONFIG_BADC_RES_9BIT (0x0000)
#define INA219_CONFIG_BADC_RES_10BIT (0x0080)
#define INA219_CONFIG_BADC_RES_11BIT (0x0100)
#define INA219_CONFIG_BADC_RES_12BIT (0x0180)
#define INA219_CONFIG_BADC_RES_12BIT_1S (0x0180)
// 1060us conversion time
#define INA219_CONFIG_BADC_RES_12BIT_2S   (0x0480)
// 2130us conversion time
#define INA219_CONFIG_BADC_RES_12BIT_4S   (0x0500)
// 4240us conversion time
#define INA219_CONFIG_BADC_RES_12BIT_8S   (0x0580)
// 8510us conversion time
#define INA219_CONFIG_BADC_RES_12BIT_16S  (0x0600)
// 17ms conversion time
#define INA219_CONFIG_BADC_RES_12BIT_32S  (0x0680)
// 34ms conversion time
#define INA219_CONFIG_BADC_RES_12BIT_64S  (0x0700)
// 69ms conversion time
#define INA219_CONFIG_BADC_RES_12BIT_128S (0x0780)

// Shunt ADC resolution, (number of samples)
// 84us conversion time
#define INA219_CONFIG_SADC_RES_9BIT_1S    (0x0000)
// 148us conversion time
#define INA219_CONFIG_SADC_RES_10BIT_1S   (0x0008)
// 276us conversion time
#define INA219_CONFIG_SADC_RES_11BIT_1S   (0x0010)
// 532us conversion time
#define INA219_CONFIG_SADC_RES_12BIT_1S   (0x0018)
// 1060us conversion time
#define INA219_CONFIG_SADC_RES_12BIT_2S   (0x0048)
// 2130us conversion time
#define INA219_CONFIG_SADC_RES_12BIT_4S   (0x0050)
// 4260us conversion time
#define INA219_CONFIG_SADC_RES_12BIT_8S   (0x0058)
// 8510us conversion time
#define INA219_CONFIG_SADC_RES_12BIT_16S  (0x0060)
// 17ms conversion time
#define INA219_CONFIG_SADC_RES_12BIT_32S  (0x0068)
// 34ms conversion time
#define INA219_CONFIG_SADC_RES_12BIT_64S  (0x0070)
// 69ms conversion time
#define INA219_CONFIG_SADC_RES_12BIT_128S (0x0078)

// mode bits
// datasheet table 6, page 20
#define INA219_CONFIG_MODE_POWERDOWN        0x00
#define INA219_CONFIG_MODE_SHUNT_TRIGGERED  0x01
#define INA219_CONFIG_MODE_BUS_TRIGGERED    0x02
#define INA219_CONFIG_MODE_SB_TRIGGERED     0x03
#define INA219_CONFIG_MODE_ADC_OFF          0x04
#define INA219_CONFIG_MODE_SHUNT_CONTINUOUS 0x05
#define INA219_CONFIG_MODE_BUS_CONTINUOUS   0x06
#define INA219_CONFIG_MODE_SB_CONTINUOUS    0x07

#endif /* _INA219_H */

