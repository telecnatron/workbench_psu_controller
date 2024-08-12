// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "lcd.h"
#include "ina219.h"
#include "shtdwn.h"
#include "lib/lcd/lcd_i2c.h"
#include "lib/log.h"
#include "lib/task.h"

// global buffer for what is to be displayed on LCD screen.
// two rows of 16 columns, note that we add one to allow for string null terminator
char lcd_screen_buf[16*2+1];
// interval in ms at which lcd is updated
static uint16_t lcd_update_interval=2000;
// -------------------------------------
void lcd_buf_clear()
{
    memset(lcd_screen_buf,' ', sizeof(lcd_screen_buf)-1 );
}

// -------------------------------------
void lcd_buf_to_screen()
{
    lcd_i2c_gotoxy(0,0);
    for(uint8_t i=0; i< sizeof(lcd_screen_buf)-1; i++){
	// replace any '\x0' with ' '
	char c=lcd_screen_buf[i];
	if(!c) c=' ';
	lcd_i2c_putc(c);
    }
    // add a null terminator
    //lcd_screen_buf[sizeof(lcd_screen_buf)-1]='\x0';
}

// -------------------------------------
/**
 * printf-style to the lcd at the current position.
 * format is PSTR(), ie held in program memory.
 */
void lcd_printf_P(const char* format_p, ...)
{
    va_list args;
    va_start(args, format_p);
    vsnprintf_P(lcd_screen_buf, sizeof(lcd_screen_buf), format_p, args);
    va_end(args);
}


// -------------------------------------
void task_lcd_init()
{
    static uint8_t state=0;
    // init lcd and display splash screen
    switch(state){
	case 0:
	    // i2c lcd pcf8574 at i2c address 0x27
	    lcd_i2c_init(0x27, 2, 16);
	    lcd_i2c_backlight(1);
	    lcd_i2c_clear();
	    // splash screen
	    lcd_i2c_puts("Bench PSU 15V 1A");
	    task_set_seconds_timer(1);
	    state=1;
	    break;
	case 1:
	    lcd_i2c_gotoxy(0,1);
	    lcd_i2c_puts("telecnatron.com");
	    task_set_seconds_timer(2);
	    state=2;
	    break;
	case 2:
	    lcd_i2c_clear();
	    task_unready();
	    task_num_ready(TASK_LCD_RUN,1);
	    break;
	default:
	    task_unready();
    }
}

// -------------------------------------
void task_lcd_run()
{
    //lcd_buf_clear();
    LCD_PRINTF_P("%6.3fV   %5.3fA%6.3fW %7.0fJ", ina219_data.voltage, ina219_data.current, ina219_data.power, ina219_data.joules);
    //LOG_INFO_FP("'%s'", lcd_screen_buf);

    if(is_shutdown()){
	static uint8_t s=0;
	// PSU is shutdown: let them know that.
	memcpy_P(lcd_screen_buf+16, PSTR("*SHUTDOWN*"), 10);
    }
    task_set_tick_timer(lcd_update_interval);
    lcd_buf_to_screen();
}

// -------------------------------------
void task_lcd_blink()
{
    static uint8_t on=0;
    lcd_i2c_backlight(on);
    on = ! on;
    task_set_tick_timer(600);
}
