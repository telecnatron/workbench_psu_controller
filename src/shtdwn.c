// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include "config.h"
#include "shtdwn.h"
#include "lcd.h"
#include "./lib/mmp/mmp_cmd.h"    
#include "./lib/uart/uart.h"
#include "./lib/log.h"
#include "./lib/task.h"

static uint8_t shtdwn = 0;

// -------------------------------------------------------------------
void shutdown_init()
{
    SHTDWN_INIT_OUTPUT();
    SHTDWN_OFF();
    shtdwn=0;
}
// -------------------------------------------------------------------
uint8_t is_shutdown()
{
    return shtdwn;
}
// -------------------------------------------------------------------
uint8_t psu_shutdown(uint8_t sht)
{
    if(sht){
	// shut it down
	SHTDWN_ON();
	// make the lcd backlight blink
	//task_num_ready(TASK_LCD_BLINK,1);
    }else{
	// restart
	SHTDWN_OFF();
	// make the lcd backlight not blink
	//task_num_ready(TASK_LCD_BLINK,0);
    }
    shtdwn=sht;
    // send async message notifying of change 
    uint8_t d[2]={7, sht};
    mmp_async_send(d, 2, uart_putc);
    return shtdwn;
}

// -------------------------------------------------------------------
void cmd_shtdwn(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    uint8_t status=1;
    if(data_len == 1){
	switch(data[0]){
	    case 0:
		// unshutdown (restart) psu
		status=0;
		psu_shutdown(0);
		break;
	    case 1:
		// shutdown psu
		status=0;
		psu_shutdown(1);
		break;
	    case 2:
		// get shutdown status
		status=0;
		break;
	}
    }
    reply_data[0]=shtdwn;
    mmp_cmd_reply(handle, status, 1);
    LOG_INFO_FP("psu_shutdown: %u", shtdwn);
}


