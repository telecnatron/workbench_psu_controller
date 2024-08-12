// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include <string.h>
#include "config.h"
#include "load_switch.h"
#include "./lib/adc.h"
#include "./lib/mmp/mmp_cmd.h"
#include "./lib/task.h"
#include "./lib/uart/uart.h"

// global variables.
// 1: load is connected, 0 load is disconnected, -1 is uninitalised
int8_t load_switch_status=-1;


// -------------------------------------------------------------------
void load_switch_init(uint8_t adc_channel)
{
    adc_init_avcc(0);
#if 0    
    ADC_CHANNEL_SELECT(0);
    ADC_INTERRUPT_ENABLE();
    ADC_START_CONVERSION();
#endif    
}

// -------------------------------------------------------------------
#if 0
// handler for ADC interrrupt
ISR(ADC_vect)
{
    task_num_ready(TASK_LOAD_SWITCH, 1);
}
#endif

// -------------------------------------------------------------------
// mmp command handler: read status of load switch
void cmd_load_switch(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    memcpy(reply_data, &load_switch_status, sizeof(load_switch_status));
    mmp_cmd_reply(handle, 0, sizeof(load_switch_status));
}

// -------------------------------------------------------------------
//! monitor state of load switch, signal when changed
void task_load_switch()
{
    uint16_t ls=adc_read_poll(0);
    if(ls>0x0120){
	// it's on
	ls=1;
    }else if(ls<0x0080){
	// it's off
	ls=0;
    }
    if(ls != load_switch_status){
	// load switch has toggled
	load_switch_status=ls;
	//LOG_INFO_FP("load: 0x%02x, 0x%02x", ls, load_switch_status);	
	// send them async message of two bytes, being {5, load_switch_status}
	uint8_t d[2]={5, load_switch_status};
	mmp_async_send(d, 2, uart_putc);
    }
    task_set_tick_timer(80);
}

