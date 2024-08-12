# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2017. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
import time
from datetime import datetime
from struct import pack ;

from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler

class Clock(Handler):
    
    # subcommands
    SC_READ          = 0
    SC_SET_CLOCK     = 1
    SC_SET_SYSCLK    = 2
    SC_SET_TICK_FREQ = 3

    # -------------------------------------------
    def read(self):
        """ read the MCU clock and return it's data as a dict: eg {'H': 1, 'M': 3, 'S': 12, 'ticks': 870, 'seconds': 3791, 'seconds_count': 3791, 'tick_freq': 1000}"""
        fields=('S', 'M', 'H', 'ticks', 'seconds', 'seconds_count', 'tick_freq')
        rmsg=self.sub_command(Clock.SC_READ)
        return self.rmsg_to_dict("<BBBHLLH", fields, rmsg)
    
    # -------------------------------------------
    def set_tick_freq(self, freq):
        """set the MCU sysclk_tick_freq, this can be used correct errors in the clock."""
        rmsg = self.sub_command(Clock.SC_SET_TICK_FREQ, pack('<H', int(freq)))
        logging.info(f"set tick frequency to {freq}: {rmsg}")
        return rmsg.status
    
    # -------------------------------------------
    def set_clock(self):
        """set the MCU clock to the current Hours Minutes, Seconds."""
        n=datetime.now()
        rmsg=self.sub_command(Clock.SC_SET_CLOCK, pack("<BBB", n.second, n.minute, n.hour))
        logging.info(f"set clock to {n}")
        return rmsg.status
    
    # -------------------------------------------
    def set_sysclk(self):
        """ """
        ut=int(time.time())
        rmsg=self.sub_command(Clock.SC_SET_SYSCLK, pack("<L", ut))
        logging.info(f"set sysclk to {ut}")
        return rmsg.status
        
