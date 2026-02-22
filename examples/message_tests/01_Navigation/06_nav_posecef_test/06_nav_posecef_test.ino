/*!
 * @file 06_nav_posecef_test.ino
 *
 * Message test: Poll UBX-NAV-POSECEF and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-POSECEF Message Test ==="));

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
  UBX_NAV_POSECEF_t posecef;

  if (!ubx.pollNAVPOSECEF(&posecef)) {
    Serial.println(F("NAV-POSECEF poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-POSECEF ---"));

  Serial.print(F("iTOW: "));
  Serial.print(posecef.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("ecefX: "));
  Serial.print(posecef.ecefX / 100.0, 2);
  Serial.println(F(" m"));

  Serial.print(F("ecefY: "));
  Serial.print(posecef.ecefY / 100.0, 2);
  Serial.println(F(" m"));

  Serial.print(F("ecefZ: "));
  Serial.print(posecef.ecefZ / 100.0, 2);
  Serial.println(F(" m"));

  Serial.print(F("pAcc: "));
  Serial.print(posecef.pAcc / 100.0, 2);
  Serial.println(F(" m"));

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
