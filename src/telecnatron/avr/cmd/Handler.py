# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2017. http://telecnatron.com/
# -----------------------------------------------------------------------------
from struct import unpack, pack ; 
from telecnatron.mmp.MMP import MMP
import logging

# Exceptions
class ENoResponse(Exception):
    pass

class EStatus(Exception):
    pass

# ==========================================================
class Handler():
    """ """
    # the response message received back from MCU by send_recv()
    rmsg = None
    # ----------------------------------------
    def __init__(self, mmp, cmd_num):
        """mmp is reference to object of type: class 'telecnatron.mmp.MMP.MMP' """
        self.cmd_num = cmd_num
        self.mmp=mmp
    
    # ----------------------------------------    
    def command(self, send_bytes=b''):
        r=None
        self.rmsg=self.mmp.sendReceiveCmd(self.cmd_num,send_bytes)
#        self.rmsg=self.mmp.sendReceiveCmd(self.cmd_num,b'0')
        if not self.rmsg == None:
            # we got a response message
            if self.rmsg.status == 0:
                # command was successful, ie return status 0
                self.handle_response()
            else:
                # command failed. Non-zero status.
                self.handle_failure()
            r=self.rmsg
        else:
            # no response
            self.handle_no_response()
        return r
    
    # ----------------------------------------        
    def sub_command(self, scmd=0, data=b''):
        d=bytearray()
        # convert subcommand to byte
        d.extend(pack('<B',scmd))
        # add the data
        d.extend(data)
        return self.command(bytes(d))
    
    # ----------------------------------------
    def handle_no_response(self):
        m="no response from mcu"
        logging.error(m)
        raise ENoResponse(m)
        
    # ----------------------------------------
    def handle_response(self):
        pass
        
    # ----------------------------------------        
    def handle_failure(self):
        m=f"got error response: status: {self.rmsg.status} msg: {self.rmsg}"
        logging.info(m)
        raise EStatus(m)


    # ----------------------------------------
    def rmsg_to_dict(self, pack_str, field_array, rmsg):
        """ Make and return a dictionary with the keys being the names passed in field_array
        rmsg:        telecnatron.mmp.MMP.MMPMsg
        field_array: array of key names
        pack_str:    string used by struct.unpack() to unpack the rmsg data into the dict values
        """
        
        data=unpack(pack_str, rmsg.data)
        # make dict with required keys
        d=dict.fromkeys(field_array)
        # assign the values
        d.update(zip(d, data))
        return d
