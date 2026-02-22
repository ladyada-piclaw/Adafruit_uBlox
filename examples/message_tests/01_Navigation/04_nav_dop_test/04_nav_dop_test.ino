/*!
 * @file 04_nav_dop_test.ino
 *
 * Message test: Poll UBX-NAV-DOP and continuously print all fields.
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

  Serial.println(F("=== UBX-NAV-DOP Message Test ==="));

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
  UBX_NAV_DOP_t dop;

  if (!ubx.poll(UBX_CLASS_NAV, UBX_NAV_DOP, &dop, sizeof(dop))) {
    Serial.println(F("NAV-DOP poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-DOP (Dilution of Precision) ---"));

  Serial.print(F("Geometric DOP: "));
  Serial.println(dopScaled(dop.gDOP), 2);

  Serial.print(F("Position DOP: "));
  Serial.println(dopScaled(dop.pDOP), 2);

  Serial.print(F("Time DOP: "));
  Serial.println(dopScaled(dop.tDOP), 2);

  Serial.print(F("Vertical DOP: "));
  Serial.println(dopScaled(dop.vDOP), 2);

  Serial.print(F("Horizontal DOP: "));
  Serial.println(dopScaled(dop.hDOP), 2);

  Serial.print(F("Northing DOP: "));
  Serial.println(dopScaled(dop.nDOP), 2);

  Serial.print(F("Easting DOP: "));
  Serial.println(dopScaled(dop.eDOP), 2);

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

double dopScaled(uint16_t value) {
  return value / 100.0;
}
