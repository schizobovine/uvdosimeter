ARDUINO_LIBS      := Wire MicroView Adafruit_SI1145 Adafruit_SD RTClib RTClib/DS3231 Time Timezone SPI
ARDUINO_PORT      := /dev/ttyUSB0
USER_LIB_PATH     := $(realpath ./lib)
BOARD_TAG         := uno

CXXFLAGS      += -std=gnu++11 -Wl,-u,vfprintf
CFLAGS        += -std=gnu++11 -Wl,-u,vfprintf
LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf

include Arduino.mk
