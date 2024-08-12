// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include "config.h"
#include "../mmp_cmd.h"
#include "../../log.h"
#include "../../i2c/pcf8574.h"
#include <string.h>

void cmd_pcf8574(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    uint8_t subcmd=data[0];
    // i2c address fo the pcf8574 device
    const uint8_t pcf8574_addr=0x27;
    uint8_t status=1;
    
    switch(subcmd){
	// -------------------------------------------
	case 0:
	    // set output to passed byte
	    if(data_len == 2){
		pcf8574_write(pcf8574_addr, (uint8_t)data[1]);
		LOG_INFO_FP("pcf set: 0x%x",(uint8_t)data[1]);
		status=0;
	    }else{
		// invalid data length
	    }
	    mmp_cmd_reply(handle, status, 0);
	    break;
	    // -------------------------------------------
	case 1:
	    // get input pins from device
	    //LOG_INFO_FP("pcf get: 0x%x",(uint8_t)data[1]);
	    reply_data[0]=pcf8574_read(pcf8574_addr);
	    //LOG_INFO_FP("pcf got: 0x%x",(uint8_t)data[1]);
	    status=0;
	    mmp_cmd_reply(handle, status, sizeof(uint8_t));
	    break;
	// -------------------------------------------
	default:
	    // we don't know about that command,
	    mmp_cmd_reply(handle, 2, 0);
    }
}


