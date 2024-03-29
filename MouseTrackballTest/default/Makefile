###############################################################################
# Makefile for the project mouse_trackball_test
###############################################################################

## General Flags
PROJECT = mouse_trackball_test
MCU = atmega324p
TARGET = mouse_trackball_test.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -O0
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = vt100.o buffer.o cmdline.o mouse_trackball_test.o rprintf.o uart2.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) mouse_trackball_test.hex mouse_trackball_test.eep size

## Compile
vt100.o: ../vt100.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

buffer.o: ../buffer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

cmdline.o: ../cmdline.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mouse_trackball_test.o: ../mouse_trackball_test.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rprintf.o: ../rprintf.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart2.o: ../uart2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) mouse_trackball_test.elf dep/* mouse_trackball_test.hex mouse_trackball_test.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

