MCU                 = atmega32u4
ARCH                = AVR8
BOARD               = MICRO
F_CPU               = 16000000
F_USB               = $(F_CPU)
OPTIMIZATION        = s
TARGET              = rb3-midi-adapter
SRC                 = $(TARGET).c Descriptors.c MIDI.c HIDReport.c PadConfig.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH           = lufa/LUFA
DEFS				=  # User-specified defines
CC_FLAGS            = -DUSE_LUFA_CONFIG_HEADER -IConfig/ $(DEFS)
LD_FLAGS            =
AVRDUDE_PORT       ?= /dev/ttyACM0
AVRDUDE_PROGRAMMER  = avr109

all:

# Include LUFA-specific DMBS extension modules
DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/avrdude.mk
