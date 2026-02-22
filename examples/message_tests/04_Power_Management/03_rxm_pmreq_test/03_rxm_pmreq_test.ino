/*!
 * @file 03_rxm_pmreq_test.ino
 *
 * Message test: Test UBX-RXM-PMREQ message formatting
 * NOTE: This test does NOT actually put the module to sleep!
 * It only verifies the message structures and constants.
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

  Serial.println(F("=== UBX-RXM-PMREQ Message Test ==="));

  if (!ddc.begin()) {
    halt(F("Could not connect to GPS module on I2C"));
  }
  Serial.println(F("GPS module connected on I2C"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);

  UBXSendStatus status = ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);
  if (status != UBX_SEND_SUCCESS) {
    Serial.print(F("WARNING: setUBXOnly status: "));
    Serial.println(status);
  } else {
    Serial.println(F("UBX-only mode set on DDC port"));
  }

  Serial.println();
  Serial.println(F("NOTE: This test does NOT send sleep commands!"));
  Serial.println();

  Serial.println(F("RXM-PMREQ struct sizes:"));
  Serial.print(F("  V0: "));
  Serial.print(sizeof(UBX_RXM_PMREQ_t));
  Serial.println(F(" bytes"));
  Serial.print(F("  V1: "));
  Serial.print(sizeof(UBX_RXM_PMREQ_V1_t));
  Serial.println(F(" bytes"));

  Serial.println();
  Serial.println(F("Flag definitions:"));
  Serial.print(F("  UBX_PMREQ_FLAG_BACKUP: 0x"));
  Serial.println(UBX_PMREQ_FLAG_BACKUP, HEX);
  Serial.print(F("  UBX_PMREQ_FLAG_FORCE: 0x"));
  Serial.println(UBX_PMREQ_FLAG_FORCE, HEX);

  Serial.println();
  Serial.println(F("Wakeup source definitions:"));
  Serial.print(F("  UBX_PMREQ_WAKE_UARTRX: 0x"));
  Serial.println(UBX_PMREQ_WAKE_UARTRX, HEX);
  Serial.print(F("  UBX_PMREQ_WAKE_EXTINT0: 0x"));
  Serial.println(UBX_PMREQ_WAKE_EXTINT0, HEX);
  Serial.print(F("  UBX_PMREQ_WAKE_EXTINT1: 0x"));
  Serial.println(UBX_PMREQ_WAKE_EXTINT1, HEX);
  Serial.print(F("  UBX_PMREQ_WAKE_SPICS: 0x"));
  Serial.println(UBX_PMREQ_WAKE_SPICS, HEX);

  Serial.println();
  Serial.println(F("To actually put module to sleep, call:"));
  Serial.println(F("  ubx.sendPmreq(duration_ms, UBX_PMREQ_FLAG_BACKUP);"));
  Serial.println();
}

void loop() {
  // Nothing to do - RXM-PMREQ is send-only, test is informational
  delay(10000);
  Serial.println(F("(RXM-PMREQ is a send-only message)"));
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
