/*!
 * @file 03_mon_hw2_test.ino
 *
 * Message test: Poll UBX-MON-HW2 (Extended Hardware Status)
 * Displays IQ imbalance, config source, and POST status.
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

  Serial.println(F("=== UBX-MON-HW2 Message Test ==="));

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
}

void loop() {
  UBX_MON_HW2_t hw2;

  if (!ubx.pollMonHw2(&hw2)) {
    Serial.println(F("MON-HW2 poll failed (timeout)"));
    delay(2000);
    return;
  }

  Serial.println(F("--- MON-HW2 ---"));

  Serial.println(F("IQ Imbalance:"));
  Serial.print(F("  ofsI: "));
  Serial.print(hw2.ofsI);
  Serial.print(F(", magI: "));
  Serial.println(hw2.magI);
  Serial.print(F("  ofsQ: "));
  Serial.print(hw2.ofsQ);
  Serial.print(F(", magQ: "));
  Serial.println(hw2.magQ);

  Serial.print(F("Config source: "));
  Serial.print(hw2.cfgSource);
  Serial.print(F(" ("));
  Serial.print(getConfigSourceName(hw2.cfgSource));
  Serial.println(F(")"));

  Serial.print(F("Low-level config: 0x"));
  Serial.println(hw2.lowLevCfg, HEX);

  Serial.print(F("POST status: 0x"));
  Serial.println(hw2.postStatus, HEX);

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

const char *getConfigSourceName(uint8_t src) {
  switch (src) {
    case UBX_MON_HW2_CFG_ROM:
      return "ROM";
    case UBX_MON_HW2_CFG_OTP:
      return "OTP";
    case UBX_MON_HW2_CFG_PINS:
      return "Pins";
    case UBX_MON_HW2_CFG_FLASH:
      return "Flash";
    default:
      return "Unknown";
  }
}
