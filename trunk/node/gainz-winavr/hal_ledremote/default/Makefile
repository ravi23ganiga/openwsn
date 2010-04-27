###############################################################################
# Makefile for the project lumsensor
###############################################################################

## General Flags
PROJECT = lumsensor
MCU = atmega128
TARGET = lumsensor.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=lumsensor.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = lumsensor.o hal_assert.o hal_cc2420.o hal_cpu.o hal_debugio.o hal_foundation.o hal_interrupt.o hal_led.o hal_spi.o hal_target.o hal_timer.o hal_uart.o hpl_atmega128.o hal_adc.o hal_luminance.o rtl_openframe.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) lumsensor.hex lumsensor.eep lumsensor.lss## Compile
lumsensor.o: ../lumsensor.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_assert.o: ../../../common/hal/hal_assert.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cc2420.o: ../../../common/hal/hal_cc2420.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_cpu.o: ../../../common/hal/hal_cpu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_debugio.o: ../../../common/hal/hal_debugio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_foundation.o: ../../../common/hal/hal_foundation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_interrupt.o: ../../../common/hal/hal_interrupt.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_led.o: ../../../common/hal/hal_led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_spi.o: ../../../common/hal/hal_spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_target.o: ../../../common/hal/hal_target.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_timer.o: ../../../common/hal/hal_timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_uart.o: ../../../common/hal/hal_uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hpl_atmega128.o: ../../../common/hal/hpl_atmega128.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_adc.o: ../../../common/hal/hal_adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hal_luminance.o: ../../../common/hal/hal_luminance.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rtl_openframe.o: ../../../common/rtl/rtl_openframe.c
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

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) lumsensor.elf dep/* lumsensor.hex lumsensor.eep lumsensor.lss lumsensor.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

