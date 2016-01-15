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
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SI1145.h>
#include <EnableInterrupt.h>
#include <Bounce2.h>

#define DISP_COLOR WHITE
#define DISP_MODE  SSD1306_SWITCHCAPVCC
#define DISP_ADDR  0x3C

#define BATT_DIV_PIN  9
#define BUTT_A        10
#define BUTT_B        6
#define BUTT_C        5
#define BUTT_INTERVAL 10

// Display controller
Adafruit_SSD1306 display = Adafruit_SSD1306();

// UV sensor
Adafruit_SI1145 si1145 = Adafruit_SI1145();

// Buttons
Bounce buttA = Bounce();
Bounce buttB = Bounce();
Bounce buttC = Bounce();

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
  display.setTextColor(WHITE);
}

void textInvert() {
  display.setTextColor(BLACK, WHITE);
}

////////////////////////////////////////////////////////////////////////
// INTERRUPTS
////////////////////////////////////////////////////////////////////////

void irqButtA() {
  buttA.update();
}

void irqButtB() {
  buttB.update();
}

void irqButtC() {
  buttC.update();
}

////////////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////////////

void setup() {

  // Avoid stupid with display/Serial/other hilarity
  delay(200);

  // Init i2C
  Wire.begin();

  // Init display
  display.begin(DISP_MODE, DISP_ADDR);
  display.clearDisplay();
  display.setTextSize(0);
  display.setTextColor(DISP_COLOR);
  display.print(F("UV Dosimeter"));
  display.setTextWrap(false);
  display.display();

  // Init UV sensor
  si1145.begin();

  // Setup buttons & debouncing
  buttA.attach(BUTT_A, INPUT_PULLUP, BUTT_INTERVAL);
  buttB.attach(BUTT_B, INPUT_PULLUP, BUTT_INTERVAL);
  buttC.attach(BUTT_C, INPUT_PULLUP, BUTT_INTERVAL);

  // Attach interrupts so we can sleep more
  enableInterrupt(BUTT_A, irqButtA, CHANGE);
  enableInterrupt(BUTT_B, irqButtB, CHANGE);
  enableInterrupt(BUTT_C, irqButtC, CHANGE);

  // More delay for supersitition?
  delay(500);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  float ir = si1145.readIR();
  float vs = si1145.readVisible();
  float uv = si1145.readUV() / 100.0;

  //buttA.update();
  //buttB.update();
  //buttC.update();

  display.clearDisplay();

  //
  // Display sensor readings
  //

  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print(F("UV "));
  display.print(uv, 2);

  display.setTextSize(0);
  display.setCursor(0, 16);
  display.print(F("IR  "));
  display.print(ir, 0);

  display.setCursor(0, 24);
  display.print(F("Vis "));
  display.print(vs, 0);

  display.setCursor(64, 16);
  display.print(F("Batt "));
  display.print(getBattVoltage(), 2);
  display.print(F("V"));

  //
  // Display button state
  //

  display.setCursor(110, 24);
  if (!buttA.read()) {
    textInvert();
  } else {
    textNormal();
  }
  display.print(F("A"));
  textNormal();

  display.setCursor(116, 24);
  if (!buttB.read()) {
    textInvert();
  } else {
    textNormal();
  }
  display.print(F("B"));
  textNormal();

  display.setCursor(122, 24);
  if (!buttC.read()) {
    textInvert();
  } else {
    textNormal();
  }
  display.print(F("C"));
  textNormal();

  // Display refresh
  display.display();

  delay(1000);

}
