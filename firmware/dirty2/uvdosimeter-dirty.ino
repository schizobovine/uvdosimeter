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
#include <SD.h>
#include <RTClib.h>
#include <DS3231.h>
#include <Time.h>
#include <Timezone.h>

#define SERIAL_TIME_SET 0

const uint8_t SD_CS   = 6;
const uint8_t SD_MOSI = 5;
const uint8_t SD_MISO = 3;
const uint8_t SD_SCK  = 2;
const uint8_t LOGMODE = O_WRITE | O_CREAT | O_APPEND;

Adafruit_SI1145 si1145 = Adafruit_SI1145();
File logfile;
RTC_DS3231 rtc = RTC_DS3231();
uint8_t lastmin = 0;

/*
 * Set time based on UNIX timestamps shat out on the serial port.
 */
#if SERIAL_TIME_SET
void timeset() {
  time_t newnow = 0;

  Serial.begin(9600);
  Serial.setTimeout(3000);

  uView.clear(PAGE);
  uView.setCursor(0, 0);
  uView.println("timeset");
  uView.display();

  if (Serial.find("T")) {
    newnow = Serial.parseInt();
  }

  if (newnow != 0) {
    rtc.adjust(DateTime(newnow));
  }

  uView.println(newnow);
  uView.display();
  delay(1000);

}
#endif

/*
 * Barf error info to microview display.
 */
void error(const char *msg, int pause=5000) {
  uView.clear(PAGE);
  uView.setCursor(0, 0);
  uView.println(msg);
  uView.display();
  delay(pause);
}

void setup() {
  
  // Turn out display and print out default image
  uView.begin();
  uView.clear(ALL);
  uView.setFontType(0);
  uView.display();

  // Find our UV sensor
  Wire.begin();
  while (!si1145.begin()) {
    error("Could not find UV Sensor!");
  }

  // Initialize RTC
  rtc.begin();
  rtc.clearControlRegisters();

  // Initialize SD card for logging
  pinMode(SD_CS, OUTPUT);
  while (!SD.begin(SD_CS, SD_MOSI, SD_MISO, SD_SCK)) {
    error("Could not connect microSD card!");
  }
  logfile = SD.open("uvdose.log", LOGMODE);
  while (!logfile) {
    error("Error opening uvdose.log!");
  }
  
  delay(500);

#if SERIAL_TIME_SET
  timeset();
#endif

}

void printDisplay(float visible, float ir, float uv, DateTime *dt) {

  PGM_P FMT = PSTR(
     "UV Dosimtr"
     "----------"
     "Vis %#6.1f"
     "IR  %#6.1f"
     "UV  %#6.1f"
     " %02d:%02d:%02d"
  );

  char buff[64];
  snprintf_P(buff, sizeof(buff), FMT,
      visible, ir, uv,
      dt->hour(),
      dt->minute(),
      dt->second()
  );
  buff[sizeof(buff) - 1] = '\0';

  uView.clear(PAGE);
  uView.setCursor(0, 0);
  uView.print(buff);
  uView.display();

}

void logToFile(float visible, float ir, float uv, DateTime *dt) {

  PGM_P FMT = PSTR(
     "%04d-%02d-%02d "
     "%02d:%02d:%02d,"
     "%#6.1f,"
     "%#6.1f,"
     "%#6.1f,"
     "\n"
  );

  char buff[64];
  snprintf_P(buff, sizeof(buff), FMT,
      dt->year(),
      dt->month(),
      dt->day(),
      dt->hour(),
      dt->minute(),
      dt->second(),
      visible, ir, uv
  );
  buff[sizeof(buff) - 1] = '\0';

  logfile.write(buff);
  logfile.flush();

}

void loop() {
  
  float visible = si1145.readVisible();
  float ir = si1145.readIR();
  float uv = si1145.readUV();
  DateTime now = rtc.now();

  printDisplay(visible, ir, uv, &now);
  if (now.minute() != lastmin) {
    lastmin = now.minute();
    logToFile(visible, ir, uv, &now);
  }

  delay(1000);

}
