// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
#ifndef _CLOCK_H
#define _CLOCK_H 1
#include "../rtc.h"

// struct for tracing h:m:s
typedef rtc_time_t clock_t;

// global clock structure
extern  clock_t clock;

// function declarations
// clock_tick should be called once per second to make the clock keep time.
void clock_tick();

#endif /* _CLOCK_H */

