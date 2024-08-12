// -----------------------------------------------------------------------------
// Copyright Stephen Stebbing 2023. http://telecnatron.com/
// -----------------------------------------------------------------------------
// A clock that counts in hours, minutes and seconds.
// Call clock_tick() once per second for it to keep time.
#include "config.h"
#include "clock.h"

// global clock structure
clock_t clock;


void clock_tick()
{
    // count up in hours, minutes and seconds
    clock.second++;
    if (clock.second==60){
	clock.second=0;
	clock.minute++;
	if(clock.minute==60){
	    clock.minute=0;
	    clock.hour++;
	    if(clock.hour==24){
		clock.hour=0;
	    }
	}
    }
}


