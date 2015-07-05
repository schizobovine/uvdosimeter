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
#include <SPI.h>
#include <Wire.h>
#include <MicroView.h>
#include <Adafruit_SI1145.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_UART.h>
#include <SoftwareSerial.h>

const int BLUART_RX  = 5;
const int BLUART_TX  = 3;
const int BLUART_CTS = 2;
const int BLUART_RTS = 6;
const int BLUART_MOD = -1;

Adafruit_SI1145 si1145 = Adafruit_SI1145();

SoftwareSerial bluart = SoftwareSerial(BLUART_TX, BLUART_RX);

Adafruit_BluefruitLE_UART ble = Adafruit_BluefruitLE_UART(
  bluart,
  BLUART_MOD,
  BLUART_CTS,
  BLUART_RTS
);

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
    uView.println("Could not find Si1145!");
    uView.display();
    delay(5000);
  }

  // Initialize Bluetooth module
  while (!ble.begin()) {
    uView.clear(PAGE);
    uView.setCursor(0, 0);
    uView.println("Could not find BlUART!");
    uView.display();
    delay(5000);
  }

  // Initialize Bluetooth module
  while (!ble.factoryReset()) {
    uView.clear(PAGE);
    uView.setCursor(0, 0);
    uView.println("Could not find reset BluART!");
    uView.display();
    delay(5000);
  }

  // Wait for pairing
  //while (!ble.isConnected()) {
  //  delay(1000);
  //}

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

void printBluetooth() {

  PGM_P FMT = PSTR("%#6.1f,%#6.1f,%#6.1f");
  char buff[64];
  snprintf_P(buff, sizeof(buff), FMT, visible, ir, uv);
  buff[sizeof(buff) - 1] = '\0';

  ble.println(buff);

}

void loop() {
  
  visible = si1145.readVisible();
  ir = si1145.readIR();
  uv = si1145.readUV();

  printDisplay();
  //printBluetooth();

  delay(1000);

}
