/*!
 * @file 03_mon_hw2_hw.ino
 * @brief Hardware test for MON-HW2 (Extended Hardware Status)
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

  Serial.println(F("=== MON-HW2 Hardware Test ==="));

  if (!ddc.begin()) {
    halt(F("GPS not found on I2C"));
  }
  Serial.println(F("GPS module connected"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);

  UBX_MON_HW2_t hw2;
  if (ubx.pollMonHw2(&hw2)) {
    Serial.println(F("PASS: MON-HW2 poll OK"));
    Serial.print(F("  ofsI="));
    Serial.print(hw2.ofsI);
    Serial.print(F(" magI="));
    Serial.println(hw2.magI);
    Serial.print(F("  ofsQ="));
    Serial.print(hw2.ofsQ);
    Serial.print(F(" magQ="));
    Serial.println(hw2.magQ);
    Serial.print(F("  Config: "));
    Serial.println(cfgSrc(hw2.cfgSource));
    Serial.print(F("  POST: 0x"));
    Serial.println(hw2.postStatus, HEX);
  } else {
    Serial.println(F("FAIL: MON-HW2 poll failed"));
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

  Serial.print(F("I("));
  Serial.print(hw2.ofsI);
  Serial.print(F(","));
  Serial.print(hw2.magI);
  Serial.print(F(") Q("));
  Serial.print(hw2.ofsQ);
  Serial.print(F(","));
  Serial.print(hw2.magQ);
  Serial.print(F(") cfg="));
  Serial.println(cfgSrc(hw2.cfgSource));

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

const char *cfgSrc(uint8_t s) {
  switch (s) {
    case 114:
      return "ROM";
    case 111:
      return "OTP";
    case 112:
      return "Pins";
    case 102:
      return "Flash";
    default:
      return "???";
  }
}
