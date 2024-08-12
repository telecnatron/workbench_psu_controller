// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#ifndef _ADC_H
#define _ADC_H 1
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
#define ADC_CHANNEL_SELECT(cnum)   ADMUX &= ( 0b11110000 | (cnum & 0b00001111) )

// disable digital input on  pin that is to be used for analogue. See; table 23.9.5
// apin can be ADC0D to ADC5D
#define ADC_DIN_DISABLE(apin)  DIDR0 |= _BV(apin)
    
#endif /* _ADC_H */

