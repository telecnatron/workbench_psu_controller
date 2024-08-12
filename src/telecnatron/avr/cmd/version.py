# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
from struct import unpack, pack ;

from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus

# -----------------------------------
class Version(Handler):
    # -------------------------------
    def version(self):
        rmsg=self.command()
        v=rmsg.data.decode('utf-8')
        # should be something like: c1b1-3-g26e2f24 2.2-1-g14e1c77
        # that is 'APP_GIT_DESCRIBE AVRLIB_GIT_DESCRIBE)
        (a,lib)=v.split(' ')
        return {'app': a, 'lib': lib }

