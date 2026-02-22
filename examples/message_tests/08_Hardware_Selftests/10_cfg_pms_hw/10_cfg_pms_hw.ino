/*!
 * @file 10_cfg_pms_hw.ino
 * @brief Hardware test for CFG-PMS (Power Mode Setup)
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

  Serial.println(F("=== CFG-PMS Hardware Test ==="));

  if (!ddc.begin()) {
    halt(F("GPS not found on I2C"));
  }
  Serial.println(F("GPS module connected"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);

  UBX_CFG_PMS_t pms;
  if (ubx.pollCfgPms(&pms)) {
    Serial.println(F("PASS: CFG-PMS poll OK"));
    Serial.print(F("  Mode: "));
    Serial.println(modeName(pms.powerSetupValue));
  } else {
    Serial.println(F("FAIL: CFG-PMS poll failed"));
  }

  if (ubx.setPowerMode(UBX_PMS_FULLPOWER)) {
    Serial.println(F("PASS: setPowerMode(FULL) OK"));
  } else {
    Serial.println(F("FAIL: setPowerMode(FULL) failed"));
  }
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

const char *modeName(uint8_t m) {
  switch (m) {
    case 0:
      return "Full";
    case 1:
      return "Balanced";
    case 2:
      return "Interval";
    case 3:
      return "Aggr1Hz";
    case 4:
      return "Aggr2Hz";
    case 5:
      return "Aggr4Hz";
    default:
      return "???";
  }
}
