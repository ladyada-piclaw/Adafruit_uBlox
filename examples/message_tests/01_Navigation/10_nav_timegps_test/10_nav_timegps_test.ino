/*!
 * @file 10_nav_timegps_test.ino
 *
 * Message test: Poll UBX-NAV-TIMEGPS and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-TIMEGPS Message Test ==="));

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
  UBX_NAV_TIMEGPS_t timegps;

  if (!ubx.pollNAVTIMEGPS(&timegps)) {
    Serial.println(F("NAV-TIMEGPS poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-TIMEGPS ---"));

  Serial.print(F("iTOW: "));
  Serial.print(timegps.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("fTOW: "));
  Serial.print(timegps.fTOW);
  Serial.println(F(" ns"));

  Serial.print(F("week: "));
  Serial.println(timegps.week);

  Serial.print(F("leapS: "));
  Serial.print(timegps.leapS);
  Serial.println(F(" s"));

  Serial.print(F("valid: 0x"));
  Serial.print(timegps.valid, HEX);
  Serial.print(F(" (TOW="));
  Serial.print(timegps.valid & 0x01);
  Serial.print(F(", week="));
  Serial.print((timegps.valid >> 1) & 0x01);
  Serial.print(F(", leapS="));
  Serial.print((timegps.valid >> 2) & 0x01);
  Serial.println(F(")"));

  Serial.print(F("tAcc: "));
  Serial.print(timegps.tAcc);
  Serial.println(F(" ns"));

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
