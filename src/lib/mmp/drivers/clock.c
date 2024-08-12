// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include "config.h"
#include "../../task.h"
#include "../../rtc/clock.h"
#include "../mmp_cmd.h"
#include "../../log.h"
#include "../../sysclk.h"
#include <string.h>

extern clock_t clock;

// make the clock clock tick
void task_clock()
{
    task_set_seconds_timer(1);
    clock_tick();
}


void cmd_clock(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{

    uint8_t subcmd=data[0];
    //LOG_INFO_FP("%s:%s: cmd_clock: cmd: 0x%x, rsubcmd: 0x%x, dmax: %i, dlen: %i: d123: %x, %x, %x, %x", __FILE__,__LINE__, cmd, subcmd, data_max_len, data_len, data[0], data[1], data[2], data[3]);
    
    switch(subcmd){
	// assume failure
	uint8_t status=1;
	uint8_t rlen;
	// -------------------------------------------
	case 0:
	    // tell the time
	    // reply:  clock_t, sysclk_ticks: uint16, sysclk_seconds: uint32, sysclk_seconds_count: uint32, sysclk_freq: uint16_t
	    rlen =sizeof(clock_t)+sizeof(uint16_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint16_t);
	    if (data_max_len >= rlen ){
		// clock
		memcpy(reply_data, &clock, sizeof(clock_t));
		reply_data+=sizeof(clock_t);
		// sysclk_ticks		
		*((uint16_t *)reply_data) = sysclk_get_ticks();
		reply_data+=sizeof(uint16_t);
		// sysclk_seconds
		*((uint32_t *)reply_data) = sysclk_get_seconds();
		reply_data+=sizeof(uint32_t);
		// sysclk_seconds_count
		*((uint32_t *)reply_data) = sysclk_get_seconds_count();
		reply_data+=sizeof(uint32_t);
		// sysclk_freq
		*((uint16_t *)reply_data) = sysclk_get_tick_freq();
		status=0;
	    }else{
		// reply would exceed data_max_len
		status =1;
	    }
	    mmp_cmd_reply(handle, status, rlen);
	    break;
	    
	// -------------------------------------------	    
	case 1:
	    // they want us to set our time to the time given in data
	    if(data_len == sizeof(clock_t)+1){
		memcpy(&clock, data+1, sizeof(clock_t));
		LOG_INFO_FP("%s:%u: clock set to: %02u:%02u:%02u", __FILE__, __LINE__,clock.hour, clock.minute, clock.second);
	    }
	    // XXX steves: Thu 25 Jan 2024 11:28:24: this currently alwasy sets reply status to 0
	    mmp_cmd_reply(handle, 0, sizeof(clock_t));
	    break;
    	// -------------------------------------------
	case 2:
	    // set sysclk_seconds
	    // data should contain uint32_t being the seconds to set
	    if(data_max_len >= sizeof(uint32_t)){
		uint32_t *ssec=(uint32_t *)(data+1);
		sysclk_set_seconds(*ssec);
		LOG_INFO_FP("%s:%u: set sysclk_seconds to %lu", __FILE__, __LINE__, sysclk_get_seconds());
		status=0;
	    }else{
		status =1;
	    }
	    mmp_cmd_reply(handle, status, 0);
	    break;
	// -------------------------------------------
	case 3:
	    // set sysclk_tick_freq
	    if(data_max_len >= sizeof(uint16_t)){
		uint16_t *f = (uint16_t *)(data+1);
		sysclk_set_tick_freq(*f);
		LOG_INFO_FP("%s:%u: set sysclk_tick_freq to %u", __FILE__, __LINE__, sysclk_get_tick_freq());
		status=0;
	    }else{
		// reply would exceed data_max_len
	    }
	    mmp_cmd_reply(handle, status, 0);
	    break;

	    
	// -------------------------------------------
	default:
	    // we don't know about that command,
	    mmp_cmd_reply(handle, 2, 0);
    }
    

}
