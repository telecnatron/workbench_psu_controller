// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#ifndef _LCD_H
#define _LCD_H 1

#include <avr/pgmspace.h>

//!
void lcd_buf_to_screen();

/**
 * printf-style to the lcd at the current position.
 * format is PSTR(), ie held in program memory.
 */
void lcd_printf_P(const char* format_p, ...);
#define LCD_PRINTF_P(fmt, msg...) lcd_printf_P(PSTR(fmt),msg)

//!
void lcd_buf_clear();


#endif /* _LCD_H */

