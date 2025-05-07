# No way to write build-properties so a Makefile is needed
# https://forum.arduino.cc/t/compiler-options-in-arduino-cli/938124/2

FW_VERSION := 1.0.0

FQBN := arduino:avr:micro
PORT := /dev/ttyACM0
BUILD := build
FLAGS := -DFW_VERSION=\"$(FW_VERSION)\"
USB_NAME := VadersWrapper

ARDUINO := arduino-cli

SKETCH := $(wildcard *.ino)
LIBS := $(wildcard *.cpp *.h *.tcc)
HEX := $(BUILD)/$(subst :,.,$(FQBN))/$(addsuffix .hex, $(SKETCH))

ifeq ($(CLEAN),1)
CLEANOPT := --clean
endif

all: $(HEX)

$(HEX): $(SKETCH) $(LIBS)
	$(ARDUINO) compile $(CLEANOPT) --verbose --log -b $(FQBN) \
		--build-property compiler.cpp.extra_flags="$(FLAGS)" \
		--build-property build.usb_product='"$(USB_NAME)"' -e

upload: $(HEX)
	$(ARDUINO) upload -b $(FQBN) -p $(PORT) -i $(HEX)

clean:
	rm -rf $(BUILD)
	$(ARDUINO) cache clean
