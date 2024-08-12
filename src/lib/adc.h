#ifndef _ADC_H
#define _ADC_H 1
// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
// ATmega328 datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
#include <avr/io.h>

// control whether adc interrupt is triggered
#define ADC_INTERRUPT_ENABLE()     ADCSRA |= _BV(ADIE)
#define ADC_INTERRUPT_DISABLE()    ADCSRA &=~ _BV(ADIE)

// adc control
#define ADC_ENABLE()               ADCSRA |= _BV(ADEN)
#define ADC_DISABLE()              ADCSRA &=~ _BV(ADEN)

// start convertion
#define ADC_START_CONVERSION()     ADCSRA |= _BV(ADSC)

// reference selection, see datasheet table 23.9.1:
// use external voltage on AREF pin
#define ADC_REFERENCE_AREF()   ADMUX &=~ ((_BV(REFS1) | _BV(REFS0))
// use AVcc
#define ADC_REFERENCE_AVCC()   ADMUX &=~ _BV(REFS1); ADMUX |= _BV(REFS0)
// use internal bandgap reference
#define ADC_REFERENCE_BG()     ADMUX |= (_BV(REFS1) | _BV(REFS0))

// channel selection, table 23.4
#define ADC_CHANNEL_SELECT(cnum)   ADMUX &=  0b11110000; ADMUX |= (cnum & 0b00001111)

// clock prescaler:  table 23.5
// "requires an input clock frequency between 50kHz and 200kHz to get maximum resolution."
// We run at 16MHz, divide by 128 gives 125000kHz. Convertion takes 13 cycles, therefore sampling rate is  9615Hz, 104us.
#define ADC_PRESCALER_128()        ADCSRA |= ( _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2)   )    


// wait for conversion to complete by looping unti ADSC bit goes to 0
#define	ADC_WAIT_POLL()  while (  ADCSRA & _BV(ADSC))


// trigger source is configured via register ADCSRB, default is free running    
    
// disable digital input on  pin that is to be used for analogue. See; table 23.9.5
// apin can be ADC0D to ADC5D
#define ADC_DIN_DISABLE(apin)  DIDR0 |= _BV(apin)

   
// -----------------------------------------------------------------------------    
// function declarations
// -----------------------------------------------------------------------------    
// init using AVcc as reference and 128 clock prescale divisor    
void adc_init_avcc(uint8_t channel);

// read adc by polling
uint16_t adc_read_poll(uint8_t channel);


    
#endif /* _ADC_H */

