#ifndef LCD_5110_H
#define LCD_5110_H
// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing. 2014.
// $Id: lcd_5110.h 157 2014-09-30 17:05:16Z steves $
// Interface for AVR processors to Nokia 5110 LCD module
// -----------------------------------------------------------------------------
#include <avr/io.h>

// -----------------------------------------------------------------------------
#ifndef LCD_5110_DEFS
#define LCD_5110_DEFS
#warning "Using default defs"
// ports and pins
#define LCD_5110_DDR       DDRC
// Note: all pins must be on the same port
#define LCD_5110_PORT      PORTC
#define LCD_5110_PIN_SCE   PIN5
#define LCD_5110_PIN_RESET PIN4
#define LCD_5110_PIN_DC    PIN3
#define LCD_5110_PIN_DATA  PIN2
#define LCD_5110_PIN_SCLK  PIN1

// init values
//#define LCD_5110_CONTRAST_SETTING   0xb1
#define LCD_5110_CONTRAST_SETTING   0xaf

#define LCD_5110_TEMP_COEFF_SETTING 0x04
#define LCD_5110_BIAS_SETTING       0x14

// font include file
#define LCD_5110_FONT_PATH "lcd_5110_font.h"
#endif
// -----------------------------------------------------------------------------

// command bytes
#define LCD_5110_CMD_EXT 0x21
#define LCD_5110_CMD_BASIC 0x20
#define LCD_5110_CMD_ALL_ON 0x09
#define LCD_5110_CMD_BLANK  0x08
#define LCD_5110_CMD_NORMAL 0x0c

//! Write a command byte to the device
void lcd_5110_write_cmd(uint8_t cmd);

//! clear screen
void lcd_5110_clear_scr();

//! write passed ascii char at current location
void lcd_5110_putc(char ch);

//! write passed ascii character with reversed background
void lcd_5110_putc_reverse(char ch);

/** 
 *  write passed ascii character in double size (4x4 cells);
 * @param x  x co-ord of top cell 
 * @param y  y co-ord of left cell
 * @param ch the character to display
 */
void lcd_5110_putc_double(uint8_t x, uint8_t y, char ch);


/** 
 *  write passed string at current location in double sized font
 * @param x  x co-ord of top cell 
 * @param y  y co-ord of left cell
 * @param ch the character to display
 */
void lcd_5110_puts_double(uint8_t x, uint8_t y,char *s);


//! write passed string at current location
void lcd_5110_puts(char *s);


//! Set x and y address of cursor - 0<=x<=83, 0<=y<=5
//! Note: y is set on a character row (8 pixels), x on a pixel column
void lcd_5110_goto_xy_pix(uint8_t x, uint8_t y);

//! Set x and y address of cursor - 0<=x<=14, 0<=y<=5
//! Note: y is set on a character row (8 pixels), x on a character column (6 pixels)
void lcd_5110_goto_xy(uint8_t x, uint8_t y);

//! initialise the device
void lcd_5110_init();



#endif
