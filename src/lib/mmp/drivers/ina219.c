// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
//
// mmp commands to directly interact with ina219 device over i2c
//
#include "config.h"
#include "../mmp_cmd.h"
#include "../../devices/ina219.h"
#include "../../log.h"


// subcommands
#define SC_INA219_READ_CONFIG         0
#define SC_INA219_READ_SHUNT_VOLTAGE  1
#define SC_INA219_READ_BUS_VOLTAGE    2
#define SC_INA219_READ_POWER          3
#define SC_INA219_READ_CURRENT        4
#define SC_INA219_READ_CALIBRATION    5
#define SC_INA219_WRITE_CONFIG        6
#define SC_INA219_WRITE_CALIBRATION   7


void cmd_ina219(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    // first byte of data is the sub command number
    uint8_t subcmd=data[0];
    // second is i2c address
    uint8_t addr=data[1];

    // skip over subcommand and i2c address
    uint16_t *d=(uint16_t *)(data+2);

    // assume failure
    uint8_t status = 1;
    //LOG_INFO_FP("cmd_lcd: cmd: %u, subcmd: %u, len: %u, d1: 0x%x, d2: 0x%x", cmd, subcmd, data_len, data[1], data[2]);
    // reply lenght 
    uint8_t rl=sizeof(uint16_t);
    
    switch(subcmd){
	// -------------------------------------------
	case SC_INA219_READ_CONFIG:
	    if(data_max_len > sizeof(uint16_t)){
		*((uint16_t *)reply_data) = INA219_READ_CONFIG(addr);
		//LOG_INFO_FP("read conf: %x %x", reply_data[0], reply_data[1]);
		status=0;
	    }
	    break;

	// -------------------------------------------
	case SC_INA219_READ_SHUNT_VOLTAGE:
	    if(data_max_len > sizeof(uint16_t)){
		*((uint16_t *)reply_data) = INA219_READ_SHUNT_VOLTAGE(addr);
		status=0;
	    }
	    break;
	// -------------------------------------------	    
	case SC_INA219_READ_BUS_VOLTAGE  :
	    if(data_max_len > sizeof(uint16_t)){
		uint16_t rbv=INA219_READ_BUS_VOLTAGE(addr);
		*((uint16_t *)reply_data) = rbv;
		LOG_INFO_FP("read conf: 0x%02x", rbv );
		status=0;		
	    }
	    break;
	// -------------------------------------------	    
	case SC_INA219_READ_POWER        :
	    if(data_max_len > sizeof(uint16_t)){
		*((uint16_t *)reply_data) = INA219_READ_POWER(addr);
		status=0;		
	    }
	    break;
	// -------------------------------------------	    
	case SC_INA219_READ_CURRENT      :
	    if(data_max_len > sizeof(uint16_t)){
		uint16_t c=INA219_READ_CURRENT(addr);
		uint8_t *p;
		p = (uint8_t *)&c;
		LOG_INFO_FP("read current: 0x%x %x", *p, *(p+1));
		*((uint16_t *)reply_data) = INA219_READ_CURRENT(addr);
		status=0;
	    }
	    break;
	// -------------------------------------------	    
	case SC_INA219_READ_CALIBRATION  :
	    if(data_max_len > sizeof(uint16_t)){
		*((uint16_t *)reply_data) = INA219_READ_CALIBRATION(addr);
		status=0;		
	    }
	    break;
	    
        // -------------------------------------------
	case SC_INA219_WRITE_CALIBRATION:
	    //LOG_INFO_FP("write cal: %x ", *d);
	    INA219_WRITE_CALIBRATION(addr, *d);
	    status=0;
	    break;
	    
        // -------------------------------------------
	case SC_INA219_WRITE_CONFIG:
	    //LOG_INFO_FP("write conf: %x", *d);
	    INA219_WRITE_CONFIG(addr, *d);
	    status=0;
	    break;

        // -------------------------------------------
	default:
	    // we don't know about that command,
	    status=2;
	    rl=0;
    }
    // send the reply message
    mmp_cmd_reply(handle, status, rl);
}

