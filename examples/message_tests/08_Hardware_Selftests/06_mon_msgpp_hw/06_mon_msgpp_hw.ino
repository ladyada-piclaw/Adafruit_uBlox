/*!
 * @file 06_mon_msgpp_hw.ino
 * @brief Hardware test for MON-MSGPP (Message Parse/Process Status)
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

  Serial.println(F("=== MON-MSGPP Hardware Test ==="));

  if (!ddc.begin()) {
    halt(F("GPS not found on I2C"));
  }
  Serial.println(F("GPS module connected"));

  if (!ubx.begin()) {
    halt(F("UBX parser init failed"));
  }

  delay(500);
  ubx.setUBXOnly(UBX_PORT_DDC, true, 1000);

  UBX_MON_MSGPP_t msgpp;
  if (ubx.pollMonMsgpp(&msgpp)) {
    Serial.println(F("PASS: MON-MSGPP poll OK"));
    Serial.println(F("  DDC counts: UBX NMEA RTCM2 -- -- RTCM3 -- --"));
    Serial.print(F("              "));
    for (uint8_t i = 0; i < 8; i++) {
      Serial.print(msgpp.msg[0][i]);
      Serial.print(F("    "));
    }
    Serial.println();
    Serial.print(F("  DDC skipped: "));
    Serial.println(msgpp.skipped[0]);
  } else {
    Serial.println(F("FAIL: MON-MSGPP poll failed"));
  }
}

void loop() {
  UBX_MON_MSGPP_t msgpp;

  if (!ubx.pollMonMsgpp(&msgpp)) {
    Serial.println(F("MON-MSGPP poll failed (timeout)"));
    delay(5000);
    return;
  }

  Serial.print(F("DDC: UBX="));
  Serial.print(msgpp.msg[0][0]);
  Serial.print(F(" NMEA="));
  Serial.print(msgpp.msg[0][1]);
  Serial.print(F(" skip="));
  Serial.println(msgpp.skipped[0]);

  delay(5000);
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
