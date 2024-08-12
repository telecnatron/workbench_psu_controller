// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include "config.h"
#include "../mmp_cmd.h"
#include "../../log.h"
#include "../../lcd/lcd_i2c.h"
#include <string.h>

// subcommands
#define LCD_SC_BACKLIGHT  0
#define LCD_SC_PUTS       1
#define LCD_SC_GOTOXY     2
#define LCD_SC_CLEAR      3

void cmd_lcd(void *handle, uint8_t cmd, uint8_t data_len, uint8_t data_max_len, uint8_t *data, uint8_t *reply_data)
{
    uint8_t subcmd=data[0];
    // assume failure
    uint8_t status = 1;
    //LOG_INFO_FP("cmd_lcd: cmd: %u, subcmd: %u, len: %u, d1: 0x%x, d2: 0x%x", cmd, subcmd, data_len, data[1], data[2]);
    
    switch(subcmd){
	// -------------------------------------------
	case LCD_SC_BACKLIGHT:
	    // control backlight
	    // data[1]==0 then backlight off, otherwise on
	    lcd_i2c_backlight(data[1]);
	    mmp_cmd_reply(handle, 0, 0);
	    break;
	// -------------------------------------------
	case LCD_SC_PUTS:
	    // write passed string to lcd
	    // just in case string is not null terminated
	    data[data_len-1]=0;
	    lcd_i2c_puts((char *)(data+1));
	    mmp_cmd_reply(handle, 0, 0);
	    break;
	// -------------------------------------------
	case LCD_SC_GOTOXY:
	    // goto xy
	    if(data_len == 3){
		lcd_i2c_gotoxy(data[1],data[2]);
		status=0;
	    }
	    mmp_cmd_reply(handle, status, 0);
	    break;
	// -------------------------------------------
	case LCD_SC_CLEAR:
	    // clear screen and move cursor to home position
	    lcd_i2c_clear();
	    lcd_i2c_home();
	    mmp_cmd_reply(handle, 0, 0);
	    break;
        // -------------------------------------------
	default:
	    // we don't know about that command,
	    mmp_cmd_reply(handle, 2, 0);
    }
}



