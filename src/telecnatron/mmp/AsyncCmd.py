# ------------------------------------------------------------------------------
# Copyright 2020 Stephen Stebbing. telecnatron.com
#
#    Licensed under the Telecnatron License, Version 1.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        https://telecnatron.com/software/licenses/
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
# ------------------------------------------------------------------------------
#/**
# * @file   AsyncCmd.py
# * @author steves
# * @date   2017/02/22 00:56:10
# * 
# * @brief  
# * 
# */
import logging;
from struct import *
import queue
from telecnatron.mmp.MMP import MMP
from telecnatron.mmp import AsyncCmd
from telecnatron.mmp.MMP import MMPMsg


class CmdResponseMsg(MMPMsg):
    """ """
    def __init__(self):
        """ """
        MMPMsg.__init__(self)
        self.cmd = 0;
        self.status = 0;

    def __str__(self):
        """ """
        return "cmd: {}, status: {}: {}".format(self.cmd, self.status, MMPMsg.__str__(self));
        

class AsyncCmd(MMP):
    """ """

    # message FLAGS bits that we recognise:
    # bit 0, if set and all other bits are zero, then this is a bootloader message
    FLAGS_BIT_BOOT  = 0X0
    # bit 1, if set then this is a CMD message, otherwise it is a ASYNC message
    FLAGS_BIT_CMD   = 0x1

    def set_cmd(self, flags):
        return flags| (0x1 << self.FLAGS_BIT_CMD)
    def is_cmd(self, flags):
        return flags & (0x1 << self.FLAGS_BIT_CMD )

    def set_async(self, flags):
        return ~flags & (0x1 << self.FLAGS_BIT_CMD )
    def is_async(self, flags):
        return ~flags & (0x1 << self.FLAGS_BIT_CMD )

    def is_boot(self, flags):
        return flags == 0x1;
    
    def __init__(self, transport=None):
        """ """
        MMP.__init__(self, transport);
        # init queue for async messages
        self.asyncq = queue.Queue(maxsize=16);
        # init queue for cmd messages
        self.cmdq = queue.Queue(maxsize=1);
        # keep track of number of messages received
        self.num_async = 0;
        self.num_cmd = 0;
        self.num_responses = 0;
        self.errors_unrecognised_msg = 0
        self.errors_invalid_cmd_response = 0
        self.errors_response_timeout = 0
        # set True to have debug messages logged
        self.debug=False


    def handleMsg(self, msg):
        """ """
        # add message to appropiate queue
        logging.debug("got msg: {}".format(msg));
        if self.is_async(msg.flags):
            logging.debug("added msg to async queue");
            self.num_async += 1
            self.asyncq.put(msg)
        elif self.is_cmd(msg.flags):
            logging.debug("added msg to cmd queue");
            self.num_cmd += 1
            self.cmdq.put(msg);
        else:
            logging.warn("Unhandled msg flags: {}".format(msg));
            self.errors_unrecognised_msg += 1



    def sendReceiveCmd(self, cmd, msg_data, timeoutSec = 0.5):
        """ """

        # check that msg is of type bytes
        #logging.info(f" --- sendReceiveCmd: cmd: {cmd}: data: {msg_data}")
        t = type(msg_data)
        #logging.info(f"type of msg: {t}")
        if t != bytes:
            if type(msg_data) == str:
                # it's a string, convert it to bytes
                msg_data=bytes(msg_data, 'utf-8')
            else:
                raise Exception(f"msg_data must be of type bytes or type string but it is {t}")
        #logging.info(f"type of msg: {type(msg_data)}")            

        # flush rx queue
        while not self.cmdq.empty():
            self.cmdq.get_nowait();

        # prepend the msg_data with the cmd byte,
        #logging.info(f"type of msg: {type(msg_data)}, len: {len(msg_data)}")                    
        msg_data = pack("<B", cmd) + msg_data
        if self.debug:
            logging.info(f">>> {msg_data}")
        # send msg
        self.sendMsg(msg_data, flags=self.set_cmd(0));
        # wait for response
        try:
            # get response message
            rmsg= self.cmdq.get(True, timeoutSec)
            if self.debug:
                logging.info(f"<<< {rmsg}")
            # make a new cmd-response objetc
            crmsg = CmdResponseMsg();
            crmsg.flags = rmsg.flags;
            # response data length must be at least two, data[0] being the cmd number, data[1] being the status byte
            if rmsg.len  < 2:
                logging.warn("invalid response message for cmd: {}: msg: {}".format(cmd, rmsg))
                self.errors_invalid_cmd_response += 1
                return None
            # check cmd byte from response message
            crmsg.cmd = rmsg.data[0];
            if not crmsg.cmd == cmd:
                # invalid response: cmd field of received message did not match that of sent message
                logging.warn("invalid response to cmd: {}: {}".format(cmd, rmsg));
                self.errors_invalid_cmd_response += 1
                return None
            # extract status code from the data
            crmsg.status = rmsg.data[1];
            # valid response, remove the cmd and status bytes from data
            crmsg.data= rmsg.data[2:]
            crmsg.len = len(crmsg.data)
            self.num_responses += 1
            #logging.info(f"reply msg received: {crmsg}")
            return crmsg;
        except queue.Empty as e:
            logging.warn("receive timeout");
            self.errors_response_timeout += 1
            return None





