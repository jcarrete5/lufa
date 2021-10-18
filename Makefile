# File to write to when performing an upload
AVR_PORT ?= /dev/ttyACM0

CC = avr-gcc
ifdef DEBUG
CFLAGS = -Og -g -Wall -Wextra -mmcu=atmega32u4 $(DEFS)
else
CFLAGS = -Os -Wall -Wextra -mmcu=atmega32u4 $(DEFS)
endif
DEFS = -DF_CPU=16000000UL
TARGET = rb3-midi-adapter

.PHONY: all
all: $(TARGET)

.PHONY: upload
upload: $(TARGET)
	avrdude -p m32u4 -P $(AVR_PORT) -c avr109 -U flash:w:$(TARGET):e

.PHONY: clean
clean:
	-$(RM) $(TARGET)