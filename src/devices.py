# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
from struct import pack,unpack
from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus
# -----------------------------------
class Fan(Handler):

    def control(self, on=True):
        """ turn fan on or off according to 'on' parameter """
        if on:
            on = b'\x01'
        else:
            on = b'\x00'
        rmsg=self.command(on)
        return rmsg.status


# -----------------------------------
class Load(Handler):
    """ read setting of load switch """
    def read(self):
        rmsg=self.command()
        adcval=unpack('<B' ,rmsg.data)
        print(f"load switch: 0x{adcval[0]:02x}")
        return adcval[0]


# -----------------------------------
class Shutdown(Handler):
    """ control PSU shutdown function """

    # subcommands
    SC_RESTART  = 0
    SC_SHUTDOWN = 1
    SC_READ     = 2
    
    def shutdown(self):
        rmsg=self.sub_command(self.SC_SHUTDOWN)

    def restart(self):
        # subcommand 0
        rmsg=self.sub_command(self.SC_RESTART)

    def read(self):
        # subcommand 2
        rmsg=self.sub_command(self.SC_READ)
        s=unpack('<B',rmsg.data)
        s=s[0]
        logging.info(f"shutdown status: {s}")
        return(s)
    
# -----------------------------------
class Measurements(Handler):
    """ obtain voltage, current, power, energy measurements from the MCU """

    # subcommands
    SC_READ   = 0
    SC_RESET  = 1
    
    def read(self):
        """ Get the current measurement values fromt he MCU, returns a dict like: {'volts': 10.7900390625, 'amps': 0.11220702528953552, 'watts': 1.2107181549072266, 'joules': 306.39349365234375}"""
        rmsg=self.sub_command(self.SC_READ)
        fields=('volts', 'amps', 'watts', 'joules')
        m=self.rmsg_to_dict("<ffff",fields, rmsg)
        #logging.info(f"m: {m}: ")
        return m


    def reset(self):
        """ reset the MCU's number of joules counter """
        rmsg=self.sub_command(self.SC_RESET)
        
