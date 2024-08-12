// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#include <avr/interrupt.h>
#include "./lib/log.h"
#include "adc.h"

void adc_init_avcc(uint8_t channel)
{
    ADC_REFERENCE_AVCC();
    //enable ADC module, set prescalar of 128 which gives CLK/128
    ADC_ENABLE();
    ADC_PRESCALER_128();
    // disable digital input on the pin
    ADC_DIN_DISABLE(channel);
}


// read adc by polling
uint16_t adc_read_poll(uint8_t channel)
{
    ADC_CHANNEL_SELECT(channel);
    ADC_START_CONVERSION();
    ADC_WAIT_POLL();
    return ADCW;                      
}


// example handler for ADC interrupt
#if 0
ISR(ADC_vect)
{
    LOG_INFO_FP("isr: %u", ADCW);
    adc_value=ADCW;
}
#endif
