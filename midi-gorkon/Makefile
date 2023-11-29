BOARD_TAG    = micro

# Platform libs
ARDUINO_LIBS = SPI EEPROM
# User libs
ARDUINO_LIBS+= Control-Surface MIDIUSB

# Workaround for MIDIUSB with 1.8.19: 1819 < 10606 (1.6.6)
ARDUINO_VERSION = 10819

GK_VERSION = $(shell git describe --always --tags)

CPPFLAGS = -DUSB_PRODUCT='"Gorkon"' -DGK_VERSION='"$(GK_VERSION)"'
#CPPFLAGS+= -DGK_DEBUG

include /usr/share/arduino/Arduino.mk
