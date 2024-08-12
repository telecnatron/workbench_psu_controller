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
import sys, os, threading, time, binascii, traceback, logging
from struct import * ;
from telecnatron.mmp.transport import Transport


class MMPMsg:
    """ Class represents a MMP message"""
    def __init__(self):
        """ """
        self.data=bytearray();
        # expected length of the message/ lenght of the received message, 1 byte
        self.len=0;
        # flags, 1 byte
        self.flags=0;
        # count of the number of data bytes received so far
        self.count=0;

    def __str__(self):
        s="data length: {}, ".format(self.len)
        s = s+ "flags: 0x{:02x}, ".format(self.flags)
        s = s + "data: "
        for ch in self.data:
            s = s + "0x{:x},".format(ch)
        s = s+ " str: "
        for ch in self.data:
            s = s + "{}".format(chr(ch))
        return s

       

# -------------------------------------------        
class MMP:
    """ """
    # byte indicating start of message
    MSG_SOM=b'\1'
    MSG_STX=b'\2'
    MSG_ETX=b'\3'

    # string used to match logger strings
    LOGS = b'	LOG'
    # max length of log string
    LOGS_MAX = 255;

    def __init__(self, transport=None):
        """ """
        self.alive= True
        self.transport=transport
        # Default message flags byte, used if None is specified in method calls
        self.flags=1
        if transport == None:
            # use default transport
            logging.warn("MMP is using default transport")
            self.transport=Transport()
        # keep track of how many errors have occured
        self.errors_rx = 0
        self.errors_timeout = 0
        self.errors_log = 0;
        # keep track of how many messages and log strings have been received
        self.num_log = 0;
        self.num_msg = 0;
        # init reader thread
        self.reader = threading.Thread(target=self.readerThread)
        self.reader.start();


    def stop(self):
        """ """
        self.alive=False


    def readByte(self):
        """ """
        return self.transport.readByte();


    def write(self, databytes):
        """ """
        self.transport.write(databytes);


    def nonHandledByte(self, byte):
        """ """
        #print "nh: {:s}".format(byte);
        pass


    def logStrReceived(self, logStr):
        """ """
        # don't print leading \t
        logging.info(f"MCU: {logStr}")


    def calcCS(self, intCS):
        return (256 - intCS) %256


    def handleMsg(self, msg):
        """ Called when a message is received. Expected to be overriden in subclasses. """
        logging.info(f"PCRX: len: {msg.len}, str: {str(msg.data)}");


    def rebootMCU(self):
        """ Send MCU message telling it to reboot itself."""
        self.sendMsg('r',flags=0x1)

    def sendMsg(self,  msg_data, flags=None):
        """ """
        # use default flags if not specified as parameter
        if flags == None:
            flags=self.flags;

        # check that msg is of type bytes
        if type(msg_data) != bytes:
            if type(msg_data) == str:
                # it's a string, convert it to bytes
                msg_data=bytes(msg_data, 'utf-8')
            else:
                raise Exception(f"msg_data must be of type bytes or type string but it is {type(msg_data)}")
            
        length = len(msg_data)
        # make up  header
        # checksum
        cs=length+flags;
        m=pack('<cBBc', self.MSG_SOM, length, flags, self.MSG_STX);
        # add data
        for ch in msg_data:
            # XXX python3: ch is type 'string', needs to be 'bytes' for pack() to work
            #print(f"type of ch: {type(ch)}")
            if type(ch)==int:
                # XXX should always be int?
                m += pack('<c',ch.to_bytes(1,'big'));
            else:
                m += pack('<c',ch)
            #cs += ord(ch)
            cs += ch
            cs = cs % 256
        # pack ETX
        m += pack('<c', self.MSG_ETX)
        # pack CS
#        m += pack('<B', self.calcCS(cs));
        m += pack('<B', cs);
        #logging.info(f"msg about to be sent: {m}, data: {msg_data}")
        self.write(m)


    def readerThread(self):
        """ """
        # states
        SIDLE  = 0
        SLOG   = 1
        SSTX   = 2
        SDATA  = 3
        SETX   = 4
        SLEN   = 5
        SCS    = 6
        SFLAGS = 7
        
        logi = 0;
        state = SIDLE;
        logstr = b'';
        msg = MMPMsg();
        # checksum
        cs = 0;
        while self.alive:
            try:
                c = self.readByte();
                #logging.info(f"type c: {type(c)}")
                if(c != None and len(c) != 0):
                    # char (python string) has been received,
                    #if ( ord(c) >= 32 and ord(c)<=127):
                    #    a=c;
                    #else:
                    #    a='_'
                    #s="~0x{:02x}={}".format(ord(c),a);
                    #print s,
                    
                    # ---------------------------------
                    if state == SIDLE:
                        if c == self.MSG_SOM:
                            # got start byte
                            logi = 0
                            state = SLEN
                            msg = MMPMsg();
                            logging.debug("-SOM-")
                        else:
                            if c == self.LOGS[logi].to_bytes(1,'big'):
                                #logging.info(f"received ch: {c}, logi: {logi}: {self.LOGS[logi].to_bytes(1,'big')}, {len(self.LOGS)}")
                                # got a LOGS char
                                logi += 1
                                if logi == len(self.LOGS):
                                    # Yup, got all the LOGS chars, this is a LOG string
                                    #logging.info(f"got log string!")
                                    logstr=self.LOGS;
                                    state = SLOG
                                    logi=0;
                            else:
                                # nope, false alarm, char received was not start of message, not start of log string
                                logi=0;
                                self.nonHandledByte(c)
                    # ---------------------------------
                    elif state == SLOG:
                        # we're receiving a log msg
                        if c == b'\n':
                            # this is end of log string
                            #logging.info(f"end of log string: {c}, type: {type(logstr)}")
                            self.num_log += 1
                            # don't give them the leading \t
                            #logging.info(f"type of logstr: {type(logstr)}, str: {logstr}")
                            self.logStrReceived(logstr[1:])
                            state = SIDLE
                            logstr = b'';
                        else:
                            logstr = logstr + c;
                            if len(logstr) > self.LOGS_MAX:
                                logging.warn("log string is too long. Truncated: "+logstr.decode('utf-8')+"\n")
                                self.errors_log += 1;
                                self.logStrReceived(logstr)
                                state = SIDLE
                                logstr = b'';
                    # ---------------------------------
                    elif state == SLEN:
                        # recived char is message length
                        msg.len = ord(c)
                        cs = msg.len 
                        logging.debug("-LEN-")
                        state = SFLAGS

                    elif state == SFLAGS:
                        # recived char is message flags
                        msg.flags = ord(c)
                        cs += ord(c) 
                        logging.debug("-FLAGS-")
                        state = SSTX

                    elif state == SSTX:
                        if (c == self.MSG_STX):
                            # got STX char as expected
                            logging.debug("-STX-")
                            state = SDATA
                        else:
                            # did not get SSTX char when expected
                            logging.debug("!STX not seen!")
                            self.errors_rx += 1
                            state = SIDLE
                    elif state == SDATA:
                        # we're receiving msg data.
                        # XXX note: msg must contain at least one byte of data, ie no zero sized data is allowed here
                        msg.data.extend(c);
                        #logging.debug("d:"+c)
                        cs += ord(c)
                        msg.count += 1
                        if msg.count == msg.len:
                            # that was the last of the data
                            state = SETX

                    elif state == SETX:
                        if( c == self.MSG_ETX ):
                            # got ETX char as expected.
                            logging.debug("-ETX-")
                            state = SCS
                        else:
                            # did not get ETX char as expected
                            self.errors_rx += 1
                            logging.debug("!ETX not seen!")

                    elif state == SCS:
                        if ord(c) == self.calcCS(cs) :
                            # checksum was valid, message is complete
                            self.num_msg += 1
                            self.handleMsg(msg)
                        else:
                            # checksum was invalid
                            self.errors_rx += 1
                            logging.debug("!invalid checksum, expected: 0x{:x} got 0x{:x}".format(self.calcCS(cs), ord(c)))
                        state = SIDLE

                else:
                    # timeout: no byte received
                    if state != SIDLE:
                        logging.debug("timeout")
                        self.errors_timeout += 1
                        state = SIDLE
                    pass

            except Exception as e:
                logging.error("Caught exception in reader thread: {}".format(e))
                traceback.print_exc(e)


    def display_stats(self):
        print("") 
        print("====== STATISTICS =======")
        print("")
        print ("messages received:     {}".format(self.num_msg))
        print ("log messages received: {}".format(self.num_log))
        print ("rx errors:             {}".format(self.errors_rx))
        print ("timeout errors:        {}".format(self.errors_timeout))
        print ("log errors:            {}".format(self.errors_log))
        print("")
        print ("async messages received: {}".format(self.num_async))
        print ("cmd messages sent:       {}".format(self.num_cmd))
        print ("cmd respones received:   {}".format(self.num_responses))
        print ("unrecognised messages:   {}".format(self.errors_unrecognised_msg))
        print ("invalid response errors: {}".format(self.errors_invalid_cmd_response))
        print ("response timeout errors: {}".format(self.errors_response_timeout))
        print("")

        

    
