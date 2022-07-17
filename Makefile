MCU                 = atmega32u4
ARCH                = AVR8
BOARD               = MICRO
F_CPU               = 16000000
F_USB               = $(F_CPU)
OPTIMIZATION        = 3
TARGET              = rb3-midi-adapter
SRCDIR             := src
SRC                 = $(SRCDIR)/$(TARGET).c $(SRCDIR)/Descriptors.c $(SRCDIR)/MIDI.c $(SRCDIR)/HIDReport.c $(SRCDIR)/PadConfig.c \
                      $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH           = lufa/LUFA
DEFS				=  # User-specified defines
CC_FLAGS            = -DUSE_LUFA_CONFIG_HEADER -IConfig/ $(DEFS)
LD_FLAGS            =
C_STANDARD          = gnu17
AVRDUDE_PORT       ?= /dev/ttyACM0
AVRDUDE_PROGRAMMER  = avr109

all: $(SRCDIR)/PadConfig.h

%: %.mako
	mako-render $< > $@

# Include LUFA-specific DMBS extension modules
DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/avrdude.mk

# Reads in the target EEPROM memory using AVRDUDE
avrdude-read-ee: $(TARGET)-data.eep $(MAKEFILE_LIST)
	@echo $(MSG_AVRDUDE_CMD) Reading device \"$(AVRDUDE_MCU)\" EEPROM using \"$(AVRDUDE_PROGRAMMER)\" on port \"$(AVRDUDE_PORT)\"
	avrdude $(BASE_AVRDUDE_FLAGS) -U eeprom:r:$< $(AVRDUDE_FLAGS)

cleanmako:
	@echo $(MSG_REMOVE_CMD) Removing mako generated files of \"$(TARGET)\"
	find $(SRCDIR)/ -type f -name *.mako | sed 's/\.mako//' | xargs $(RM)

mostlyclean: cleanmako
