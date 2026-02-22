/*!
 * @file 07_nav_velned_test.ino
 *
 * Message test: Poll UBX-NAV-VELNED and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-VELNED Message Test ==="));

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
  UBX_NAV_VELNED_t velned;

  if (!ubx.pollNAVVELNED(&velned)) {
    Serial.println(F("NAV-VELNED poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-VELNED ---"));

  Serial.print(F("iTOW: "));
  Serial.print(velned.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("velN: "));
  Serial.print(velned.velN / 100.0, 3);
  Serial.println(F(" m/s"));

  Serial.print(F("velE: "));
  Serial.print(velned.velE / 100.0, 3);
  Serial.println(F(" m/s"));

  Serial.print(F("velD: "));
  Serial.print(velned.velD / 100.0, 3);
  Serial.println(F(" m/s"));

  Serial.print(F("speed (3D): "));
  Serial.print(velned.speed / 100.0, 3);
  Serial.println(F(" m/s"));

  Serial.print(F("gSpeed (2D): "));
  Serial.print(velned.gSpeed / 100.0, 3);
  Serial.println(F(" m/s"));

  Serial.print(F("heading: "));
  Serial.print(velned.heading * 1e-5, 2);
  Serial.println(F(" deg"));

  Serial.print(F("sAcc: "));
  Serial.print(velned.sAcc / 100.0, 3);
  Serial.println(F(" m/s"));

  Serial.print(F("cAcc: "));
  Serial.print(velned.cAcc * 1e-5, 2);
  Serial.println(F(" deg"));

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
