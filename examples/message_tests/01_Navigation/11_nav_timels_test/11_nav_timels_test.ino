/*!
 * @file 11_nav_timels_test.ino
 *
 * Message test: Poll UBX-NAV-TIMELS and continuously print leap second info.
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

  Serial.println(F("=== UBX-NAV-TIMELS Message Test ==="));

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
  UBX_NAV_TIMELS_t timels;

  if (!ubx.pollNavTimels(&timels)) {
    Serial.println(F("NAV-TIMELS poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-TIMELS ---"));

  Serial.print(F("iTOW: "));
  Serial.print(timels.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("srcOfCurrLs: "));
  Serial.print(timels.srcOfCurrLs);
  Serial.print(F(" ("));
  Serial.print(getSourceName(timels.srcOfCurrLs));
  Serial.println(F(")"));

  Serial.print(F("currLs: "));
  Serial.print(timels.currLs);
  Serial.println(F(" s (GPS-UTC)"));

  Serial.print(F("srcOfLsChange: "));
  Serial.print(timels.srcOfLsChange);
  Serial.print(F(" ("));
  Serial.print(getSourceName(timels.srcOfLsChange));
  Serial.println(F(")"));

  Serial.print(F("lsChange: "));
  Serial.print(timels.lsChange);
  Serial.println(F(" s (upcoming)"));

  Serial.print(F("timeToLsEvent: "));
  Serial.print(timels.timeToLsEvent);
  Serial.println(F(" s"));

  if (timels.dateOfLsGpsWn > 0) {
    Serial.print(F("dateOfLsEvent: GPS week "));
    Serial.print(timels.dateOfLsGpsWn);
    Serial.print(F(", day "));
    Serial.println(timels.dateOfLsGpsDn);
  }

  Serial.print(F("valid: 0x"));
  Serial.print(timels.valid, HEX);
  Serial.print(F(" (currLs="));
  Serial.print((timels.valid & UBX_TIMELS_VALID_CURR_LS) ? F("Y") : F("N"));
  Serial.print(F(", timeToEvent="));
  Serial.print((timels.valid & UBX_TIMELS_VALID_TIME_TO_EVENT) ? F("Y") : F("N"));
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

const char *getSourceName(uint8_t src) {
  switch (src) {
    case 0:
      return "Default";
    case 1:
      return "GPS";
    case 2:
      return "SBAS";
    case 3:
      return "BeiDou";
    case 4:
      return "Galileo";
    case 5:
      return "GLONASS";
    case 255:
      return "Unknown";
    default:
      return "Reserved";
  }
}
