/*!
 * @file 11_rxm_pmreq_hw.ino
 * @brief Hardware test for RXM-PMREQ (Power Management Request)
 * NOTE: This test only validates struct sizes, does NOT sleep the module!
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

  Serial.println(F("=== RXM-PMREQ Hardware Test ==="));
  Serial.println(F("NOTE: Not sending sleep commands!"));

  if (!ddc.begin()) {
    halt(F("GPS not found on I2C"));
  }
  Serial.println(F("GPS module connected"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);

  if (sizeof(UBX_RXM_PMREQ_t) == 8) {
    Serial.println(F("PASS: v0 struct size = 8"));
  } else {
    Serial.println(F("FAIL: v0 struct size wrong"));
  }

  if (sizeof(UBX_RXM_PMREQ_V1_t) == 16) {
    Serial.println(F("PASS: v1 struct size = 16"));
  } else {
    Serial.println(F("FAIL: v1 struct size wrong"));
  }

  UBX_NAV_PVT_t pvt;
  if (ubx.poll(UBX_CLASS_NAV, UBX_NAV_PVT, &pvt, sizeof(pvt))) {
    Serial.println(F("PASS: Module still responding"));
  } else {
    Serial.println(F("WARN: NAV-PVT poll failed"));
  }

  Serial.println(F("\nTo sleep module, call:"));
  Serial.println(F("  ubx.sendPmreq(5000, UBX_PMREQ_FLAG_BACKUP);"));
}

void loop() {
  delay(1000);
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
