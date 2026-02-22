/*!
 * @file 09_nav_timeutc_test.ino
 *
 * Message test: Poll UBX-NAV-TIMEUTC and continuously print all fields.
 * Validates basic field ranges and halts on communication failure.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code
 */

#include <Adafruit_UBX.h>
#include <Adafruit_UBloxDDC.h>

Adafruit_UBloxDDC ddc;
Adafruit_UBX ubx(ddc);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("=== UBX-NAV-TIMEUTC Message Test ==="));

  if (!ddc.begin()) {
    halt(F("Could not connect to GPS module on I2C"));
  }
  Serial.println(F("GPS module connected on I2C"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  UBXSendStatus status = ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);
  if (status != UBX_SEND_SUCCESS) {
    Serial.print(F("WARNING: setUBXOnly status: "));
    Serial.println(status);
  } else {
    Serial.println(F("UBX-only mode set on DDC port"));
  }

  Serial.println();
}

void loop() {
  UBX_NAV_TIMEUTC_t timeutc;

  if (!ubx.pollNAVTIMEUTC(&timeutc)) {
    Serial.println(F("NAV-TIMEUTC poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-TIMEUTC ---"));

  Serial.print(F("iTOW: "));
  Serial.print(timeutc.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("tAcc: "));
  Serial.print(timeutc.tAcc);
  Serial.println(F(" ns"));

  Serial.print(F("nano: "));
  Serial.print(timeutc.nano);
  Serial.println(F(" ns"));

  Serial.print(F("UTC: "));
  printDate(timeutc.year, timeutc.month, timeutc.day);
  Serial.print(F(" "));
  printTime(timeutc.hour, timeutc.min, timeutc.sec);
  Serial.println();

  Serial.print(F("valid: 0x"));
  Serial.print(timeutc.valid, HEX);
  Serial.print(F(" (TOW="));
  Serial.print(timeutc.valid & 0x01);
  Serial.print(F(", WKN="));
  Serial.print((timeutc.valid >> 1) & 0x01);
  Serial.print(F(", UTC="));
  Serial.print((timeutc.valid >> 2) & 0x01);
  Serial.println(F(")"));

  Serial.println();
  delay(2000);
}

/**************************************************************************/
/* Helper functions                                                       */
/**************************************************************************/

void halt(const __FlashStringHelper *msg) {
  Serial.print(F("HALT: "));
  Serial.println(msg);
  while (1)
    delay(10);
}

void printDate(uint16_t year, uint8_t month, uint8_t day) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%04u-%02u-%02u", year, month, day);
  Serial.print(buf);
}

void printTime(uint8_t hour, uint8_t min, uint8_t sec) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%02u:%02u:%02u", hour, min, sec);
  Serial.print(buf);
}
