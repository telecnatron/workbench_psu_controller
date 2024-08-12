// -----------------------------------------------------------------------------
// Copyright Stebbing Computing. 2013.
// $Id: $
// -----------------------------------------------------------------------------
//! Routines for interfacing to HD44780 LCD module in 4 bit mode.

#include "lcd.h"
#include <util/delay.h>
#include <avr/pgmspace.h>


#define LCD_E_HIGH()   LCD_E_PORT |= _BV(LCD_E_PIN)
#define LCD_E_LOW()    LCD_E_PORT &=~_BV(LCD_E_PIN)
//#define LCD_E_ASSERT() LCD_E_HIGH(); _delay_us(100);/* delay needed? */LCD_E_LOW()
#define LCD_E_ASSERT() LCD_E_HIGH(); LCD_E_LOW()
// RS is high to select data register
#define LCD_RS_D()     LCD_RS_PORT |= _BV(LCD_RS_PIN)
#define LCD_RS_I()     LCD_RS_PORT &=~_BV(LCD_RS_PIN)

// bit patterns for the commands:
// generic command, for clear, home, entry mode set, display on/off, 
#define LCD_CMD  0x00  
#define LCD_CMD_SHIFT 0x10
#define LCD_CMD_FUNCTION_SET 0x20

// Can use these to pass to lcd_write_i()
#define LCD_CLEAR LCD_CMD | 0x01
#define LCD_HOME  LCD_CMD | 0x02

// set the lcd data ram address to that passed, only lower 7 bits are used.
#define LCD_SET_DD_RAM_ADDRESS(address) lcd_write_i( 0x80 | ((address) & 0x7f) )

void lcd_clear()
{
    lcd_write_i(LCD_CLEAR);
    _delay_ms(2); // delay for command to take effect
}

void lcd_home()
{
    lcd_write_i(LCD_HOME);
    _delay_ms(2); // delay for command to take effect
}

void lcd_write_i(unsigned char data)
{
    // rs to instruction register
    LCD_RS_I();
    // high nibble
    unsigned portBits = LCD_DATA_PORT & 0xf0; // get high nibble from port
    LCD_DATA_PORT = portBits | (( data >> 4) &  0x0f);
    LCD_E_ASSERT();
    // low nibble
    LCD_DATA_PORT = portBits | ( data &  0x0f);
    LCD_E_ASSERT();
    // delay 50us for command to take effect
    _delay_us(50);
}

void lcd_write_d(unsigned char data)
{
    // rs to data register
    LCD_RS_D();
    // high nibble
    unsigned portBits = LCD_DATA_PORT & 0xf0; // get high nibble from port
    LCD_DATA_PORT = portBits | (( data >> 4) &  0x0f);
    LCD_E_ASSERT();
    // low nibble
    LCD_DATA_PORT = portBits | ( data &  0x0f);
    LCD_E_ASSERT();
    // delay 50us for command to take effect
    _delay_us(50);
}

void lcd_putc(char c)
{
    lcd_write_d(c);
}

void lcd_puts(const char* str)
{
    while(*str!='\x0'){
	lcd_write_d(*str++);
    }
}

void lcd_puts_P(const char* str)
{
    char c = pgm_read_byte(str++);
    while ( c != 0x0){
	lcd_write_d(c);
	c=pgm_read_byte(str++);
    }
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
    LCD_SET_DD_RAM_ADDRESS(   (x-1) + ((y-1)*64)  );
}

void lcd_init()
{
    // data pins as outputs
    LCD_DATA_DDR |= 0x0F;
    // RS pin as output
    LCD_RS_DDR |= ( _BV(LCD_RS_PIN));
    // E pin as output
    LCD_E_DDR |= ( _BV(LCD_E_PIN));

    // init the lcd
    // wait 16 ms after power on
    _delay_ms(16);


    unsigned portBits = LCD_DATA_PORT & 0xf0; // get high nibble from port

    // software reset
    LCD_E_LOW();
    LCD_RS_I();
    LCD_DATA_PORT  = portBits | 0x03; 
    LCD_E_ASSERT();
    _delay_ms(5);
    LCD_E_ASSERT();
    _delay_us(150);
    LCD_E_ASSERT();
    _delay_us(150);

    // set 4 bit mode
    LCD_E_LOW();
    LCD_RS_I();
    LCD_DATA_PORT =  portBits | 0x02;
    LCD_E_ASSERT();

    // configure geometry
    lcd_write_i(0b00101000); // 5x8 char, 2 line

    // display on, config cursor, blink
    LCD_DISPLAY_CONTROL(1,1,1);

    // entry mode - 0x06 is display shift on, increment address coutner
    lcd_write_i(0x06);

    lcd_clear();

    return;
}
