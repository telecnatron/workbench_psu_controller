// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing. 2014.
// $Id: $
// Interface for AVR processors to Nokia 5110 LCD module
// -----------------------------------------------------------------------------
#include "lcd_5110.h"
#include "lib/util/io.h"
#include "util/delay.h"
#include LCD_5110_FONT_PATH
#include <avr/pgmspace.h>

// bit helpers
// SCE enable serial interface, active low
#define LCD_5110_SCE_HI()     PIN_HI(LCD_5110_PORT, LCD_5110_PIN_SCE)
#define LCD_5110_SCE_LO()     PIN_LO(LCD_5110_PORT, LCD_5110_PIN_SCE)
#define LCD_5110_SCE_ASSERT()   LCD_5110_SCE_LO()
#define LCD_5110_SCE_DEASSERT() LCD_5110_SCE_HI()
// reset
#define LCD_5110_RESET_HI()   PIN_HI(LCD_5110_PORT, LCD_5110_PIN_RESET)
#define LCD_5110_RESET_LO()   PIN_LO(LCD_5110_PORT, LCD_5110_PIN_RESET)
// set command or data mode
#define LCD_5110_CMD_MODE()        PIN_LO(LCD_5110_PORT, LCD_5110_PIN_DC)
#define LCD_5110_DATA_MODE()       PIN_HI(LCD_5110_PORT, LCD_5110_PIN_DC)
// set clock line
#define LCD_5110_SCLK_HI()    PIN_HI(LCD_5110_PORT,LCD_5110_PIN_SCLK)
#define LCD_5110_SCLK_LO()    PIN_LO(LCD_5110_PORT,LCD_5110_PIN_SCLK)
// set data line
#define LCD_5110_DATA_HI()    PIN_HI(LCD_5110_PORT,LCD_5110_PIN_DATA)
#define LCD_5110_DATA_LO()    PIN_LO(LCD_5110_PORT,LCD_5110_PIN_DATA)

// bias setting: b should be in range 0-4, with 4 being optimum according to datasheet
// Although, 3 might be better when running device from 5v

// write passed byte to the device.
// command or data mode should have been set up prior to calling this
static void lcd_5110_write(uint8_t data)
{
    // enable device's serial port
    LCD_5110_SCE_ASSERT();
    // shift cmd out - msb first
    for(uint8_t i=0; i!=8; ++i){
	// put bit on data line
	if( data & 0x80 )
	    LCD_5110_DATA_HI();
	else
	    LCD_5110_DATA_LO();
	// pulse clk
	// note: max clk speed is 4MHz, same as AVR max io for 16MHz F_CPU
	LCD_5110_SCLK_HI();
	LCD_5110_SCLK_LO();
	data=data << 1;
    }
    // disable device's serial port
    LCD_5110_SCE_DEASSERT();
}

void lcd_5110_write_cmd(uint8_t cmd)
{
    // DC low is for command
    LCD_5110_CMD_MODE();
    lcd_5110_write(cmd);
}

void lcd_5110_write_data(uint8_t data)
{
    // DC high is for data mode
    LCD_5110_DATA_MODE();
    lcd_5110_write(data);
}



void lcd_5110_init()
{
    // set pins as outputs
    LCD_5110_DDR |= _BV(LCD_5110_PIN_DC) |_BV(LCD_5110_PIN_SCE) |_BV(LCD_5110_PIN_DATA) | _BV(LCD_5110_PIN_RESET) | _BV(LCD_5110_PIN_SCLK);
    // apply *RST pulse to initialise device
    LCD_5110_RESET_LO();
    LCD_5110_RESET_HI();

    // send initialisation commands
    lcd_5110_write_cmd(LCD_5110_CMD_EXT);

    // contrast
    lcd_5110_write_cmd(LCD_5110_CONTRAST_SETTING);
    // temp coefficient
    lcd_5110_write_cmd(LCD_5110_TEMP_COEFF_SETTING);
    // bias - 4 is optimum according to datasheet
    // 0x03 is mux 1:48, 0x04 is mux 1:40
    lcd_5110_write_cmd(LCD_5110_BIAS_SETTING);

    // set basic command mode
    lcd_5110_write_cmd(LCD_5110_CMD_BASIC);

#if 1
    // all pixels on for a short time
    lcd_5110_write_cmd(LCD_5110_CMD_ALL_ON);
    _delay_ms(300);
#endif
    lcd_5110_write_cmd(LCD_5110_CMD_NORMAL);
    lcd_5110_clear_scr();
}

void lcd_5110_clear_scr()
{
    for(int i=0; i<84*6; ++i){
	lcd_5110_write_data(0x0);
    }
}


void lcd_5110_putc_reverse(char ch)
{
    uint8_t chindex=ch-0x20;

    for(uint8_t i=0; i<5; ++i){
	lcd_5110_write_data(~pgm_read_byte(&lcd_5110_font[chindex][i]));
    }
    // blank vertical line at rhs of character
    lcd_5110_write_data(0xff);

}

void lcd_5110_putc_double(uint8_t x, uint8_t y, char ch)
{
    uint8_t chindex=ch-0x20;

    for(uint8_t i=0; i<5; ++i){
	uint8_t b=pgm_read_byte(&lcd_5110_font[chindex][i]);

	uint16_t wb=0;
	for(uint8_t j=0; j<8; ++j){
	    if(b&0x80){
		wb|=0x1;
		wb=wb<<1;
		wb|=0x1;
		wb=wb<<1;
	    }else{
		wb&=~0x1;
		wb=wb<<1;
		wb&=~0x1;
		wb=wb<<1;
	    }
	    b=b<<1;
	}
	// now wb msb should be displayed at cell y+1, lsb at cell y
	uint8_t x1=x+i*2;
	lcd_5110_goto_xy_pix(x1,y+1);
	lcd_5110_write_data((wb & 0xff00) >> 8);
	lcd_5110_write_data((wb & 0xff00) >> 8);
	lcd_5110_goto_xy_pix(x1,y);
	lcd_5110_write_data(wb & 0x00ff );
	lcd_5110_write_data(wb & 0x00ff );
    }
}

void lcd_5110_putc(char ch)
{
    uint8_t chindex=ch-0x20;

    for(uint8_t i=0; i<5; ++i){
	lcd_5110_write_data(pgm_read_byte(&lcd_5110_font[chindex][i]));
    }
    // blank vertical line at rhs of character
    lcd_5110_write_data(0x0);
}


void lcd_5110_puts(char *s)
{
    while(*s)
	lcd_5110_putc(*(s++));
}

void lcd_5110_puts_double(uint8_t x, uint8_t y,char *s)
{
    while(*s){
	lcd_5110_putc_double(x,y,*(s++));
	x+=12;
    }
}


void lcd_5110_goto_xy_pix(uint8_t x, uint8_t y)
{
    lcd_5110_write_cmd(0x80|x);
    lcd_5110_write_cmd(0x40|y);
}

void lcd_5110_goto_xy(uint8_t x, uint8_t y)
{
    lcd_5110_write_cmd(0x80|(x*6));
    lcd_5110_write_cmd(0x40|y);
}
