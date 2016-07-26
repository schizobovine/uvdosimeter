/*
 * uv-sensor-testing.ino
 *
 * Summary: Test and compare various UV sensors using a SparkFun MicroView OLED
 *          display/microcontroller.
 *
 *          Devices under test:
 *
 *          - VEML6070  (UVA, i2c, Adafruit)
 *          - GUVA-S12D (UVA, analog, Adafruit)
 *          - Si1145 (not-really-UV, i2c, Adafruit)
 *          - VEML6075  (UVA/UVB, i2c, Tindie/Pesky Products)
 *
 * Author:  Sean Caulfield <sean@yak.net>
 * License: GPLv2.0
 *
 */

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SI1145.h>
#include <Adafruit_VEML6070.h>
#include <VEML6075.h>
#include <SD.h>
#include <RTClib.h>
#include <DS3231.h>

const uint8_t SD_CS     = 10;
const uint8_t LOGMODE   = O_WRITE | O_CREAT | O_APPEND;
const uint8_t OLED_RST  = 6;
//#define LOG_FILENAME "uvsensors.log"
//#define LOG_RAW_FILENAME "uv_raw.log"
#define LOG_FILENAME "uv_raw.log"

Adafruit_SI1145 si1145 = Adafruit_SI1145();
Adafruit_VEML6070 veml6070 = Adafruit_VEML6070();
VEML6075 veml6075 = VEML6075();

RTC_DS3231 rtc = RTC_DS3231();
Adafruit_SSD1306 display(OLED_RST);

File logfile;
uint8_t lastmin = 0;

uint16_t guva_reading;
float si1145_index = 0.0;
float veml6070_index = 0.0;
float veml6075_uva = 0.0;
float veml6075_uvb = 0.0;
float veml6075_index = 0.0;

uint16_t veml6070_raw_uv = 0;
uint16_t veml6075_raw_uva = 0;
uint16_t veml6075_raw_uvb = 0;
uint16_t veml6075_raw_dark = 0;
uint16_t veml6075_raw_vis = 0;
uint16_t veml6075_raw_ir = 0;

void error(const char *msg, int pause=1000) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(msg);
  display.display();
  if (pause > 0) {
    delay(pause);
  }
}

float getUVIndex(uint16_t uv_reading) {
  return (uv_reading / 1024.0 * 5.0) / 0.1;
}

void refreshDisplay(DateTime *dt) {

  display.clearDisplay();

  display.setCursor(0, 0);
  display.print(F("GUVA "));
  display.setCursor(32, 0);
  display.print(getUVIndex(guva_reading), 1);

  display.setCursor(0, 8);
  display.print(F("SI45 "));
  display.setCursor(32, 8);
  display.print(si1145_index, 1);

  display.setCursor(0, 16);
  display.print(F("VL70 "));
  display.setCursor(32, 16);
  display.print(veml6070_index, 1);

  display.setCursor(0, 24);
  display.print(F("VL75 "));
  display.setCursor(32, 24);
  display.print(veml6075_index, 1);

  display.setCursor(64, 0);
  display.print(dt->year());
  if (dt->month() < 10) {
    display.print(F("-0"));
  } else {
    display.print(F("-"));
  }
  display.print(dt->month());
  if (dt->day() < 10) {
    display.print(F("-0"));
  } else {
    display.print(F("-"));
  }
  display.print(dt->day());

  display.setCursor(64, 8);
  if (dt->hour() < 10) {
    display.print(F("0"));
  }
  display.print(dt->hour());
  if (dt->minute() < 10) {
    display.print(F(":0"));
  } else {
    display.print(F(":"));
  }
  display.print(dt->minute());
  if (dt->second() < 10) {
    display.print(F(":0"));
  } else {
    display.print(F(":"));
  }
  display.print(dt->second());

  display.display();

}

void timestamp(DateTime *dt, File *f) {
  if (dt && f) {

    f->print(dt->year());
    if (dt->month() < 10) {
      f->print(F("-0"));
    } else {
      f->print(F("-"));
    }
    f->print(dt->month(), 2);
    if (dt->day() < 10) {
      f->print(F("-0"));
    } else {
      f->print(F("-"));
    }
    f->print(dt->day());

    f->print(F("T"));

    if (dt->hour() < 10) {
      f->print(F("0"));
    }
    f->print(dt->hour());
    if (dt->minute() < 10) {
      f->print(F(":0"));
    } else {
      f->print(F(":"));
    }
    f->print(dt->minute());
    if (dt->second() < 10) {
      f->print(F(":0"));
    } else {
      f->print(F(":"));
    }
    f->print(dt->second());

  }
}

void logRawData(DateTime *dt) {
  if (logfile) {
    timestamp(dt, &logfile);
    logfile.print(F(","));
    logfile.print(guva_reading, 1);     logfile.print(F(","));
    logfile.print(si1145_index, 1);   logfile.print(F(","));
    logfile.print(veml6070_raw_uv, 1); logfile.print(F(","));
    logfile.print(veml6075_raw_uva, 1);   logfile.print(F(","));
    logfile.print(veml6075_raw_uvb, 1);   logfile.print(F(","));
    logfile.print(veml6075_raw_dark, 1); logfile.println(F(","));
    logfile.print(veml6075_raw_vis, 1); logfile.println(F(","));
    logfile.print(veml6075_raw_ir, 1); logfile.println(F(","));
  }
}

void logData(DateTime *dt) {
  if (logfile) {
    timestamp(dt, &logfile);
    logfile.print(F(","));
    logfile.print(guva_reading, 1);     logfile.print(F(","));
    logfile.print(si1145_index, 1);   logfile.print(F(","));
    logfile.print(veml6070_index, 1); logfile.print(F(","));
    logfile.print(veml6075_uva, 1);   logfile.print(F(","));
    logfile.print(veml6075_uvb, 1);   logfile.print(F(","));
    logfile.print(veml6075_index, 1); logfile.println(F(","));
  }
}

void setup() {

  // Initialize i2c bus
  Wire.begin();

  // Init RTC
  rtc.begin();

  // Setup display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();

  // Setup GUVA-S12D
  pinMode(A1, INPUT);

  // Setup Si1145
  si1145.begin();

  // Setup VEML6070
  veml6070.begin(VEML6070_1_T);

  // Setup VEML6075
  veml6075.begin();

  // Open logfile
  pinMode(SD_CS, OUTPUT);
  //if (!SD.begin(SD_CS, SD_MOSI, SD_MISO, SD_SCK)) {
  if (!SD.begin(SD_CS)) {
    error("MicroSD not found!");
  } else if (!(logfile = SD.open(F(LOG_FILENAME), LOGMODE))) {
    error("Could not open " LOG_FILENAME "!");
  //} else if (!(lograw = SD.open(F(LOG_RAW_FILENAME), LOGMODE))) {
  //  error("Could not open " LOG_RAW_FILENAME "!");
  }

}

void getRawData() {

  veml6070_raw_uv = veml6070.readUV();

  veml6075_raw_uva = veml6075.getRawUVA();
  veml6075_raw_uvb = veml6075.getRawUVA();
  veml6075_raw_dark = veml6075.getRawDark();
  veml6075_raw_vis = veml6075.getRawVisComp();
  veml6075_raw_ir = veml6075.getRawIRComp();

}

void loop() {

  //
  // Get current time
  //
  DateTime now = rtc.now();

  getRawData();
  logRawData(&now);

  // Read GUVA-S12D
  guva_reading = analogRead(A1);

  // Read Si1145
  si1145_index = si1145.readUV();

  // Read VEML6070
  veml6070_index = veml6070.getUVIndex();

  // Read VEML6075
  veml6075.poll();
  veml6075_uva = veml6075.getUVA();
  veml6075_uvb = veml6075.getUVB();
  veml6075_index = veml6075.getUVIndex();

  //
  // Display results
  //
  refreshDisplay(&now);

  //
  // Log to file every minute
  //
  if (now.minute() != lastmin) {
    lastmin = now.minute();
    logData(&now);
  }

  delay(1000);

}
