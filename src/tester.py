#!/usr/bin/python3
# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import sys, os, argparse, threading, binascii, serial;
from queue import Queue;
from datetime import datetime
import time
import traceback;
import logging;
import random
from struct import * 

from telecnatron.mmp.MMP import MMP;
from telecnatron.mmp.transport import SerialTransport
from telecnatron.mmp.AsyncCmd import AsyncCmd
#from telecnatron.mmp import *;
from telecnatron.util import logging_config
from telecnatron.avr.cmd.clock import Clock
from telecnatron.avr.cmd.ping import Ping
from telecnatron.avr.cmd.version import Version
#from telecnatron.avr.cmd.PCF8574 import PCF8574
from telecnatron.avr.cmd.LCD import LCD
from telecnatron.avr.cmd.INA219 import INA219
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse
from devices import Fan, Load, Shutdown, Measurements
from config import MMPCmd
# -------------------------------------------
# flag to run/stop main loop

def exit_nicely(level=0):
    # signal msg thread to exit
    mmp.stop();
    print("exited")
    sys.exit(level)

def listSerial():
    "List available serial ports"
    try:
        from serial.tools.list_ports import comports
    except ImportError:
        comports = None
    print(comports())

# -----------------------------------------------------------------------------
if __name__ == '__main__':

    # parse  command line options
    # type for hexidecimal arguments 
    def auto_int(x):
        return int(x, 0)
    argp = argparse.ArgumentParser()
    argp.add_argument('-p','--port', help="Name of serial port eg: /dev/ttyUSB0.", default="/dev/ttyAMA0")
    argp.add_argument('-b','--baud', default=38400, help="Baud rate, default 38400.")
    argp.add_argument('-l','--list-serial', action='store_true', help="List available serial ports and exit.")
    argp.add_argument('-sc','--set-clocks', action='store_true', help="Set the clock and sysclk seconds on the MCU to the current time.")
    argp.add_argument('-rb','--reboot_mcu', action='store_true', help="reboot the MCU.")
    argp.add_argument('-tf','--tick_freq', default=1000, help="set the MCU ticks per second value.")
    argp.add_argument('-rj','--reset-joules', action='store_true', help="reset the count of joules to zero.")
    args = argp.parse_args()

    # logger
    #logging_config(level=logging.INFO);
    # configure logging format
    logging.basicConfig(format='LOG:%(levelname)s:%(asctime)s:%(created)u:%(filename)s:%(lineno)d: %(message)s', datefmt="%H%M%S", level=logging.INFO)
    

    # list serial ports if specified
    if(args.list_serial):
        listSerial();

    try:
        mmp = AsyncCmd(SerialTransport(args.port, args.baud, timeoutSec=0.008));
        #        mmp.rebootMCU();
        #mmp.debug=True
        
        # Object that communicate with MCU over MMP
        ping=Ping(mmp, MMPCmd.CMD_PING)
        version=Version(mmp,MMPCmd.CMD_VERSION)
        clk=Clock(mmp, MMPCmd.CMD_CLOCK)
        # mmp objects specific to this project
        fan=Fan(mmp,MMPCmd.CMD_FAN)
        load=Load(mmp,MMPCmd.CMD_LOAD_SWITCH)
        shtdwn=Shutdown(mmp, MMPCmd.CMD_SHTDWN)
        measurements=Measurements(mmp, MMPCmd.CMD_MEASUREMENTS)
        #measurements.reset()
        #shtdwn.shutdown()
        shtdwn.restart()
        if False:
            ina219=INA219(mmp,6)
            # 0 0 0
            ina219.reset()
            conf=0x0000
            conf = conf | ina219.CONFIG_GAIN_1 | ina219.CONFIG_BADCRES_12BIT_4S | ina219.CONFIG_SADCRES_12BIT_4S | ina219.CONFIG_MODE_SANDBVOLT_CONTINUOUS
            logging.info(f"about to write 0x{conf:2x} to config register")
            #ina219.write_config(conf)
            logging.info(f"read config: {ina219.read_config():02x}")
            cal=0x0066
            logging.info(f"about to write 0x{cal:2x} to calibration register")        
            ina219.write_calibration(cal)
            logging.info(f"read cal: {ina219.read_calibration():02x}")        
        
        if args.reboot_mcu:
            mmp.rebootMCU()
            logging.info("rebooted MCU...")
            # wait a bit for it to startup
            time.sleep(6)

        if args.reset_joules:
            measurements.reset()
            
        # keep track of when we set the time
        start_sec = round(time.time())
        if args.set_clocks:
            # set the time on the MCU
            clk.set_clock()
            clk.set_sysclk()
            logging.info(f"set MCU seconds to {start_sec} because that is the unix time right now")

        if args.tick_freq!=1000:
            # it's not the default, therefore set it
            clk.set_tick_freq(args.tick_freq)
            
        # loop count
        lc=0
        while True:
            try:
                #print("---- loop ----")
                #print(f"read clock: {clk.read()}")            
                #print(f"ping        {ping.ping(4)}")
                #print(f"version:    {version.version()}")

                # toggle
                if lc % 2 ==0:
                    pass
                    # turn fan on
                    #fan.control(1)
                    #lcd.backlight(True)
                    # read the load switch
                    #load.read()
                    #shtdwn.shutdown()
                else:
                    #shtdwn.restart()                    
                    #fan.control(0)
                    #lcd.backlight(False)
                    pass
                    
                # ina219 data
                if False:
                    rbv=ina219.read_bus_voltage()
                    sv=ina219.read_shunt_voltage()
                    #bv=0.0005 * rbv
                    #sa=sv * 10e-5
                    #w=sa*bv
                    cal=ina219.read_calibration()
                    conf=ina219.read_config()
                    p=ina219.read_power()
                    cur=ina219.read_current()
                    #logging.info(f"voltages: bus: {rbv} {rbv:#x}  shunt: {sv} {sv:#x} , cal: {cal:#x}, conf: {conf:#x}, cur: {cur} {cur:#x}, power: {p} {p:#x}, {bv:5.3f} volts, {sa:5.3f} A, {w:6.3f} W")
                    logging.info(f"voltages: bus: {rbv} {rbv:#x}  shunt: {sv} {sv:#x} , cal: {cal} {cal:#x}, conf: {conf:#x}, cur: {cur} {cur:#x}, power: {p} {p:#x}")
                    ina219.reset()
                
                #shtdwn.read()
                
                if False:
                    lcd.gotoxy(0,0)
                    lcds=f"{lc}: {datetime.now()}"
                    lcds=f"{bv:5.3f}V {sa:5.3f}A"
                    lcd.puts(lcds.ljust(32))

                logging.info(measurements.read())
                    
                if lc % 5 == 0:
                    # check MCU time error
                    mcu_clk=clk.read()
                    mcu_utime = mcu_clk['seconds']
                    mcu_tfreq=mcu_clk['tick_freq']
                    now = round(time.time())
                    diff=mcu_utime-now;
                    rsec=now-start_sec
                    # sc is number of seconds since MCU was rebooted
                    sc=mcu_clk['seconds_count']
                    if diff > 0:
                        s='fast'
                    else:
                        s='slow'
                    if rsec>0:
                        diff_r=diff/sc
                        tfcorrection= mcu_tfreq * (1+diff_r)
                        #logging.info(f"mcu is time is {s} by {abs(diff)} seconds over {sc} seconds, that is {diff_r*100:6.2f}%, {round(diff_r * 3600 * 24)} seconds per day, MCU tick_freq should be set to {round(tfcorrection, 2)} (currently: {mcu_tfreq})")
                    
                lc+=1
            except ENoResponse as e:
                print(e)
                
            
            # check whether mmp reader tread has exited
            if not mmp.reader.is_alive():
                break;
            
            while not mmp.asyncq.empty():
                amsg= mmp. asyncq.get_nowait();
                logging.info("PC RX async: {}".format(amsg));

            # delay before next time through loop.
            time.sleep(2)
                
    except KeyboardInterrupt:
        if True:
            # CTRL-C pushed, display stats
            mmp.display_stats()

    except Exception as e:
        traceback.print_exc();
        logging.error("Caught exception: {}".format(e))
        exit_nicely(1);

    exit_nicely();

#    serial.write(pack('cBccccccB','S',4,'T','a','b','c','d','E',0x8e))
#    serial.write(pack('cBccccccB','S',4,'T','B','b','c','d','E',0x6f))



