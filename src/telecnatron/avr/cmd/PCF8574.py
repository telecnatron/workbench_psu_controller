# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
from struct import unpack, pack ;

from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus

# -----------------------------------
class PCF8574(Handler):
    
    # subcommands
    SC_WRITE = 0
    SC_READ  = 1
    # -------------------------------
    def write(self, byte):
        rmsg=self.sub_command(PCF8574.SC_WRITE, pack('<B', byte))
        logging.info(f"wrote byte to pcf8574: {byte}")
        return rmsg.status

    # -------------------------------
    def read(self):
        rmsg=self.sub_command(PCF8574.SC_READ)
        return pack('<B', rmsg.data[0])


