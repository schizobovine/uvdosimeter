/*
 * uvdosimeter-dirty.ino
 *
 * Quick and dirty UV index sensor for vacation.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <Arduino.h>
#include <Wire.h>
#include <MicroView.h>
#include <Adafruit_SI1145.h>

const int BLUART_RX  = 5;
const int BLUART_TX  = 3;
const int BLUART_CTS = 2;
const int BLUART_RTS = 6;
const int BLUART_MOD = -1;

Adafruit_SI1145 si1145 = Adafruit_SI1145();

float visible;
float ir;
float uv;

void setup() {
  
  // Turn out display and print out default image
  uView.begin();
  uView.clear(ALL);
  uView.setFontType(0);
  uView.display();

  // Find our UV sensor
  Wire.begin();
  while (!si1145.begin()) {
    uView.clear(PAGE);
    uView.setCursor(0, 0);
    uView.println("Could not find UV Sensor!");
    uView.display();
    delay(5000);
  }
  
  delay(500);

}

void printDisplay() {

  PGM_P FMT = PSTR(
         "Vis %#6.1f"
     "IR  %#6.1f"
     "UV  %#6.1f"
  );

  char buff[64];
  snprintf_P(buff, sizeof(buff), FMT, visible, ir, uv);
  buff[sizeof(buff) - 1] = '\0';

  uView.clear(PAGE);
  uView.setCursor(0, 0);
  uView.println("UV Dosimtr");
  uView.print(buff);
  uView.display();

}

void loop() {
  
  visible = si1145.readVisible();
  ir = si1145.readIR();
  uv = si1145.readUV();

  printDisplay();

  delay(1000);

}
