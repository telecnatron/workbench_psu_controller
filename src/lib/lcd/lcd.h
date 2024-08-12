#ifndef LCD_H
#define LCD_H
// -----------------------------------------------------------------------------
// Copyright Stebbing Computing. 2013.
// $Id: lcd.h 110 2014-08-30 07:15:56Z steves $
// -----------------------------------------------------------------------------
/**
 * @file   lcd.h
 * @author Stephen Stebbing <steves@slap>
 * @date   Mon Jul 29 22:56:26 2013
 * 
 * @brief   HD44780 2x16 lcd control functions
 * see http://www.geocities.com/dinceraydin/lcd/commands.htm for detail of lcd controls
 * 
 */
#include <avr/io.h>
#include "config.h"

//! Defines for ddrs, ports and pins. These can be defined in another file which is included
//! prior to here - ensure that the file defines LCD_DEFS, otherwise these defaults will be used.
#ifndef LCD_DEFS
#warning "lcd.h is using default HW definitions"
// lcd data 0..3 should be connected to pins 0..3 of the port specified in LCD_DATA_PORT
#define LCD_DATA_PORT PORTB
#define LCD_DATA_DDR  DDRB
// port and pin of the lcd's rs line
#define LCD_RS_PORT  PORTD
#define LCD_RS_DDR  DDRD
#define LCD_RS_PIN PIN6
// port of the enable line
#define LCD_E_PORT PORTB
#define LCD_E_DDR  DDRB
#define LCD_E_PIN PIN4
#endif

// Display, cursor and blink on/off control.
#define LCD_DISPLAY_CONTROL(display, cursor, blink) lcd_write_i ( 0x08 | (display << 2) | (cursor << 1) | (blink) )
#define LCD_CURSOR_ON() LCD_DISPLAY_CONTROL(1,1,0)
#define LCD_CURSOR_OFF() LCD_DISPLAY_CONTROL(1,0,0)
#define LCD_CURSOR_BLINK_ON() LCD_DISPLAY_CONTROL(1,1,1)
#define LCD_CURSOR_BLINK_OFF() LCD_DISPLAY_CONTROL(1,1,0)

//! Initalise the lcd display
void lcd_init();

//! Clear the lcd - delays for 2ms
void lcd_clear();
//! Write passed byte to lcd as data - delays for 50us
void lcd_write_d(unsigned char data);
//! Write passed byte to lcd as instruction - delays for 50us
void lcd_write_i(unsigned char data);

//! Display passed character at current position
void lcd_putc(char c);

//! Display a string at current position
void lcd_puts(const char* str);

//! Display a progmem string at current postion
void lcd_puts_P(const char* pm_str);

//! Moved cursor to passed row and column
void lcd_gotoxy(unsigned char x, unsigned char y);
#endif
