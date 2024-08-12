// -----------------------------------------------------------------------------
// Copyright 2018 Stephen Stebbing. telecnatron.com
//
//    Licensed under the Telecnatron License, Version 1.0 (the “License”);
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        https://telecnatron.com/software/licenses/
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an “AS IS” BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// -----------------------------------------------------------------------------   
#include "sysclk.h"
#include <avr/interrupt.h>

// tick count
uint16_t sysclk_ticks;
// seconds, this can be remotely set to eg unix time
uint32_t sysclk_seconds;
// seconds since boot
uint32_t sysclk_seconds_count;
// flag to indicated that clk has ticked since last call to sysclk_has_ticked()
uint8_t sysclk_ticked;
// flag to indicated that seconds haveticked since last call to sysclk_have_seconds_ticked()
uint8_t sysclk_seconds_ticked;
//!
uint16_t sysclk_tick_freq = SYSCLK_TICK_FREQ;


void sysclk_init()
{
    SYSCLK_INIT();
    SYSCLK_INIT_PRE();
    SYSCLK_SET_TC();
    sysclk_start();
}

inline void sysclk_stop()
{
    // disable clk source
    SYSCLK_STOP();
    // disable interrupt
    SYSCLK_INT_DISABLE();
}

inline void sysclk_start()
{
    // enable clk source
    SYSCLK_START();
    // enable interrupt
    SYSCLK_INT_ENABLE();
}

void sysclk_reset()
{
    sysclk_ticks=0;
    sysclk_seconds=0;
    sysclk_ticked=0;
    sysclk_seconds_ticked = 0;
}


inline uint16_t sysclk_get_tick_freq()
{
    return sysclk_tick_freq;
}

inline void sysclk_set_tick_freq(uint16_t freq)
{
    SYSCLK_INT_DISABLE();
    sysclk_tick_freq=freq;
    SYSCLK_INT_ENABLE();
}


inline uint16_t sysclk_get_ticks()
{
    return sysclk_ticks;
}

inline uint8_t sysclk_has_ticked()
{
    if( sysclk_ticked ){
	sysclk_ticked=0;
	return 1;
    }
    return 0;
}

inline uint8_t sysclk_have_seconds_ticked()
{
    if( sysclk_seconds_ticked){
	sysclk_seconds_ticked= 0;
	return 1;
    }
    return 0;
}

inline uint32_t sysclk_get_seconds()
{
    return sysclk_seconds;
}

inline uint32_t sysclk_get_seconds_count()
{
    return sysclk_seconds_count;
}

inline void sysclk_set_seconds(uint32_t seconds)
{
    SYSCLK_INT_DISABLE();
    sysclk_seconds=seconds;
    SYSCLK_INT_ENABLE();
}

inline void sysclk_reset_seconds()
{
    sysclk_seconds=0;
}


ISR(SYSCLK_ISR_NAME)
{
    sysclk_ticked=1;
    sysclk_ticks++;
    if( sysclk_ticks >=  sysclk_tick_freq){
	sysclk_seconds++;
	sysclk_seconds_count++;
	sysclk_seconds_ticked = 1;
	sysclk_ticks=0;
    }
}


