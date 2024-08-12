# -----------------------------------------------------
# Stephen Stebbing. 2016. http://telecnatron.com/
# -----------------------------------------------------
# CPU_SC should be one of: ATMEGA328, ATMEGA168, ATMEGA88
CPU_SC=ATMEGA328

# baud rate for uart.
# If baudrate is required in app is different to that used in the bootloader,
# it can be set by having app call:  uart_reset_baud() in lib/boot/util.h
BAUD=38400

# serial  port that is used for programming via the bootloader
# To use another port, try something like:
#   make program -E PORT=/dev/ttyS0
ifndef PORT
PORT=/dev/ttyAMA0
endif

# CPU clock frequency
F_CPU=16000000

# To have application use bootloader functions define USE_BOOTLOADER, ie:
# don't use bootloader: 
#USE_BOOTLOADER=
# use bootloader:
#USE_BOOTLOADER=1
#USE_BOOTLOADER=1

# defines
DEFS = -D F_CPU=$(F_CPU) -D $(CPU_SC)  -D BAUD=$(BAUD) 
#DEFS+= -D MMP_USE_LOGGER -D MMP_NO_REBOOT
# -----------------------------------------------------
# list source file here:
# C sources
LIBS = lib/sysclk.c lib/task.c lib/log.c lib/util.c lib/wdt.c lib/mmp/mmp_cmd.c  lib/rtc/clock.c  lib/i2c/pcf8574.c lib/lcd/lcd_i2c.c lib/devices/ina219.c lib/adc.c
#LIBS += lib/mmp/drivers/pcf8574.c lib/mmp/drivers/lcd.c lib/mmp/drivers/ina219.c lib/mmp/drivers/stdcmd.c
LIBS += lib/i2c/i2c_master.c lib/mmp/drivers/stdcmd.c lib/mmp/drivers/clock.c
SOURCES =  $(LIBS) main.c    load_switch.c shtdwn.c lcd.c ina219.c drivers.c 

ifdef USE_BOOTLOADER
SOURCES += lib/boot/boot_functions.c 
else
SOURCES += lib/uart/uart.c ./lib/mmp/mmp.c
endif
$(info SOURCES: $(SOURCES))

# ASM sources
ASOURCES = 

# include files
INCLUDES=config.h

BUILD_DIR = build

MAIN_ELF=$(BUILD_DIR)/main.elf
MAIN_HEX=$(BUILD_DIR)/main.hex
APP_BIN=$(BUILD_DIR)/app.bin
# -----------------------------------------------------
# ATMEGA168
# -----------------------------------------------------
ifeq ($(CPU_SC),ATMEGA168)
# CPU as used by avrdude
CPU_AVRDUDE=m168p
# CPU as used my GCC
CPU_MMCU=atmega168

ifdef USE_BOOTLOADER
# byte address of start of bootloader
BOOT_START=0x3800
# word address of start of bootloader
BOOT_START_WORD=0x1C00
# Start address of boot_ftab structure - this should be address of end 
# of flash (eg 0x7FFF on 32k flash atmega328) minus size of the boot_ftab structure.
# See /usr/lib/avr/include/avr/ioXXX.h where XXX specifies processor type:
#   eg iom328p.h for atmega328p. In this file FLASHEND is defined.
# See lib/boot/boot_ftab.S
BOOT_FTAB_START=0x3FE0
endif # USE_BOOTLOADER
else
# -----------------------------------------------------
# ATMEGA328
# -----------------------------------------------------
ifeq ($(CPU_SC),ATMEGA328)
# CPU as used by avrdude
CPU_AVRDUDE=m328p
# CPU as used my GCC
CPU_MMCU=atmega328

ifdef USE_BOOTLOADER
# Start address of boot_ftab structure - this should be address of end 
# of flash (eg 0x7FFF on 32k flash atmega328) minus size of the boot_ftab structure.
# See /usr/lib/avr/include/avr/ioXXX.h where XXX specifies processor type:
#   eg iom328p.h for atmega328p. In this file FLASHEND is defined.
# See lib/boot/boot_ftab.S
BOOT_FTAB_START=0x7FE0
endif # USE_BOOTLOADER
# -----------------------------------------------------
# Other CPUs
# -----------------------------------------------------
else 
#error CPU not supported
endif
endif
# -----------------------------------------------------
CC=avr-gcc

# C flags
CFLAGS = -Os -mmcu=$(CPU_MMCU) -I. -Wall
#CFLAGS += -fdata-sections -ffunction-sections
# limit use of compiler inlining and call prologs (whatever that is)
CFLAGS += -finline-limit=3  -fno-inline-small-functions
CFLAGS +=  -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wa,-adhlns=$(<:.c=.lst) -gstabs 
CFLAGS += -fno-move-loop-invariants -fno-tree-scev-cprop -fno-jump-tables
CPFLAGS = -std=gnu99 

# linker flags
# instruct linker to gc unused sections
LDFLAGS =  -Wl,--relax -Wl,-gc-sections  
LDFLAGS += -Wl,-Map,$(BUILD_DIR)/main.map
#LDFLAGS += -Wl, -u crc16_update
# sprintf for floating point
LDFLAGS +=-Wl,-u,vfprintf  -lprintf_flt

# ---------------------------------------------
ifdef USE_BOOTLOADER
# Extra stuff for when using bootloader:
# move start of data section up so there is room for shared globals
LDFLAGS += -Wl,-section-start=.data=$(DATA_START)
# boot_ftab section
LDFLAGS += -Wl,-section-start=.boot_ftab=$(BOOT_FTAB_START) -Wl,--undefined=_boot_ftab
# Specify start of data segment (usually 0x800000),
# we move it up to allow for global data that is shared between
# bootloader and application.
# See lib/boot/globals.h for what this value should be set to.
DATA_START=0x800107
# Extra defines for when using bootloader
DEFS += -D BOOT_FTAB_START=$(BOOT_FTAB_START)
# For bootloader (ie application is the bootloader itself), we define BOOT
#DEFS += -D BOOT 
# For application that calls into the bootloader, we define BOOT_APP
DEFS += -D BOOT_APP -D BOOT_SPACE_1024_WORDS
endif # USE_BOOTLOADER
# ---------------------------------------------

# .o files
#OBJS=$(SOURCES:.c=.o)
#OBJS += $(ASOURCES:.S=.o)
OBJS = $(patsubst %, $(BUILD_DIR)/%, $(SOURCES:.c=.o))
$(info OBJS: $(OBJS))

# .lst files
LISTS=$(SOURCES:.c=.lst)

# ---------------------------------------------
all: $(APP_BIN)

config.h config.c: config.def config.h.inc
	./configure.py

#$(BUILD_DIR)/MAIN_O: main.c $(INCLUDES) config.h 
#	$(CC) -c $(CFLAGS) $(CPFLAGS) $(DEFS) -o $@ $<	

$(OBJS): $(SOURCES) config.h
#	$(CC)  -c $(CFLAGS) $(CPFLAGS) $(DEFS) -I . -o $@ $<
	$(CC)  -c $(CFLAGS) $(CPFLAGS) $(DEFS) -I . -o $@  $(subst build/, , $(@:.o=.c))

#%.o: %.c config.h
#	$(CC)  -c $(CFLAGS) $(CPFLAGS) $(DEFS) -I . -o $@ $<


$(MAIN_ELF): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(CPFLAGS) -o $(MAIN_ELF) $(OBJS)

$(MAIN_HEX) : $(MAIN_ELF)
	avr-objcopy  -R .eeprom -O ihex $(MAIN_ELF)  $(MAIN_HEX)

$(APP_BIN): $(MAIN_HEX)
	avr-objcopy  -R .eeprom  -O binary $(MAIN_ELF)  $(APP_BIN) 
	avr-size $(MAIN_ELF)	 

program:
	/home/steves/bin/bootavr -nv --port $(PORT) -rb -fw $(APP_BIN) -r
#	sudo avrdude  -c usbasp -p $(CPU_AVRDUDE) -U flash:w:main.hex:a
# ---------------------------------------------
size: $(APP_BIN)
	avr-size main.o main.elf
	ls -al $(APP_BIN)

DISASSEMBLE:
	avr-objdump -S --disassemble main.elf | less

clean:
	rm -f $(OBJS) 
	rm -f $(LISTS)
	rm -f mcui.defs
	rm -f  *.elf *.map $(APP_BIN) $(APP_HEX) $(MAIN_HEX) config.c config.h
