// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "./lib/mmp/mmp_cmd.h"
#include "./lib/log.h"
#include "adc.h"
#include "config.h"



// -------------------------------------------------------------------
// turn fan on or off
void cmd_fan(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    // data[0] == 0 then fan is turned off, otherwise it is turned on
    if(data[0]==0)
	FAN_OFF();
    else
	FAN_ON();
    mmp_cmd_reply(handle, 0, 0);
}






