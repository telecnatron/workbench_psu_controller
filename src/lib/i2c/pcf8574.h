#ifndef _PCF8574_H
#define _PCF8574_H 1
// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------

// function declerations
// write the byte to device at addr
void pcf8574_write(uint8_t addr, uint8_t byte);

// read the byte from device at addr
uint8_t pcf8574_read(uint8_t addr);

#endif /* _PCF8574_H */

