ARDUINO_DIR       = $(realpath $(HOME)/arduino/arduino-current)
ARDUINO_LIBS      := SPI Wire MicroView/src Adafruit_SI1145 Bounce2 EnableInterrupt

ARDUINO_PORT      := /dev/ttyACM0
USER_LIB_PATH     := $(realpath ./lib)

ARCHITECTURE      = avr
ALTERNATE_CORE    = adafruit
BOARD_TAG         = feather32u4

#CXXFLAGS      += -std=gnu++11 -Wl,-u,vfprintf
#CFLAGS        += -std=gnu11 -Wl,-u,vfprintf
#LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf
CXXFLAGS      += -std=gnu++11 -fno-threadsafe-statics
CFLAGS        += -std=gnu11

include Arduino.mk
