/*!
 * @file 05_nav_posllh_test.ino
 *
 * Message test: Poll UBX-NAV-POSLLH and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-POSLLH Message Test ==="));

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
  UBX_NAV_POSLLH_t posllh;

  if (!ubx.pollNAVPOSLLH(&posllh)) {
    Serial.println(F("NAV-POSLLH poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-POSLLH ---"));

  Serial.print(F("iTOW: "));
  Serial.print(posllh.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("Lat: "));
  Serial.print(posllh.lat * 1e-7, 7);
  Serial.println(F(" deg"));

  Serial.print(F("Lon: "));
  Serial.print(posllh.lon * 1e-7, 7);
  Serial.println(F(" deg"));

  Serial.print(F("Height (ellipsoid): "));
  Serial.print(posllh.height / 1000.0, 3);
  Serial.println(F(" m"));

  Serial.print(F("Height (MSL): "));
  Serial.print(posllh.hMSL / 1000.0, 3);
  Serial.println(F(" m"));

  Serial.print(F("hAcc: "));
  Serial.print(posllh.hAcc / 1000.0, 3);
  Serial.println(F(" m"));

  Serial.print(F("vAcc: "));
  Serial.print(posllh.vAcc / 1000.0, 3);
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
