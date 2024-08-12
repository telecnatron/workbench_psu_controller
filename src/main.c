// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include <avr/interrupt.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
// library includes
#include "./lib/i2c/i2c_master.h"
#include "./lib/i2c/pcf8574.h"
#include "./lib/lcd/lcd_i2c.h"
#include "./lib/log.h"
#include "./lib/mmp/mmp_cmd.h"
#include "./lib/rtc/clock.h"
#include "./lib/stdout.h"
#include "./lib/sysclk.h"
#include "./lib/task.h"
#include "./lib/uart/uart.h"
#include "./lib/util/io.h"
//#include "./lib/devices/ina219.h"

// local includes
#include "config.c"
#include "config.h"
#include "ina219.h"
#include "lcd.h"
#include "load_switch.h"
#include "shtdwn.h"

// -------------------------------------
// globals

// 0: psu is running, else psu is shutdown
// int8_t shtdwn_status = 0;

// -------------------------------------
// function forward declerations
void _uart_init();

// -------------------------------------
void user_init()
{
    // onboard LED
    LED_INIT_OUTPUT();
    LED_ON();

    // fan
    FAN_INIT_OUTPUT();

    // shutdown psu
    shutdown_init();
    
    // i2c stuff
    i2c_init();
    LOG_INFO_FP("i2c is initaliased.", NULL);
    i2c_enumerate();
    // current and voltage sensor:
    ina219_init();
    
    // load switch - we're reading this via ADC, channel0
    load_switch_init(0);

}
// -------------------------------------
void task_led()
{
    static uint8_t lc=0;
    task_set_tick_timer(250);
    lc++;
    if(lc == 4){
	lc=0;
	LED_TOGGLE();
    }
}

// -------------------------------------
int main()
{
    // sysclk ticker, uses timer2
    sysclk_init();
    // uart and stdout
    _uart_init();
    LOG_INFO_FP(" --- INITIALISING --- ", NULL);
    // tasks
    init_tasks();
    // logger
    log_set_level(LOG_LEVEL_INFO);
    // message handler
    init_mmp_cmd();
    // call user init function
    user_init();
    // enable global interrupts
    sei();
    
    LOG_INFO_FP(" --- RUNNING --- ", NULL);
    // -------------- main loop ---------------
    for(;;){
	// process any chars available from uart
	while(GETC_AVAIL()){
	    // and pass them to mmp_cmd
	    mmp_cmd_rx_ch(&mmp_cmd_ctrl, GETC());
	}

	if(sysclk_has_ticked()){
	    // this block is called at ~1000Hz
	    mmp_cmd_tick(&mmp_cmd_ctrl);
	    task_tick();
	    if( sysclk_have_seconds_ticked()){
		// this block called every second or so
		task_seconds_tick();
	    }
	    task_run();
	}
    }
}

//! enable uart, set up stdout
void _uart_init()
{
    // buffer for uart rx buffer
    static char rxbuf[UART_RXBUF_SIZE];
    // init uart
    uart_init(rxbuf,UART_RXBUF_SIZE);
    STDOUT_INIT();
}



