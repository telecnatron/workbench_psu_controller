# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2024. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
from datetime import datetime
from struct import unpack, pack ;

from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus

class DS1302_rtc(Handler):

    # subcommands
    SC_READ   = 0
    SC_SET    = 1
    
    # -------------------------------------------
    def read(self):
        """ read the MCU DS1302 RTC and returns it's data as a dict.
        The returned data is a struct of type rtc_datetime_t as defined in lib/rtc.h
        """
        fields=('second', 'minute', 'hour', 'DOM', 'month', 'DOW', 'year')
        rmsg=self.sub_command(DS1302_rtc.SC_READ)
        return self.rmsg_to_dict("<BBBBBBB", fields, rmsg)

    # -------------------------------------------
    def set(self):
        """set the MCU clock to the current Hours Minutes, Seconds."""
        n=datetime.now()
        dt= [ n.second, n.minute, n.hour, n.day, n.month, 0, n.year-2000]
        rmsg=self.sub_command(DS1302_rtc.SC_SET, pack("<BBBBBBB",*dt ))
        logging.info(f"RTC clock has been set  to {n}")
        return rmsg.status

    
