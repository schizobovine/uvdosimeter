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

#define DISP_RST_PIN (-1)
#define DISP_MODE    SSD1306_SWITCHCAPVCC
#define DISP_ADDR    0x3C

#define BATT_DIV_PIN 9

// Display controller
Adafruit_SSD1306 display(DISP_RST_PIN);

// UV sensor
Adafruit_SI1145 si1145 = Adafruit_SI1145();

////////////////////////////////////////////////////////////////////////
// HALPING
////////////////////////////////////////////////////////////////////////

float getBattVoltage() {
  float measured = analogRead(BATT_DIV_PIN);
  return measured * 2.0 * 3.3 / 1024;
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
  display.print(F("UV Dosimeter"));
  display.setTextWrap(false);
  display.display();

  // Init UV sensor
  si1145.begin();

  // Serial debugging
  //Serial.begin(9600);
  //while (!Serial)
  //  ;

  pinMode(13, OUTPUT);
  delay(500);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {
 
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print(F("UV Index"));

  display.setCursor(8, 0);
  display.print(F("Batt "));
  display.print(getBattVoltage(), 2);
  display.print(F("V"));

  display.display();

  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);

}
