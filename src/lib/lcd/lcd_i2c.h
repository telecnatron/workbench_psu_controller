#ifndef LCD_I2C_H
#define LCD_I2C_H
// ----------------------------------------------------------------
// Copyright Stephen Stebbing 2014 -2023.
// ----------------------------------------------------------------
#include <stdint.h>

//! Structure for containing lcd state data
typedef struct {
    // i2c address of pcf8574 device that is controlling the LCD
    uint8_t address;
    // current pcf8574  output
    uint8_t output;
    // number of rows
    uint8_t rows;
    // number of columns
    uint8_t cols;
    // row and column coord of current cursor position
    // for a 1602 display, bottom right is (15,1), home is (0,0) 
    uint8_t x,y;
} lcd_i2c_t;



// convenience macros
#define LCD_I2C_BACKLIGHT_ON() lcd_i2c_backlight(1)
#define LCD_I2C_BACKLIGHT_OFF() lcd_i2c_backlight(0)
// Display, cursor and blink on/off control.
#define LCD_I2C_DISPLAY_CONTROL(display, cursor, blink) lcd_i2c_write_i ( 0x08 | (display << 2) | (cursor << 1) | (blink) )
#define LCD_I2C_CURSOR_ON() LCD_I2C_DISPLAY_CONTROL(1,1,0)
#define LCD_I2C_CURSOR_OFF() LCD_I2C_DISPLAY_CONTROL(1,0,0)
#define LCD_I2C_CURSOR_BLINK_ON() LCD_I2C_DISPLAY_CONTROL(1,1,1)
#define LCD_I2C_CURSOR_BLINK_OFF() LCD_I2C_DISPLAY_CONTROL( 1,1,0)


// default i2c address
#define LCD_I2C_PCF8574_ADDRESS_DEFAULT 0x27


/** 
 * Initalise the lcd by doing a software reset, clearing and positioning cursor in home position
 * @param address i2c address of the pcf8574 that controls the LCD. Use PC LCD_I2C_PCF8574_ADDRESS_DEFAULT for default
 * @param rows number of rows on the lcd
 * @param cols number of columns on the lcd
 */
void lcd_i2c_init(uint8_t address, uint8_t rows, uint8_t cols);

/** 
 * Write an instruction byte to the lcd
 * @param data The instruction byte
 */
void lcd_i2c_write_i(uint8_t data);

/** 
 * Write a data byte to the lcd
 * @param data The data byte
 */
void lcd_i2c_write_d(uint8_t data);

/** 
 * Move the cursor location to specified coords.
 * Note that no range checking is done
 * @param x The x coord
 * @param y The y coord
 */
void lcd_i2c_gotoxy(uint8_t x, uint8_t y);

/** 
 * Write to lcd with printf like functionallity
 * @param format_p The format string, followed by the parameters
 */
void lcd_i2c_printf(char* format_p, ...);

/** 
 * Clear the lcd
 */
void lcd_i2c_clear();

/** 
 * Move curson to home position
 */
void lcd_i2c_home();

/** 
 * Display passed character at current position
 * @param c The character to be displayed
 */
void lcd_i2c_putc(char c);

/** 
 * Display passed string at current position
 * @param str Pointer to the string to be displayed
 */
void lcd_i2c_puts(const char* str);

/** 
 * Turn backlight on or off
 * @param on Turn on if non-zero, off otherwise
 */
void lcd_i2c_backlight(uint8_t on);



#endif
