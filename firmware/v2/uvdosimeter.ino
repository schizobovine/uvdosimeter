#include<Arduino.h>
#include<Wire.h>
#include<SoftwareSerial.h>
#include<Adafruit_SI1145.h>
#include "usec.h"

const int SERIAL_BAUD = 38400;
const int SENSOR_ADDR = 0x60;
const int DISPLAY_RX = 9;
const int DISPLAY_TX = 10;

const byte DISPLAY_CLEAR   = 0x76;
const byte DISPLAY_DP_CNTL = 0x77;
const byte DISPLAY_CURSOR  = 0x79;
const byte DISPLAY_BRIGHT  = 0x7A;

const byte DISPLAY_APOSTROPHE = 1<<5;
const byte DISPLAY_COLON = 1<<4;
const byte DISPLAY_DP4 = 1<<3;
const byte DISPLAY_DP3 = 1<<2;
const byte DISPLAY_DP2 = 1<<1;
const byte DISPLAY_DP1 = 1<<0;

const byte DISPLAY_CURSOR_DIG1 = 0;
const byte DISPLAY_CURSOR_DIG2 = 1;
const byte DISPLAY_CURSOR_DIG3 = 2;
const byte DISPLAY_CURSOR_DIG4 = 3;

const int INTERVAL = 500;

// ------------------------------------------------------------
// GLOBAL CONSTANTS/VARIABLES
// ------------------------------------------------------------

Adafruit_SI1145 uv = Adafruit_SI1145();
SoftwareSerial display = SoftwareSerial(DISPLAY_RX, DISPLAY_TX);

// ------------------------------------------------------------
// "HELPING" FUNCTIONS
// ------------------------------------------------------------

void displayInit() {
  display.begin(9600);
  display.write(DISPLAY_CLEAR);
  display.write(DISPLAY_BRIGHT);
  display.write((byte)255);
}

void showIndex(int uvIndex) {

  display.write(DISPLAY_CURSOR);
  display.write(DISPLAY_CURSOR_DIG1);
  display.write(DISPLAY_DP_CNTL);
  display.write(DISPLAY_DP1);

  char buff[16];
  snprintf(buff, 16, "%04.4d", uvIndex);
  display.print(buff);

}

void flashError(byte pin, usec pulseLen=250) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(pulseLen);
  digitalWrite(pin, LOW);
  delay(pulseLen);
}

// ------------------------------------------------------------
// SETUP() - THIS THING IS DONE ONCE
// ------------------------------------------------------------

void setup() {

  displayInit();

  Wire.begin();
  while (!uv.begin()) {
    flashError(13);
  }

}

// ------------------------------------------------------------
// LOOP() - THIS THING IS DONE LOTS, LIKE YOUR MOM
// ------------------------------------------------------------

void loop() {
  int reading = uv.readUV();
  showIndex(reading);
  delay(INTERVAL);
}
