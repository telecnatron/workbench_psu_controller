// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2024. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include "config.h"
#include "../../rtc.h"
#include "../mmp_cmd.h"
#include "../../log.h"
#include <string.h>


void cmd_ds1302_rtc(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{

    uint8_t subcmd=data[0];
    //LOG_INFO_FP("%s:%s: cmd_ds1302_rtc: cmd: 0x%x, rsubcmd: 0x%x, dmax: %i, dlen: %i: d0123: %x, %x, %x, %x", __FILE__,__LINE__, cmd, subcmd, data_max_len, data_len, data[0], data[1], data[2], data[3]);
    
    switch(subcmd){
	// assume failure
	uint8_t status=1;
	uint8_t rlen;
	// -------------------------------------------
	case 0:
	    // tell them the current time, ie return rtc_datetime_t containing the current time
	    rlen=sizeof(rtc_datetime_t);
	    if (data_max_len > rlen){
		RTC_READ((rtc_datetime_t *)reply_data);
		status=0;
	    }else{
		status=1;
	    }
	    mmp_cmd_reply(handle, status, rlen);
	    break;

	// -------------------------------------------	    
	case 1:
	    // set RTC time to that passed
	    if(data_len == sizeof(rtc_datetime_t)+1){
		RTC_WRITE((rtc_datetime_t *)(data+1));
		status=0;
	    }else{
		status=1;
	    }
	    mmp_cmd_reply(handle, status, sizeof(rtc_datetime_t));
	    break;
    }
}
