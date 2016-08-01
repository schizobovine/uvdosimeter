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
#include <Adafruit_SI1145.h>
#include <Adafruit_VEML6070.h>
#include <VEML6075.h>
#include <SD.h>
#include <RTClib.h>
#include <DS3231.h>

//#define USE_BLE 0

#ifdef USE_BLE
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#endif

#define LOG_FILENAME "uv_raw.log"

const uint8_t SD_CS     = 10;
const uint8_t LOGMODE   = O_WRITE | O_CREAT | O_APPEND;

const uint8_t BLE_CS    = 8;
const uint8_t BLE_IRQ   = 7;
const uint8_t BLE_RST   = 4;

const uint8_t GUVA12_PIN = A2;
const uint8_t ML8511_PIN = A1;

Adafruit_SI1145 si1145 = Adafruit_SI1145();
Adafruit_VEML6070 veml6070 = Adafruit_VEML6070();
VEML6075 veml6075 = VEML6075();
RTC_DS3231 rtc = RTC_DS3231();

#ifdef USE_BLE
Adafruit_BluefruitLE_SPI modem(BLE_CS, BLE_IRQ, BLE_RST);
#endif

File logfile;
uint8_t lastmin = 0;

uint16_t guva_reading;

float si1145_index = 0.0;
uint16_t si1145_vis = 0;
uint16_t si1145_ir = 0;

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
  Serial.println(msg);
  if (pause > 0) {
    delay(pause);
  }
}

float getUVIndexGUVA12(uint16_t uv_reading) {
  return (uv_reading / 1024.0 * 5.0) / 0.1;
}

void timestamp(DateTime *dt, Stream *f) {
  if (dt && f) {
    String s;
    dt->iso8601(s);
    f->print(s);
  }
}

void logData(DateTime *dt, Stream *out) {
  if (out) {

    timestamp(dt, out);
    out->print(F(","));

    out->print(guva_reading, 1);                      out->print(F(","));
    out->print(getUVIndexGUVA12(guva_reading), 1);    out->print(F(","));

    out->print(si1145_index, 1);                      out->print(F(","));
    out->print(si1145_vis);                           out->print(F(","));
    out->print(si1145_ir);                            out->print(F(","));

    out->print(veml6070_raw_uv);                      out->print(F(","));
    out->print(veml6070_index, 1);                    out->print(F(","));

    out->print(veml6075_raw_uva);                     out->print(F(","));
    out->print(veml6075_raw_uvb);                     out->print(F(","));
    out->print(veml6075_raw_dark);                    out->print(F(","));
    out->print(veml6075_raw_vis);                     out->print(F(","));
    out->print(veml6075_raw_ir);                      out->print(F(","));
    out->print(veml6075_index, 1);                    out->print(F(","));

    out->println();

  }
}

void setup() {

  // Setup serial
  while (!Serial)
    ;
  Serial.begin(115200);
  Serial.println(F("Hello, UV dosimeter tester starting..."));

  // Setup GUVA-S12D & ML8511
  pinMode(GUVA12_PIN, INPUT);

  // Initialize i2c bus
  Wire.begin();

  // Init RTC
  rtc.begin();

  // Setup Si1145
  si1145.begin();

  // Setup VEML6070
  veml6070.begin(VEML6070_1_T);

  // Setup VEML6075
  veml6075.begin();

  // Open logfile
  pinMode(SD_CS, OUTPUT);
  if (!SD.begin(SD_CS)) {
    error("MicroSD not found!");
  } else if (!(logfile = SD.open(F(LOG_FILENAME), LOGMODE))) {
    error("Could not open " LOG_FILENAME "!");
  }

#ifdef USE_BLE
  // Setup BLE modem
  if (!modem.begin(false)) {
    error("Starting BLE modem failed!");
  }
#endif

}

void getRawData() {

  // Read analog sensors
  guva_reading = analogRead(GUVA12_PIN);

  // Read Si1145
  si1145_index = si1145.readUV();
  si1145_vis = si1145.readVisible();
  si1145_ir = si1145.readIR();

  // Read VEML6070
  veml6070_raw_uv = veml6070.readUV();
  veml6070_index = veml6070.getUVIndex();

  // Read VEML6075
  veml6075.poll();
  veml6075_raw_uva = veml6075.getRawUVA();
  veml6075_raw_uvb = veml6075.getRawUVA();
  veml6075_raw_dark = veml6075.getRawDark();
  veml6075_raw_vis = veml6075.getRawVisComp();
  veml6075_raw_ir = veml6075.getRawIRComp();
  veml6075_uva = veml6075.getUVA();
  veml6075_uvb = veml6075.getUVB();
  veml6075_index = veml6075.getUVIndex();

}

void loop() {

  // Get current time
  DateTime now = rtc.now();

  // Refresh data
  getRawData();

  // Display results over serial
  logData(&now, &Serial);

#ifdef USE_BLE
  // Display results over BLE
  if (modem.isConnected()) {
    logData(&now, &modem);
  }
#endif

  // Write out data to SD card every minute or so
  if (now.minute() != lastmin) {
    lastmin = now.minute();
    logData(&now, &logfile);
    logfile.flush();
  }

  // Log to file every minute
  delay(1000);

}
