// ----------------------------------------------------------------
// Copyright Stephen Stebbing 2014 - 2023
// ----------------------------------------------------------------
#include "lcd_i2c.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
//#include "../i2c/i2c_master.h"
#include "../i2c/pcf8574.h"

// pcf8574 bit positions of the lcd control pins 
// bits 4 to 7 are the data bits
#define LCD_I2C_RS 0
#define LCD_I2C_RW 1
#define LCD_I2C_E  2
#define LCD_I2C_BACKLIGHT 3


// -----------------------------------------------------
// global for lcd_i2c structure
lcd_i2c_t lcd;


// convenience macros
// enable
#define LCD_I2C_E_HI()  lcd.output |= (1<<LCD_I2C_E)
#define LCD_I2C_E_LO()  lcd.output &=~ (1<<LCD_I2C_E)
// set data or instruction mode
#define LCD_I2C_RS_D() lcd.output |= (1<<LCD_I2C_RS)
#define LCD_I2C_RS_I() lcd.output &=~ (1<<LCD_I2C_RS)
// set the data nibble -
// note: will need to change this for devices with different wiring. XXX why?
#define LCD_I2C_DATA_NIBBLE(x) lcd.output= (x<<4 | (lcd.output & 0x0f))
//! set the lcd data ram address to that passed, only lower 7 bits are used.
#define LCD_I2C_SET_DD_RAM_ADDRESS(address) lcd_i2c_write_i(0x80 | ((address) & 0x7f) )



// -----------------------------------------------------
// static functions
// -----------------------------------------------------
//! do the i2c transaction on the bus
static void _lcd_i2c_write()
{
    pcf8574_write(lcd.address, lcd.output);
}

static void lcd_i2c_e_assert()
{
    LCD_I2C_E_LO();
    _lcd_i2c_write();
    LCD_I2C_E_HI();
    _lcd_i2c_write();
    LCD_I2C_E_LO();
    _lcd_i2c_write();
}

//! Write passed data to device
//! Note: RS must be set prior to calling
static void lcd_i2c_write(uint8_t data)
{
   // high nibble
   LCD_I2C_DATA_NIBBLE((data >> 4));
   lcd_i2c_e_assert();
   // low nibble
   LCD_I2C_DATA_NIBBLE((data & 0x0f));
   lcd_i2c_e_assert();
   // delay for command to be executed
   _delay_us(50);
}


// -----------------------------------------------------
// user callable functions
// -----------------------------------------------------
void lcd_i2c_gotoxy(uint8_t x, uint8_t y)
{
    // note: on two line devices, second line begins at address 0x40
    // XXX so this will work with 1 and 2 line devices, but probabaly not with 4.
    LCD_I2C_SET_DD_RAM_ADDRESS(y*0x40 + x  );
    lcd.x=x;
    lcd.y=y;
}

#if 0
void lcd_i2c_printf char* format, ...)
{
    va_list args;
    char *spp=NULL;

    va_start(args, format);
    int r=vasprintf(&spp, format, args);
    if(r!=-1){
	lcd_i2c_puts(spp);
	free(spp);
    }
    va_end(args);
}
#endif

void lcd_i2c_write_i(uint8_t data)
{
    LCD_I2C_RS_I();
    lcd_i2c_write(data);
}

void lcd_i2c_write_d(uint8_t data)
{
    LCD_I2C_RS_D();
    lcd_i2c_write(data);
}

void lcd_i2c_clear()
{
    lcd_i2c_write_i(0x1);
    _delay_ms(2); // delay for command to take effect
}

void lcd_i2c_home()
{
    lcd_i2c_write_i(0x02);
    lcd.x=0;
    lcd.y=0;
    _delay_ms(2); // delay for command to take effect
}

void lcd_i2c_putc(char c)
{
    lcd_i2c_write_d(c);
//    printf("c: %c (%i,%i)\n",c, lcd.x, lcd.y);
    if(++lcd.x == lcd.cols){
	lcd.x=0;
	if(++lcd.y == lcd.rows){
	    lcd.y=0;
	}
	lcd_i2c_gotoxy(lcd.x,lcd.y);
    }
}

void lcd_i2c_puts(const char* str)
{
    while(*str!='\x0'){
	lcd_i2c_putc(*str++);
    }
}

void lcd_i2c_init(uint8_t address, uint8_t rows, uint8_t cols)
{
    
    lcd.address=address;
    lcd.output=0;
    lcd.rows=rows;
    lcd.cols=cols;
    
    LCD_I2C_E_LO();
    LCD_I2C_RS_I();
    _lcd_i2c_write();

    // software reset
    LCD_I2C_DATA_NIBBLE(0x3);
    lcd_i2c_e_assert();
    _delay_ms(5); // ms
    lcd_i2c_e_assert();
    _delay_us(150);
    lcd_i2c_e_assert();
    _delay_us(150);
    
    // set 4 bit mode
    LCD_I2C_DATA_NIBBLE(0x02);
    lcd_i2c_e_assert();

    // entry mode - 0x06 is display shift on, increment address counter
    lcd_i2c_write_i(0x06);

    // set cursor
    LCD_I2C_CURSOR_BLINK_OFF();
    LCD_I2C_CURSOR_OFF();

    // clear and home 
    lcd_i2c_clear();
    lcd_i2c_home();
       
}

/** 
 * Turn backlight on or off
 * @param ct lcd_i2c_t control structure for the device
 * @param on Turn on if non-zero, off otherwise
 */
void lcd_i2c_backlight(uint8_t on)
{
    if(on)
	lcd.output |= (1<<LCD_I2C_BACKLIGHT);
    else
	lcd.output &=~ (1<<LCD_I2C_BACKLIGHT);
    _lcd_i2c_write();
}
