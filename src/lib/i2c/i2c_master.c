// ----------------------------------------------------------------------
// Modified by Stephen Stebbing 2023
// ----------------------------------------------------------------------
#include <inttypes.h>
#include <compat/twi.h>

#include "i2c_master.h"
#include "./lib/log.h"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#error "F_CPU is not defined"
#endif

/* I2C clock in Hz */
#ifndef I2C_SCL_CLOCK
#define I2C_SCL_CLOCK  100000L
#endif 


// ----------------------------------------------------------
void i2c_init(void)
{
    /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
    // set status register
    TWSR = 0;                         /* no prescaler */
    // set bitrate register
    TWBR = ((F_CPU/I2C_SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
}/* i2c_init */


/*************************************************************************	
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char i2c_start(unsigned char address)
{
    // start condition is signalled by changing level on sda while scl is high.
    uint8_t   twst;

    // send START condition
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // wait until transmission completed
    while(! (TWCR & _BV(TWINT)) );

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;

    // send device address
    TWDR = address;
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait until transmission completed and ACK/NACK has been received
    while(!(TWCR & (1<<TWINT)));

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
    
    return 0;

}


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready
 
 Input:   address and transfer direction of I2C device
*************************************************************************/
void i2c_start_wait(unsigned char address)
{
    uint8_t   twst;


    while ( 1 )
    {
	    // send START condition
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    
    	// wait until transmission completed
    	while(!(TWCR & (1<<TWINT)));
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
    
    	// send device address
    	TWDR = address;
    	TWCR = (1<<TWINT) | (1<<TWEN);
    
    	// wail until transmission completed
    	while(!(TWCR & (1<<TWINT)));
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) ) 
    	{    	    
    	    /* device busy, send stop condition to terminate write operation */
	        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	        
	        // wait until stop condition is executed and bus released
	        while(TWCR & (1<<TWSTO));
	        
    	    continue;
    	}
    	//if( twst != TW_MT_SLA_ACK) return 1;
    	break;
     }

}/* i2c_start_wait */


/*************************************************************************
 Issues a repeated start condition and sends address and transfer direction 

 Input:   address and transfer direction of I2C device
 
 Return:  0 device accessible
          1 failed to access device
*************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );

}/* i2c_rep_start */


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2c_stop(void)
{
    /* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// wait until stop condition is executed and bus released
	while(TWCR & (1<<TWSTO));

}/* i2c_stop */


/*************************************************************************
  Send one byte to I2C device
  
  Input:    byte to be transfered
  Return:   0 write successful 
            1 write failed
*************************************************************************/
unsigned char i2c_write( unsigned char data )
{	
    uint8_t   twst;
    
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits
	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return 1;
	return 0;

}/* i2c_write */


/*************************************************************************
 Read one byte from the I2C device, request more data from device 
 
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2c_readAck(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));    

    return TWDR;

}/* i2c_readAck */


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition 
 
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2c_readNak(void)
{
    TWCR = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
    return TWDR;

}/* i2c_readNak */


void i2c_enumerate()
{
    // enumerate i2c looking for devices.
    // note that address 0 is a 'broadcast' address, any/all devices can respond to it.
    for(uint8_t i=1; i<=126; i++){
	uint8_t r=i2c_start(i << 1 | I2C_WRITE);
	i2c_stop();
	if(!r){
	    // device is present at this address
	    LOG_INFO_FP("found i2c device at address 0x%x",i);
	}
    }
   
}
