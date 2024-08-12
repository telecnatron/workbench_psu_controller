# -----------------------------------------------------------------------------
# Copyright Stephen Stebbing 2023. http://telecnatron.com/
# -----------------------------------------------------------------------------
import logging
from struct import unpack, pack ;
from telecnatron.mmp.MMP import MMP
from telecnatron.avr.cmd.Handler import Handler
from telecnatron.avr.cmd.Handler import ENoResponse, EStatus

# -----------------------------------
class INA219(Handler):
    """ Interface and control INA219 Current and Voltage sensor device.
Datasheet: https://home.ss/doc/data/datasheets/misc_ics/sensors/ina219-current-voltage-sensor.pdf
"""
    # subcommands
    SC_READ_CONFIG        = 0
    SC_READ_SHUNT_VOLTAGE = 1
    SC_READ_BUS_VOLTAGE   = 2
    SC_READ_POWER         = 3
    SC_READ_CURRENT       = 4
    SC_READ_CALIBRATION   = 5
    SC_WRITE_CONFIG       = 6
    SC_WRITE_CALIBRATION  = 7

    # ------------------------------------------------
    # device config register values
    # datasheet section 8.6.2
    # +-40mV fullscale
    CONFIG_GAIN_1 = 0x0000
    # 80mV
    CONFIG_GAIN_2 = 0x0800
    # 160mV
    CONFIG_GAIN_3 = 0x1000
    # 320mv
    CONFIG_GAIN_4 = 0x1800
    
    # Bus ADC resolution
    CONFIG_BADCRES_9BIT =       (0x0000)
    CONFIG_BADCRES_10BIT =      (0x0080)
    CONFIG_BADCRES_11BIT =      (0x0100)
    CONFIG_BADCRES_12BIT_1S =   (0x0180)
    CONFIG_BADCRES_12BIT_2S =   (0x0480)
    CONFIG_BADCRES_12BIT_4S =   (0x0500)
    CONFIG_BADCRES_12BIT_8S =   (0x0580)
    CONFIG_BADCRES_12BIT_16S =  (0x0600)
    CONFIG_BADCRES_12BIT_32S =  (0x0680)
    CONFIG_BADCRES_12BIT_64S =  (0x0700)
    CONFIG_BADCRES_12BIT_128S = (0x0780)

    # shunt ADC resolution, number of samples
    CONFIG_SADCRES_9BIT_1S =    (0x0000)
    CONFIG_SADCRES_10BIT_1S =   (0x0008)
    CONFIG_SADCRES_11BIT_1S =   (0x0010)
    CONFIG_SADCRES_12BIT_1S =   (0x0018)
    CONFIG_SADCRES_12BIT_2S =   (0x0048)
    CONFIG_SADCRES_12BIT_4S =   (0x0050)
    CONFIG_SADCRES_12BIT_8S =   (0x0058)
    CONFIG_SADCRES_12BIT_16S =  (0x0060)
    CONFIG_SADCRES_12BIT_32S =  (0x0068)
    CONFIG_SADCRES_12BIT_64S =  (0x0070)
    CONFIG_SADCRES_12BIT_128S = (0x0078)
    
    # operating mode
    CONFIG_MODE_POWERDOWN =           0x00
    CONFIG_MODE_SVOLT_TRIGGERED =     0x01
    CONFIG_MODE_BVOLT_TRIGGERED =     0x02
    CONFIG_MODE_SANDBVOLT_TRIGGERED = 0x03
    CONFIG_MODE_ADCOFF =              0x04
    CONFIG_MODE_SVOLT_CONTINUOUS=     0x05
    CONFIG_MODE_BVOLT_CONTINUOUS=     0x06
    CONFIG_MODE_SANDBVOLT_CONTINUOUS= 0x07


    
    # -------------------------------
    def __init__(self, mmp, cmd_num, i2c_addr=0x40):
        Handler.__init__( self, mmp, cmd_num )
        self.addr=i2c_addr
    # -------------------------------
    def read_config(self):
        rmsg=self.sub_command(INA219.SC_READ_CONFIG, pack("<B",self.addr) )
        v=unpack('>H', rmsg.data)
        return v[0]
        
    # -------------------------------
    def read_shunt_voltage(self):
        rmsg=self.sub_command(INA219.SC_READ_SHUNT_VOLTAGE, pack("<B",self.addr) )
        v=unpack('>H', rmsg.data)
        return v[0]

    # -------------------------------
    def read_bus_voltage(self):
        rmsg=self.sub_command(INA219.SC_READ_BUS_VOLTAGE, pack("<B", self.addr))
        v=unpack('>H', rmsg.data)
        return v[0]

    # -------------------------------
    def read_power(self):
        rmsg=self.sub_command(INA219.SC_READ_POWER, pack("<B", self.addr))
        v=unpack('>h', rmsg.data)
        return v[0]

    # -------------------------------
    def read_current(self):
        rmsg=self.sub_command(INA219.SC_READ_CURRENT, pack("<B", self.addr))
        v=unpack('>h', rmsg.data)
        return v[0]
    
    # -------------------------------
    def read_calibration(self):
        rmsg=self.sub_command(INA219.SC_READ_CALIBRATION, pack("<B", self.addr))
        v=unpack('>H', rmsg.data)
        return v[0]
    
    # -------------------------------
    def write_calibration(self, data):
        b=pack(">BH", self.addr, data)
        logging.info(f"wcal data: 0x{b.hex()}")
        rmsg=self.sub_command(INA219.SC_WRITE_CALIBRATION, b)

    # -------------------------------
    def write_config(self, data):
        b=pack(">BH", self.addr, data)
        logging.info(f"wconfig data: 0x{b.hex()}")
        rmsg=self.sub_command(INA219.SC_WRITE_CONFIG, b)

    # -------------------------------
    def reset(self):
        # set bit 15 of config register to cause device to reset itself.
        self.write_config(0x8000);
        
