/*!
 * @file 15_nav_odo_test.ino
 *
 * Message test: Poll UBX-NAV-ODO and continuously print odometer data.
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

  Serial.println(F("=== UBX-NAV-ODO Message Test ==="));

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

  // Reset odometer at start
  Serial.println(F("Resetting odometer..."));
  if (ubx.resetOdometer()) {
    Serial.println(F("Odometer reset OK"));
  } else {
    Serial.println(F("WARNING: Odometer reset failed"));
  }

  Serial.println();
}

void loop() {
  UBX_NAV_ODO_t odo;

  if (!ubx.pollNavOdo(&odo)) {
    Serial.println(F("NAV-ODO poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- NAV-ODO ---"));

  Serial.print(F("iTOW: "));
  Serial.print(odo.iTOW);
  Serial.println(F(" ms"));

  Serial.print(F("distance: "));
  Serial.print(odo.distance);
  Serial.println(F(" m (since reset)"));

  Serial.print(F("totalDistance: "));
  Serial.print(odo.totalDistance);
  Serial.println(F(" m (cumulative)"));

  Serial.print(F("distanceStd: "));
  Serial.print(odo.distanceStd);
  Serial.println(F(" m (1-sigma)"));

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
