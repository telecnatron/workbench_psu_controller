#ifndef _FLAGS_H
#define _FLAGS_H 1
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
/**
 * @file   flags.h
 * @author steves
 * @date   2017/02/21 23:15:02
 * 
 * @brief  Definitions and descriptions of values for messages' FLAGS byte.
 * 
 * 
 */

#include "config.h"

// * non-bootloader messages have bit 0 set to 0.
// * boot loader messages have bit 0 set, all other bits unset, ie flags=0x1
#define MMP_FLAGS_BOOT        0x0
// bit indicates whether message is a CMD msg (1) or ASYNC msg (0)
#define MMP_FLAGS_BIT_TYPE    0x1


// convenience macros
// testing flag bits
#define MMP_FLAGS_IS_BOOT(flags)    ( flags == 0x1 )
#define MMP_FLAGS_IS_CMD(flags)     ( flags & _BV(MMP_FLAGS_BIT_TYPE) )
#define MMP_FLAGS_IS_ASYNC(flags)   ( ! MMP_FLAGS_IS_TYPE(flags) )

// setting flag bits
#define MMP_FLAGS_SET_BOOT(flags)  ( flags = 0x1)
#define MMP_FLAGS_SET_CMD(flags)   ( flags |=  _BV(MMP_FLAGS_BIT_TYPE))
#define MMP_FLAGS_SET_ASYNC(flags) ( flags &= ~_BV(MMP_FLAGS_BIT_TYPE))

#endif /* _FLAGS_H */

