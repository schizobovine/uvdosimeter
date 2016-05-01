/*
 * uvdosimeter.ino
 *
 * UV dosimeter based on Adafruit's Feather platform.
 *
 * Author: Sean Caulfield <sean@yak.net>
 *
 */

/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <Adafruit_SI1145.h>
#include <EnableInterrupt.h>
#include <Bounce2.h>

#define DISP_PIN_RST  5
#define DISP_PIN_CMD  6
#define DISP_PIN_CS   10
#define BATT_DIV_PIN  9
#define BUTT_PIN      A5
#define BUTT_INTERVAL 10

// Display controller
MicroOLED display(DISP_PIN_RST, DISP_PIN_CMD, DISP_PIN_CS);

// UV sensor
Adafruit_SI1145 si1145 = Adafruit_SI1145();

// Button
Bounce butt = Bounce();

////////////////////////////////////////////////////////////////////////
// HALPING
////////////////////////////////////////////////////////////////////////

float getBattVoltage() {
  pinMode(BATT_DIV_PIN, INPUT);
  delay(50);
  float measured = analogRead(BATT_DIV_PIN);
  pinMode(BATT_DIV_PIN, INPUT_PULLUP);
  return measured * 2.0 * 3.3 / 1024;
}

void textNormal() {
  display.setColor(WHITE);
}

void textInvert() {
  display.setColor(BLACK);
}

////////////////////////////////////////////////////////////////////////
// INTERRUPT(S)
////////////////////////////////////////////////////////////////////////

void irqButt() {
  butt.update();
}

////////////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////////////

void setup() {

  // Avoid stupid with display/Serial/other hilarity
  delay(200);

  // Init display
  display.begin();
  display.clear(ALL);
  display.clear(PAGE); // Since it's apparently not all all
  textNormal();
  display.setFontType(1);
  display.setCursor(22, 8);
  display.print(F("UV"));
  display.setFontType(0);
  display.setCursor(8, 22);
  display.print(F("Dosimeter"));
  display.display();

  // Init i2c & UV sensor
  Wire.begin();
  si1145.begin();

  // Setup button debouncing & attatch interrupt handler for it
  butt.attach(BUTT_PIN, INPUT_PULLUP, BUTT_INTERVAL);
  enableInterrupt(BUTT_PIN, irqButt, CHANGE);

  // More delay for supersitition? Also show title screen.
  delay(500);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  float ir = si1145.readIR();
  float vs = si1145.readVisible();
  float uv = si1145.readUV() / 100.0;

  //butt.update();

  display.clear(PAGE);

  //
  // Refresh sensor readings
  //

  display.setFontType(0);
  display.setCursor(0, 4);
  display.print(F("UV"));
  display.setFontType(1);
  display.setCursor(20, 0);
  display.print(uv, 1);

  display.setFontType(0);
  display.setCursor(0, 12);
  display.print(F("Vis"));
  display.setCursor(20, 12);
  display.print(vs, 0);

  display.setCursor(0, 20);
  display.print(F("IR"));
  display.setCursor(20, 20);
  display.print(ir, 0);

  display.setCursor(0, 28);
  display.print(F("Bat"));
  display.setCursor(20, 28);
  display.print(getBattVoltage(), 2);
  display.print(F("V"));

  //
  // Display button state
  //

  display.setCursor(0, 48);
  if (!butt.read()) {
    textInvert();
  } else {
    textNormal();
  }
  display.print(F("BUTT"));
  textNormal();

  //
  // Display refresh
  //
  display.display();

  delay(1000);

}
