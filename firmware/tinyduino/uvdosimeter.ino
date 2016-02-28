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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SI1145.h>

#define SERIAL_DEBUG 0
#define SERIAL_SPEED 9600

//
// Tinyduino 16 LED Charliplexing Setup
//
//       I I I I I
//       O O O O O
//       5 6 7 8 9
// LED3  H   L
// LED4  L   H
// LED5    H L
// LED6    L H
// LED7    H   L
// LED8    L   H
// LED9  H     L
// LED10 L     H
// LED11     L H
// LED12     H L
// LED13     L   H
// LED14     H   L
//


const uint8_t NUM_LEDS = 12;

const uint8_t LED_LOW[NUM_LEDS] = {
  //6, // LED1  (unused)
  //5, // LED2  (unused)
  7, // LED3
  5, // LED4
  7, // LED5
  6, // LED6
  8, // LED7
  6, // LED8
  8, // LED9
  5, // LED10
  7, // LED11
  8, // LED12
  7, // LED13
  9, // LED14
  //8, // LED15 (unused)
  //9, // LED16 (unused)
};

const uint8_t LED_HIGH[NUM_LEDS] = {
  //5, // LED1  (unused)
  //6, // LED2  (unused)
  5, // LED3
  7, // LED4
  6, // LED5
  7, // LED6
  6, // LED7
  8, // LED8
  5, // LED9
  8, // LED10
  8, // LED11
  7, // LED12
  9, // LED13
  7, // LED14
  //9, // LED15 (unused)
  //8, // LED16 (unused)
};


// UV sensor
Adafruit_SI1145 si1145 = Adafruit_SI1145();

// Current UV index reading
uint8_t uvidx = 0;

// Microseconds since last reading
uint32_t last_reading = 0;

// Time (in microseconds) between readings
const uint32_t SENSE_INTERVAL_US = 1L << 20; // 1048576

// Time (in microseconds) to flash LEDs on for
const uint32_t LED_ON_US = 1L << 10; // 1024

////////////////////////////////////////////////////////////////////////
// HALPING
////////////////////////////////////////////////////////////////////////

void led_on(size_t lednum) {
  digitalWrite(LED_HIGH[lednum], HIGH);
  digitalWrite(LED_LOW[lednum], LOW);
  pinMode(LED_HIGH[lednum], OUTPUT);
  pinMode(LED_LOW[lednum], OUTPUT);
}

void led_off(uint8_t index) {
  pinMode(LED_HIGH[index], INPUT);
  pinMode(LED_LOW[index], INPUT);
  digitalWrite(LED_HIGH[index], LOW);
}

void led_all_off() {
  for (int i=5; i<10; i++) {
    pinMode(i, INPUT);
    digitalWrite(i, LOW);
  }
}

boolean is_reading_time() {
  uint32_t now = micros();
  uint32_t diff;

  // Attempt to handle under/overflow sanely
  if (now > last_reading) {
    diff = now - last_reading;
  } else {
    diff = now + (UINT32_MAX - last_reading);
  }

  if (diff > SENSE_INTERVAL_US) {
    last_reading = now;
    return true;
  } else {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////////////

void setup() {

#if SERIAL_DEBUG
  Serial.begin(SERIAL_SPEED);
#endif

  // Init charliplexing pins in high-Z state
  led_all_off();

  // Init i2C
  Wire.begin();

  // Init UV sensor
  si1145.begin();

  // More delay for supersitition?
  //delay(500);

}

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

void loop() {

  //
  // Only fetch readings once per SENSE_INTERVAL_US
  //
  if (is_reading_time()) {

    // Fetch & round sensor readings
    float uv = si1145.readUV() / 100.0;
    uvidx = (uint8_t)(0xFF & lround(uv));

#if SERIAL_DEBUG
    // Debug sensor readings
    Serial.print(F("UV "));
    Serial.print(uv, 2);
    Serial.print(F(" => "));
    Serial.println(uvidx);
#endif

  }

  //
  // Display UV index (0-11) via Charliplexed LEDs. Use a fixed time window for
  // the entire display, so that having more "dark" LEDs won't cause the
  // remaining ones to be brighter.
  //

  for (uint8_t i=0; i<NUM_LEDS; i++) {
    if (i <= uvidx) led_on(i);
    delayMicroseconds(LED_ON_US);
    if (i <= uvidx) led_off(i);
  }

}
