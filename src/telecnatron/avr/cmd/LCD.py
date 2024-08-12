# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
from struct import unpack, pack ;
from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus

# -----------------------------------
class LCD(Handler):
    
    # subcommands
    SC_BACKLIGHT = 0
    SC_PUTS      = 1
    SC_GOTOXY    = 2
    SC_CLEAR     = 3

    # -------------------------------
    def backlight(self, on=True):
        byte=0
        b="off"
        if on:
            byte=1
            b="on"
        rmsg=self.sub_command(LCD.SC_BACKLIGHT, pack('<B', byte))
        logging.info(f"turned lcd backlight {b}.")
        return rmsg.status

    # -------------------------------
    def puts(self, string):
        b=bytes(string, 'utf-8')
        f=f"<{len(b)}sB"
        rmsg=self.sub_command(LCD.SC_PUTS, pack(f,b,0))
        return rmsg.status


    # -------------------------------
    def gotoxy(self, x,y):
        rmsg=self.sub_command(LCD.SC_GOTOXY, pack('<BB',x,y))
        return rmsg.status

    # -------------------------------
    def clear(self):
        rmsg=self.sub_command(LCD.SC_CLEAR)
        return rmsg.status
    
