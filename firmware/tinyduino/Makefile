ARDUINO_DIR       = $(realpath $(HOME)/arduino/arduino-current)
ARDUINO_LIBS      := SPI Wire Adafruit_SI1145

ARDUINO_PORT      := /dev/ttyUSB0
USER_LIB_PATH     := $(realpath ./lib)

ARCHITECTURE      = avr
BOARD_TAG         = pro
BOARD_SUB         = 8MHzatmega328
#ALTERNATE_CORE    = adafruit

#CXXFLAGS      += -std=gnu++11 -Wl,-u,vfprintf
#CFLAGS        += -std=gnu11 -Wl,-u,vfprintf
#LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf
CXXFLAGS      += -std=gnu++11 -fno-threadsafe-statics
CFLAGS        += -std=gnu11

include Arduino.mk
