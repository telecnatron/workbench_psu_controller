# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
import time
from struct import unpack, pack ;

from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus
# -----------------------------------
class Ping(Handler):
    
    # ---------------------------    
    def _init(self):
        self.successes=0
        self.errors=0
        self.timeouts=0
        
    # ---------------------------
    def ping(self, count=1, delay=0):
        self._init()
        for i in reversed(range(count)):
            try:
                rmsg=self._ping(i)
                # MCU cmd_ping() should return message with the byte that we sent (ie the id param to _ping(), here it is i) in d[0], and i+1 in d[1]
                a=rmsg.data[0]
                b=rmsg.data[1]
                if b == a+1:
                    # yes
                    self.successes+=1
                else:
                    self.errors+=1
                    logging.error("MCU ping did not increment the id number that I sent. It should have.")
            except ENoResponse:
                # MCU did not send response message
                self.timeouts+=1
            except EStatus:
                # MCU returned non-zero stat
                self.errors+=1
                
            time.sleep(delay)
            
        # make up a dict of the stats and return that
        d=dict.fromkeys(('successes', 'errors', 'timeouts'))
        d.update(zip(d, (self.successes, self.errors, self.timeouts)))
        return d
        
    # ---------------------------
    def _ping(self, id):
        # send ping msg to MCU and return MCU's reply
        rmsg=self.command(pack("<B",id))
        return rmsg
        
